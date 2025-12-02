#include "deviceconnection.h"
#include "scpicommands.h"
#include <QThread>

DeviceConnection::DeviceConnection(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_reconnectTimer(new QTimer(this))
{
    connect(m_socket, &QTcpSocket::connected, this, &DeviceConnection::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &DeviceConnection::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &DeviceConnection::onReadyRead);
    connect(m_socket, &QAbstractSocket::errorOccurred, this, &DeviceConnection::onErrorOccurred);
    connect(m_reconnectTimer, &QTimer::timeout, this, &DeviceConnection::onReconnectTimer);
    
    m_reconnectTimer->setInterval(3000);
}

DeviceConnection::~DeviceConnection()
{
    disconnectFromDevice();
}

void DeviceConnection::connectToDevice(const QString &host, quint16 port)
{
    if (m_state == ConnectionState::Connected || m_state == ConnectionState::Connecting) {
        return;
    }
    
    m_host = host;
    m_port = port;
    m_reconnectAttempts = 0;
    
    setState(ConnectionState::Connecting);
    m_socket->connectToHost(host, port);
    
    // Set connection timeout
    QTimer::singleShot(m_connectionTimeout, this, [this]() {
        if (m_state == ConnectionState::Connecting) {
            m_socket->abort();
            setState(ConnectionState::Error);
            emit errorOccurred("Connection timeout");
        }
    });
}

void DeviceConnection::disconnectFromDevice()
{
    m_reconnectTimer->stop();
    m_autoReconnect = false;
    
    if (m_socket->isOpen()) {
        m_socket->disconnectFromHost();
    }
    
    setState(ConnectionState::Disconnected);
}

void DeviceConnection::sendCommand(const QString &command)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to device");
        return;
    }
    
    QMutexLocker locker(&m_mutex);
    QByteArray data = (command + "\n").toUtf8();
    m_socket->write(data);
    m_socket->flush();
}

QString DeviceConnection::sendQuery(const QString &query, int timeoutMs)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to device");
        return QString();
    }
    
    QMutexLocker locker(&m_mutex);
    
    // Clear buffer before query
    m_receiveBuffer.clear();
    
    // Send query
    QByteArray data = (query + "\n").toUtf8();
    m_socket->write(data);
    m_socket->flush();
    
    // Wait for response
    QString response;
    int elapsed = 0;
    const int pollInterval = 10;
    
    while (elapsed < timeoutMs) {
        if (m_socket->waitForReadyRead(pollInterval)) {
            m_receiveBuffer += m_socket->readAll();
            
            // Check if we have a complete response (ends with newline)
            int newlinePos = m_receiveBuffer.indexOf('\n');
            if (newlinePos >= 0) {
                response = QString::fromUtf8(m_receiveBuffer.left(newlinePos)).trimmed();
                m_receiveBuffer.remove(0, newlinePos + 1);
                break;
            }
        }
        elapsed += pollInterval;
    }
    
    return response;
}

void DeviceConnection::sendCommandAsync(const QString &command, std::function<void(const QString&)> callback)
{
    AsyncCommand cmd;
    cmd.command = command;
    cmd.callback = callback;
    
    m_asyncQueue.enqueue(cmd);
    
    // Process queue if not already waiting
    if (!m_waitingForResponse && isConnected()) {
        // Send next command
        if (!m_asyncQueue.isEmpty()) {
            AsyncCommand next = m_asyncQueue.head();
            sendCommand(next.command);
            m_waitingForResponse = (next.callback != nullptr);
        }
    }
}

QByteArray DeviceConnection::readBinaryData(int expectedSize, int timeoutMs)
{
    if (!isConnected()) {
        return QByteArray();
    }
    
    QMutexLocker locker(&m_mutex);
    
    int elapsed = 0;
    const int pollInterval = 10;
    
    while (elapsed < timeoutMs) {
        if (m_socket->waitForReadyRead(pollInterval)) {
            m_receiveBuffer += m_socket->readAll();
            
            // Check for IEEE 488.2 binary block format: #<digit><length><data>
            if (m_receiveBuffer.startsWith('#') && m_receiveBuffer.size() > 2) {
                int digitCount = m_receiveBuffer.at(1) - '0';
                if (digitCount >= 1 && digitCount <= 9 && m_receiveBuffer.size() > 2 + digitCount) {
                    bool ok;
                    int dataSize = m_receiveBuffer.mid(2, digitCount).toInt(&ok);
                    if (ok && m_receiveBuffer.size() >= 2 + digitCount + dataSize) {
                        QByteArray result = m_receiveBuffer.mid(2 + digitCount, dataSize);
                        m_receiveBuffer.remove(0, 2 + digitCount + dataSize + 1); // +1 for newline
                        return result;
                    }
                }
            }
            
            // Check if we have expected size
            if (expectedSize > 0 && m_receiveBuffer.size() >= expectedSize) {
                QByteArray result = m_receiveBuffer.left(expectedSize);
                m_receiveBuffer.remove(0, expectedSize);
                return result;
            }
        }
        elapsed += pollInterval;
    }
    
    return m_receiveBuffer;
}

void DeviceConnection::sendRawData(const QByteArray &data)
{
    if (!isConnected()) {
        return;
    }
    
    QMutexLocker locker(&m_mutex);
    m_socket->write(data);
    m_socket->flush();
}

void DeviceConnection::setState(ConnectionState state)
{
    if (m_state != state) {
        m_state = state;
        emit connectionStateChanged(state);
    }
}

void DeviceConnection::onConnected()
{
    setState(ConnectionState::Connected);
    m_reconnectAttempts = 0;
    m_reconnectTimer->stop();
    
    // Query device ID
    queryDeviceId();
    
    emit connected();
}

void DeviceConnection::onDisconnected()
{
    m_deviceId.clear();
    setState(ConnectionState::Disconnected);
    emit disconnected();
    
    if (m_autoReconnect && m_reconnectAttempts < MAX_RECONNECT_ATTEMPTS) {
        m_reconnectTimer->start();
    }
}

void DeviceConnection::onReadyRead()
{
    m_receiveBuffer += m_socket->readAll();
    processReceivedData();
}

void DeviceConnection::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    
    QString errorMsg = m_socket->errorString();
    setState(ConnectionState::Error);
    emit errorOccurred(errorMsg);
    
    if (m_autoReconnect && m_reconnectAttempts < MAX_RECONNECT_ATTEMPTS) {
        m_reconnectTimer->start();
    }
}

void DeviceConnection::onReconnectTimer()
{
    m_reconnectAttempts++;
    
    if (m_reconnectAttempts >= MAX_RECONNECT_ATTEMPTS) {
        m_reconnectTimer->stop();
        emit errorOccurred("Maximum reconnection attempts reached");
        return;
    }
    
    connectToDevice(m_host, m_port);
}

void DeviceConnection::queryDeviceId()
{
    QString id = sendQuery(SCPI::IDN, 2000);
    if (!id.isEmpty()) {
        m_deviceId = id;
        emit deviceIdChanged(id);
    }
}

void DeviceConnection::processReceivedData()
{
    // Check for async command responses
    if (m_waitingForResponse && !m_asyncQueue.isEmpty()) {
        int newlinePos = m_receiveBuffer.indexOf('\n');
        if (newlinePos >= 0) {
            QString response = QString::fromUtf8(m_receiveBuffer.left(newlinePos)).trimmed();
            m_receiveBuffer.remove(0, newlinePos + 1);
            
            AsyncCommand cmd = m_asyncQueue.dequeue();
            m_waitingForResponse = false;
            
            if (cmd.callback) {
                cmd.callback(response);
            }
            
            // Process next command in queue
            if (!m_asyncQueue.isEmpty()) {
                AsyncCommand next = m_asyncQueue.head();
                sendCommand(next.command);
                m_waitingForResponse = (next.callback != nullptr);
            }
        }
    }
    
    // Emit signal for raw data
    if (!m_receiveBuffer.isEmpty()) {
        emit dataReceived(m_receiveBuffer);
    }
}
