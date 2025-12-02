#ifndef SCOPESETTINGS_H
#define SCOPESETTINGS_H

#include <QObject>
#include <QSettings>
#include <QString>

/**
 * @brief Global oscilloscope settings and timebase management
 *
 * The ScopeSettings class manages all global oscilloscope configuration parameters
 * including timebase, acquisition modes, trigger settings, and run state. It provides
 * a centralized interface for storing and retrieving oscilloscope settings with
 * persistence support and Qt property system integration for UI binding.
 *
 * Key features:
 * - Timebase and horizontal offset control
 * - Multiple acquisition modes (Normal, Average, High Resolution, Peak Detect)
 * - Flexible trigger configuration (mode, slope, source, level)
 * - Run state management (Stopped, Running, Single, Waiting)
 * - Settings persistence using QSettings
 * - Qt property system integration for QML/JavaScript binding
 * - Signal emissions for all setting changes
 *
 * This class serves as the model for global oscilloscope state and provides
 * helper methods for converting between enum values and strings for UI display
 * and settings storage.
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
    /**
     * @brief Acquisition modes for data capture
     */
    enum class AcquisitionMode {
        Normal,         ///< Standard acquisition mode
        Average,        ///< Averaging mode for noise reduction
        HighResolution, ///< High resolution mode for better vertical resolution
        PeakDetect      ///< Peak detect mode for capturing glitches
    };
    Q_ENUM(AcquisitionMode)

    /**
     * @brief Trigger modes for acquisition control
     */
    enum class TriggerMode {
        Auto,   ///< Auto trigger when no signal present
        Normal, ///< Normal trigger requiring valid trigger event
        Single  ///< Single shot acquisition
    };
    Q_ENUM(TriggerMode)

    /**
     * @brief Trigger slope options
     */
    enum class TriggerSlope {
        Rising,  ///< Trigger on rising edge
        Falling, ///< Trigger on falling edge
        Either   ///< Trigger on either edge
    };
    Q_ENUM(TriggerSlope)

    /**
     * @brief Current run state of the oscilloscope
     */
    enum class RunState {
        Stopped, ///< Acquisition stopped
        Running, ///< Continuous acquisition running
        Single,  ///< Single acquisition in progress
        Waiting  ///< Waiting for trigger event
    };
    Q_ENUM(RunState)

    /**
     * @brief Constructs a ScopeSettings object with default values
     * @param parent Parent QObject for memory management
     */
    explicit ScopeSettings(QObject *parent = nullptr);

    /**
     * @brief Destroys the ScopeSettings object
     */
    ~ScopeSettings() override = default;

    // Timebase
    /**
     * @brief Gets the current timebase setting
     * @return Time per division in seconds
     */
    double timebase() const { return m_timebase; }

    /**
     * @brief Sets the timebase setting
     * @param timebase New time per division in seconds
     */
    void setTimebase(double timebase);

    /**
     * @brief Gets the horizontal offset
     * @return Horizontal offset in seconds
     */
    double horizontalOffset() const { return m_horizontalOffset; }

    /**
     * @brief Sets the horizontal offset
     * @param offset New horizontal offset in seconds
     */
    void setHorizontalOffset(double offset);

    // Acquisition
    /**
     * @brief Gets the current acquisition mode
     * @return Current acquisition mode
     */
    AcquisitionMode acquisitionMode() const { return m_acquisitionMode; }

    /**
     * @brief Sets the acquisition mode
     * @param mode New acquisition mode
     */
    void setAcquisitionMode(AcquisitionMode mode);

    /**
     * @brief Gets the number of averages for averaging mode
     * @return Number of waveforms to average
     */
    int averageCount() const { return m_averageCount; }

    /**
     * @brief Sets the number of averages for averaging mode
     * @param count Number of waveforms to average (typically 2-256)
     */
    void setAverageCount(int count);

    /**
     * @brief Gets the memory depth setting
     * @return Memory depth in samples
     */
    int memoryDepth() const { return m_memoryDepth; }

    /**
     * @brief Sets the memory depth
     * @param depth Memory depth in samples
     */
    void setMemoryDepth(int depth);

    // Trigger
    /**
     * @brief Gets the current trigger mode
     * @return Current trigger mode
     */
    TriggerMode triggerMode() const { return m_triggerMode; }

    /**
     * @brief Sets the trigger mode
     * @param mode New trigger mode
     */
    void setTriggerMode(TriggerMode mode);

    /**
     * @brief Gets the trigger slope
     * @return Current trigger slope
     */
    TriggerSlope triggerSlope() const { return m_triggerSlope; }

    /**
     * @brief Sets the trigger slope
     * @param slope New trigger slope
     */
    void setTriggerSlope(TriggerSlope slope);

    /**
     * @brief Gets the trigger source channel
     * @return Trigger source channel name (e.g., "CH1", "EXT")
     */
    QString triggerSource() const { return m_triggerSource; }

    /**
     * @brief Sets the trigger source channel
     * @param source Trigger source channel name
     */
    void setTriggerSource(const QString &source);

    /**
     * @brief Gets the trigger level
     * @return Trigger level in volts
     */
    double triggerLevel() const { return m_triggerLevel; }

    /**
     * @brief Sets the trigger level
     * @param level New trigger level in volts
     */
    void setTriggerLevel(double level);

    // Run state
    /**
     * @brief Gets the current run state
     * @return Current run state
     */
    RunState runState() const { return m_runState; }

    /**
     * @brief Sets the run state
     * @param state New run state
     */
    void setRunState(RunState state);

    // Persistence
    /**
     * @brief Saves all settings to persistent storage
     */
    void saveSettings();

    /**
     * @brief Loads all settings from persistent storage
     */
    void loadSettings();

    // Helper methods
    /**
     * @brief Converts acquisition mode to string representation
     * @param mode Acquisition mode to convert
     * @return String representation of the mode
     */
    static QString acquisitionModeToString(AcquisitionMode mode);

    /**
     * @brief Converts string to acquisition mode
     * @param str String representation of acquisition mode
     * @return Corresponding AcquisitionMode enum value
     */
    static AcquisitionMode stringToAcquisitionMode(const QString &str);

    /**
     * @brief Converts trigger mode to string representation
     * @param mode Trigger mode to convert
     * @return String representation of the mode
     */
    static QString triggerModeToString(TriggerMode mode);

    /**
     * @brief Converts string to trigger mode
     * @param str String representation of trigger mode
     * @return Corresponding TriggerMode enum value
     */
    static TriggerMode stringToTriggerMode(const QString &str);

    /**
     * @brief Converts trigger slope to string representation
     * @param slope Trigger slope to convert
     * @return String representation of the slope
     */
    static QString triggerSlopeToString(TriggerSlope slope);

    /**
     * @brief Converts string to trigger slope
     * @param str String representation of trigger slope
     * @return Corresponding TriggerSlope enum value
     */
    static TriggerSlope stringToTriggerSlope(const QString &str);

    // Standard timebase values
    /**
     * @brief Gets standard timebase values for oscilloscopes
     * @return Vector of standard timebase values in seconds per division
     */
    static QVector<double> standardTimebases();

    /**
     * @brief Gets standard voltage scale values for oscilloscopes
     * @return Vector of standard voltage scale values in volts per division
     */
    static QVector<double> standardVoltageScales();

signals:
    /**
     * @brief Emitted when timebase changes
     * @param timebase New timebase value in seconds per division
     */
    void timebaseChanged(double timebase);

    /**
     * @brief Emitted when horizontal offset changes
     * @param offset New horizontal offset value in seconds
     */
    void horizontalOffsetChanged(double offset);

    /**
     * @brief Emitted when acquisition mode changes
     * @param mode New acquisition mode
     */
    void acquisitionModeChanged(AcquisitionMode mode);

    /**
     * @brief Emitted when average count changes
     * @param count New average count value
     */
    void averageCountChanged(int count);

    /**
     * @brief Emitted when memory depth changes
     * @param depth New memory depth value
     */
    void memoryDepthChanged(int depth);

    /**
     * @brief Emitted when trigger mode changes
     * @param mode New trigger mode
     */
    void triggerModeChanged(TriggerMode mode);

    /**
     * @brief Emitted when trigger slope changes
     * @param slope New trigger slope
     */
    void triggerSlopeChanged(TriggerSlope slope);

    /**
     * @brief Emitted when trigger source changes
     * @param source New trigger source channel
     */
    void triggerSourceChanged(const QString &source);

    /**
     * @brief Emitted when trigger level changes
     * @param level New trigger level value
     */
    void triggerLevelChanged(double level);

    /**
     * @brief Emitted when run state changes
     * @param state New run state
     */
    void runStateChanged(RunState state);

private:
    /// @brief Current timebase setting in seconds per division
    double m_timebase = 1e-3;           // 1ms/div default

    /// @brief Horizontal offset in seconds
    double m_horizontalOffset = 0.0;

    /// @brief Current acquisition mode
    AcquisitionMode m_acquisitionMode = AcquisitionMode::Normal;

    /// @brief Number of waveforms to average in averaging mode
    int m_averageCount = 16;

    /// @brief Memory depth in samples
    int m_memoryDepth = 1000000;        // 1M points

    /// @brief Current trigger mode
    TriggerMode m_triggerMode = TriggerMode::Auto;

    /// @brief Current trigger slope
    TriggerSlope m_triggerSlope = TriggerSlope::Rising;

    /// @brief Trigger source channel name
    QString m_triggerSource = "CH1";

    /// @brief Trigger level in volts
    double m_triggerLevel = 0.0;

    /// @brief Current run state
    RunState m_runState = RunState::Stopped;

    /// @brief QSettings object for persistence
    QSettings m_settings;
};

#endif // SCOPESETTINGS_H
