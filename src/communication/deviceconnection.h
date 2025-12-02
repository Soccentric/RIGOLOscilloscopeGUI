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
 *
 * The DeviceConnection class handles all network communication with RIGOL
 * oscilloscopes using TCP/IP protocol. It provides both synchronous and
 * asynchronous command interfaces, supports binary data transfer for
 * waveform acquisition, and includes automatic reconnection capabilities.
 *
 * Key features:
 * - TCP/IP socket communication with configurable host/port
 * - Synchronous and asynchronous SCPI command execution
 * - Binary waveform data transfer
 * - Automatic reconnection on connection loss
 * - Thread-safe operation with mutex protection
 * - Connection state monitoring and error handling
 * - Device identification and validation
 *
 * The class uses Qt's signal-slot mechanism for asynchronous notifications
 * and provides both blocking and non-blocking API variants for different
 * use cases in the oscilloscope control application.
 */
class DeviceConnection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectionStateChanged)
    Q_PROPERTY(QString deviceId READ deviceId NOTIFY deviceIdChanged)

public:
public:
    /**
     * @brief Connection states for the device connection
     */
    enum class ConnectionState {
        Disconnected, ///< Not connected to device
        Connecting,   ///< Attempting to connect
        Connected,    ///< Successfully connected
        Error         ///< Connection error occurred
    };
    Q_ENUM(ConnectionState)

    /**
     * @brief Constructs a device connection manager
     * @param parent Parent QObject for memory management
     */
    explicit DeviceConnection(QObject *parent = nullptr);

    /**
     * @brief Destroys the device connection and cleans up resources
     */
    ~DeviceConnection() override;

    // Connection management
    /**
     * @brief Initiates connection to the oscilloscope device
     * @param host IP address or hostname of the device
     * @param port TCP port number (default 5555 for RIGOL devices)
     */
    void connectToDevice(const QString &host, quint16 port);

    /**
     * @brief Disconnects from the currently connected device
     */
    void disconnectFromDevice();

    /**
     * @brief Checks if currently connected to a device
     * @return True if connected, false otherwise
     */
    bool isConnected() const { return m_state == ConnectionState::Connected; }

    /**
     * @brief Gets the current connection state
     * @return Current connection state
     */
    ConnectionState state() const { return m_state; }

    /**
     * @brief Gets the device identification string
     * @return Device ID string (e.g., manufacturer, model, serial)
     */
    QString deviceId() const { return m_deviceId; }

    // Command sending
    /**
     * @brief Sends a SCPI command without waiting for response
     * @param command SCPI command string to send
     */
    void sendCommand(const QString &command);

    /**
     * @brief Sends a SCPI query and waits for response
     * @param query SCPI query string to send
     * @param timeoutMs Timeout in milliseconds for response
     * @return Response string from device
     */
    QString sendQuery(const QString &query, int timeoutMs = 5000);

    /**
     * @brief Sends a command asynchronously with optional callback
     * @param command SCPI command to send
     * @param callback Function to call when response is received (optional)
     */
    void sendCommandAsync(const QString &command, std::function<void(const QString&)> callback = nullptr);
    
    // Raw data operations
    /**
     * @brief Reads binary data from the device
     * @param expectedSize Expected number of bytes (-1 for unknown size)
     * @param timeoutMs Timeout in milliseconds
     * @return Binary data received from device
     */
    QByteArray readBinaryData(int expectedSize = -1, int timeoutMs = 10000);

    /**
     * @brief Sends raw binary data to the device
     * @param data Binary data to send
     */
    void sendRawData(const QByteArray &data);

    // Connection settings
    /**
     * @brief Enables or disables automatic reconnection
     * @param enable True to enable auto-reconnect, false to disable
     */
    void setAutoReconnect(bool enable) { m_autoReconnect = enable; }

    /**
     * @brief Checks if automatic reconnection is enabled
     * @return True if auto-reconnect is enabled
     */
    bool autoReconnect() const { return m_autoReconnect; }

    /**
     * @brief Sets the connection timeout
     * @param ms Timeout in milliseconds
     */
    void setConnectionTimeout(int ms) { m_connectionTimeout = ms; }

    // Host info
    /**
     * @brief Gets the connected host address
     * @return Host IP address or hostname
     */
    QString host() const { return m_host; }

    /**
     * @brief Gets the connected port number
     * @return TCP port number
     */
    quint16 port() const { return m_port; }

signals:
    /**
     * @brief Emitted when connection state changes
     * @param state New connection state
     */
    void connectionStateChanged(ConnectionState state);

    /**
     * @brief Emitted when successfully connected to device
     */
    void connected();

    /**
     * @brief Emitted when disconnected from device
     */
    void disconnected();

    /**
     * @brief Emitted when a connection error occurs
     * @param errorMessage Description of the error
     */
    void errorOccurred(const QString &errorMessage);

    /**
     * @brief Emitted when data is received from device
     * @param data Received data bytes
     */
    void dataReceived(const QByteArray &data);

    /**
     * @brief Emitted when device ID is received or changed
     * @param deviceId New device identification string
     */
    void deviceIdChanged(const QString &deviceId);

private slots:
    /**
     * @brief Handles successful socket connection
     */
    void onConnected();

    /**
     * @brief Handles socket disconnection
     */
    void onDisconnected();

    /**
     * @brief Handles incoming data from socket
     */
    void onReadyRead();

    /**
     * @brief Handles socket errors
     * @param socketError The socket error that occurred
     */
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

    /**
     * @brief Handles reconnection timer timeout
     */
    void onReconnectTimer();

private:
    /**
     * @brief Sets the connection state and emits signals
     * @param state New connection state
     */
    void setState(ConnectionState state);

    /**
     * @brief Queries the device for identification information
     */
    void queryDeviceId();

    /**
     * @brief Processes received data from the device
     */
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
