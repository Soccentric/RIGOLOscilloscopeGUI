#ifndef WAVEFORMPROVIDER_H
#define WAVEFORMPROVIDER_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QPointF>
#include <QMap>

class DeviceConnection;

/**
 * @brief Provides waveform data acquisition and management
 */
class WaveformProvider : public QObject
{
    Q_OBJECT

public:
    struct WaveformPreamble {
        int format = 0;        // 0=BYTE, 1=WORD, 2=ASCii
        int type = 0;          // 0=NORMal, 1=MAXimum, 2=RAW
        int points = 0;        // Number of points
        int count = 1;         // Average count
        double xIncrement = 1e-6;
        double xOrigin = 0;
        double xReference = 0;
        double yIncrement = 0.01;
        double yOrigin = 0;
        double yReference = 128;
    };

    explicit WaveformProvider(DeviceConnection *connection, QObject *parent = nullptr);
    ~WaveformProvider() override = default;

    // Acquisition control
    void startContinuousAcquisition(int intervalMs = 100);
    void stopContinuousAcquisition();
    bool isContinuousAcquisitionActive() const { return m_timer->isActive(); }

    // Single acquisition
    void acquireWaveform(const QString &channel);
    void acquireAllChannels(const QStringList &channels);

    // Mode settings
    enum class WaveformMode { Normal, Maximum, Raw };
    Q_ENUM(WaveformMode)

    void setWaveformMode(WaveformMode mode);
    WaveformMode waveformMode() const { return m_mode; }

    void setStartPoint(int start) { m_startPoint = start; }
    void setStopPoint(int stop) { m_stopPoint = stop; }

    // Get last preamble
    WaveformPreamble preamble(const QString &channel) const;

signals:
    void waveformDataReady(const QString &channel, const QVector<QPointF> &data);
    void preambleUpdated(const QString &channel, const WaveformPreamble &preamble);
    void acquisitionError(const QString &errorMessage);
    void acquisitionStarted();
    void acquisitionStopped();

private slots:
    void onAcquisitionTimer();

private:
    WaveformPreamble parsePreamble(const QString &response);
    QVector<QPointF> parseWaveformData(const QByteArray &data, const WaveformPreamble &preamble);
    void requestPreamble(const QString &channel);
    void requestWaveformData(const QString &channel);

    DeviceConnection *m_connection;
    QTimer *m_timer;
    QStringList m_activeChannels;
    int m_currentChannelIndex = 0;
    WaveformMode m_mode = WaveformMode::Normal;
    int m_startPoint = 1;
    int m_stopPoint = 1200;
    QMap<QString, WaveformPreamble> m_preambles;
};

#endif // WAVEFORMPROVIDER_H
