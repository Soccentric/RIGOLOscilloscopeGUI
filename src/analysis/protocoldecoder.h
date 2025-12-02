#ifndef PROTOCOLDECODER_H
#define PROTOCOLDECODER_H

#include <QObject>
#include <QVector>
#include <QPointF>
#include <QVariant>

/**
 * @brief Protocol decoder for serial communication protocols
 */
class ProtocolDecoder : public QObject
{
    Q_OBJECT

public:
    enum class Protocol {
        None,
        UART,
        SPI,
        I2C,
        CAN,
        LIN
    };
    Q_ENUM(Protocol)

    struct DecodedFrame {
        double startTime;
        double endTime;
        QString type;       // START, STOP, DATA, ACK, NAK, ADDRESS, etc.
        QByteArray data;
        QString description;
        bool error = false;
        QString errorMessage;
    };

    explicit ProtocolDecoder(QObject *parent = nullptr);
    ~ProtocolDecoder() override = default;

    // Protocol selection
    void setProtocol(Protocol protocol);
    Protocol protocol() const { return m_protocol; }

    // UART configuration
    void setUARTBaudRate(int baudRate);
    void setUARTDataBits(int bits);
    void setUARTStopBits(int bits);
    void setUARTParity(int parity);  // 0=none, 1=odd, 2=even
    void setUARTPolarity(bool inverted);

    // SPI configuration
    void setSPIClockPolarity(int cpol);
    void setSPIClockPhase(int cpha);
    void setSPIBitOrder(bool msbFirst);
    void setSPIWordSize(int bits);

    // I2C configuration
    void setI2CAddressMode(int bits);  // 7 or 10

    // Thresholds
    void setHighThreshold(double threshold);
    void setLowThreshold(double threshold);

    // Decode
    QVector<DecodedFrame> decode(const QVector<QPointF> &dataChannel,
                                  const QVector<QPointF> &clockChannel = QVector<QPointF>(),
                                  const QVector<QPointF> &csChannel = QVector<QPointF>());

    // For I2C
    QVector<DecodedFrame> decodeI2C(const QVector<QPointF> &sda,
                                     const QVector<QPointF> &scl);

    // For UART
    QVector<DecodedFrame> decodeUART(const QVector<QPointF> &data);

    // For SPI
    QVector<DecodedFrame> decodeSPI(const QVector<QPointF> &mosi,
                                     const QVector<QPointF> &miso,
                                     const QVector<QPointF> &sck,
                                     const QVector<QPointF> &cs);

signals:
    void decodingComplete(const QVector<DecodedFrame> &frames);
    void protocolChanged(Protocol protocol);

private:
    // Edge detection
    struct Edge {
        double time;
        bool rising;
    };
    QVector<Edge> findEdges(const QVector<QPointF> &data, double threshold);
    
    // Level sampling
    bool sampleLevel(const QVector<QPointF> &data, double time, double threshold);
    
    // Bit period estimation
    double estimateBitPeriod(const QVector<Edge> &edges);

    Protocol m_protocol = Protocol::None;
    
    // UART settings
    int m_uartBaudRate = 9600;
    int m_uartDataBits = 8;
    int m_uartStopBits = 1;
    int m_uartParity = 0;
    bool m_uartInverted = false;

    // SPI settings
    int m_spiCPOL = 0;
    int m_spiCPHA = 0;
    bool m_spiMSBFirst = true;
    int m_spiWordSize = 8;

    // I2C settings
    int m_i2cAddressMode = 7;

    // Thresholds
    double m_highThreshold = 2.5;
    double m_lowThreshold = 0.8;
};

#endif // PROTOCOLDECODER_H
