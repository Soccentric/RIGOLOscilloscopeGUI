#ifndef SCOPESETTINGS_H
#define SCOPESETTINGS_H

#include <QObject>
#include <QSettings>
#include <QString>

/**
 * @brief Global oscilloscope settings and timebase management
 */
class ScopeSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double timebase READ timebase WRITE setTimebase NOTIFY timebaseChanged)
    Q_PROPERTY(double horizontalOffset READ horizontalOffset WRITE setHorizontalOffset NOTIFY horizontalOffsetChanged)
    Q_PROPERTY(AcquisitionMode acquisitionMode READ acquisitionMode WRITE setAcquisitionMode NOTIFY acquisitionModeChanged)
    Q_PROPERTY(TriggerMode triggerMode READ triggerMode WRITE setTriggerMode NOTIFY triggerModeChanged)
    Q_PROPERTY(TriggerSlope triggerSlope READ triggerSlope WRITE setTriggerSlope NOTIFY triggerSlopeChanged)
    Q_PROPERTY(QString triggerSource READ triggerSource WRITE setTriggerSource NOTIFY triggerSourceChanged)
    Q_PROPERTY(double triggerLevel READ triggerLevel WRITE setTriggerLevel NOTIFY triggerLevelChanged)
    Q_PROPERTY(int memoryDepth READ memoryDepth WRITE setMemoryDepth NOTIFY memoryDepthChanged)

public:
    enum class AcquisitionMode {
        Normal,
        Average,
        HighResolution,
        PeakDetect
    };
    Q_ENUM(AcquisitionMode)

    enum class TriggerMode {
        Auto,
        Normal,
        Single
    };
    Q_ENUM(TriggerMode)

    enum class TriggerSlope {
        Rising,
        Falling,
        Either
    };
    Q_ENUM(TriggerSlope)

    enum class RunState {
        Stopped,
        Running,
        Single,
        Waiting
    };
    Q_ENUM(RunState)

    explicit ScopeSettings(QObject *parent = nullptr);
    ~ScopeSettings() override = default;

    // Timebase
    double timebase() const { return m_timebase; }
    void setTimebase(double timebase);
    double horizontalOffset() const { return m_horizontalOffset; }
    void setHorizontalOffset(double offset);

    // Acquisition
    AcquisitionMode acquisitionMode() const { return m_acquisitionMode; }
    void setAcquisitionMode(AcquisitionMode mode);
    int averageCount() const { return m_averageCount; }
    void setAverageCount(int count);
    int memoryDepth() const { return m_memoryDepth; }
    void setMemoryDepth(int depth);

    // Trigger
    TriggerMode triggerMode() const { return m_triggerMode; }
    void setTriggerMode(TriggerMode mode);
    TriggerSlope triggerSlope() const { return m_triggerSlope; }
    void setTriggerSlope(TriggerSlope slope);
    QString triggerSource() const { return m_triggerSource; }
    void setTriggerSource(const QString &source);
    double triggerLevel() const { return m_triggerLevel; }
    void setTriggerLevel(double level);

    // Run state
    RunState runState() const { return m_runState; }
    void setRunState(RunState state);

    // Persistence
    void saveSettings();
    void loadSettings();

    // Helper methods
    static QString acquisitionModeToString(AcquisitionMode mode);
    static AcquisitionMode stringToAcquisitionMode(const QString &str);
    static QString triggerModeToString(TriggerMode mode);
    static TriggerMode stringToTriggerMode(const QString &str);
    static QString triggerSlopeToString(TriggerSlope slope);
    static TriggerSlope stringToTriggerSlope(const QString &str);

    // Standard timebase values
    static QVector<double> standardTimebases();
    static QVector<double> standardVoltageScales();

signals:
    void timebaseChanged(double timebase);
    void horizontalOffsetChanged(double offset);
    void acquisitionModeChanged(AcquisitionMode mode);
    void averageCountChanged(int count);
    void memoryDepthChanged(int depth);
    void triggerModeChanged(TriggerMode mode);
    void triggerSlopeChanged(TriggerSlope slope);
    void triggerSourceChanged(const QString &source);
    void triggerLevelChanged(double level);
    void runStateChanged(RunState state);

private:
    double m_timebase = 1e-3;           // 1ms/div default
    double m_horizontalOffset = 0.0;
    AcquisitionMode m_acquisitionMode = AcquisitionMode::Normal;
    int m_averageCount = 16;
    int m_memoryDepth = 1000000;        // 1M points
    TriggerMode m_triggerMode = TriggerMode::Auto;
    TriggerSlope m_triggerSlope = TriggerSlope::Rising;
    QString m_triggerSource = "CH1";
    double m_triggerLevel = 0.0;
    RunState m_runState = RunState::Stopped;
    QSettings m_settings;
};

#endif // SCOPESETTINGS_H
