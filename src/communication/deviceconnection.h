#ifndef DEVICECONNECTION_H
#define DEVICECONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QMutex>
#include <QQueue>
#include <functional>

/**
 * @brief Manages TCP/IP connection to the oscilloscope
 */
class DeviceConnection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectionStateChanged)
    Q_PROPERTY(QString deviceId READ deviceId NOTIFY deviceIdChanged)

public:
    enum class ConnectionState {
        Disconnected,
        Connecting,
        Connected,
        Error
    };
    Q_ENUM(ConnectionState)

    explicit DeviceConnection(QObject *parent = nullptr);
    ~DeviceConnection() override;

    // Connection management
    void connectToDevice(const QString &host, quint16 port);
    void disconnectFromDevice();
    bool isConnected() const { return m_state == ConnectionState::Connected; }
    ConnectionState state() const { return m_state; }
    QString deviceId() const { return m_deviceId; }

    // Command sending
    void sendCommand(const QString &command);
    QString sendQuery(const QString &query, int timeoutMs = 5000);
    void sendCommandAsync(const QString &command, std::function<void(const QString&)> callback = nullptr);
    
    // Raw data operations
    QByteArray readBinaryData(int expectedSize = -1, int timeoutMs = 10000);
    void sendRawData(const QByteArray &data);

    // Connection settings
    void setAutoReconnect(bool enable) { m_autoReconnect = enable; }
    bool autoReconnect() const { return m_autoReconnect; }
    void setConnectionTimeout(int ms) { m_connectionTimeout = ms; }

    // Host info
    QString host() const { return m_host; }
    quint16 port() const { return m_port; }

signals:
    void connectionStateChanged(ConnectionState state);
    void connected();
    void disconnected();
    void errorOccurred(const QString &errorMessage);
    void dataReceived(const QByteArray &data);
    void deviceIdChanged(const QString &deviceId);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void onReconnectTimer();

private:
    void setState(ConnectionState state);
    void queryDeviceId();
    void processReceivedData();

    QTcpSocket *m_socket;
    QString m_host;
    quint16 m_port = 5555;
    ConnectionState m_state = ConnectionState::Disconnected;
    QString m_deviceId;
    QByteArray m_receiveBuffer;
    
    // Reconnection
    bool m_autoReconnect = false;
    QTimer *m_reconnectTimer;
    int m_connectionTimeout = 10000;
    int m_reconnectAttempts = 0;
    static constexpr int MAX_RECONNECT_ATTEMPTS = 5;
    
    // Thread safety
    QMutex m_mutex;
    
    // Async command queue
    struct AsyncCommand {
        QString command;
        std::function<void(const QString&)> callback;
    };
    QQueue<AsyncCommand> m_asyncQueue;
    bool m_waitingForResponse = false;
};

#endif // DEVICECONNECTION_H
