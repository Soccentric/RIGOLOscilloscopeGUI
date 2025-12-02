#ifndef MEASUREMENTENGINE_H
#define MEASUREMENTENGINE_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QString>
#include <QPointF>
#include <functional>

class ScopeChannel;

/**
 * @brief Measurement types supported by the oscilloscope
 */
enum class MeasurementType {
    // Voltage measurements
    Vpp,
    Vmax,
    Vmin,
    Vavg,
    Vrms,
    Vamplitude,
    Vtop,
    Vbase,
    Vovershoot,
    Vpreshoot,
    
    // Time measurements
    Frequency,
    Period,
    RiseTime,
    FallTime,
    PositiveWidth,
    NegativeWidth,
    DutyCycle,
    PositiveDutyCycle,
    NegativeDutyCycle,
    
    // Phase measurements
    PhaseA_B,
    DelayA_B
};

/**
 * @brief Represents a single measurement configuration and result
 */
struct Measurement {
    MeasurementType type;
    QString channelName;
    QString secondaryChannel; // For phase/delay measurements
    double value = 0.0;
    QString unit;
    bool valid = false;
    
    QString toString() const;
    static QString typeToString(MeasurementType type);
    static QString typeToUnit(MeasurementType type);
};

/**
 * @brief Engine for calculating and managing oscilloscope measurements
 */
class MeasurementEngine : public QObject
{
    Q_OBJECT

public:
    explicit MeasurementEngine(QObject *parent = nullptr);
    ~MeasurementEngine() override = default;

    // Measurement management
    int addMeasurement(MeasurementType type, const QString &channel);
    int addPhaseMeasurement(MeasurementType type, const QString &channelA, const QString &channelB);
    void removeMeasurement(int id);
    void clearAllMeasurements();

    // Get measurements
    QVector<Measurement> activeMeasurements() const;
    Measurement getMeasurement(int id) const;
    
    // Update measurements with new data
    void updateMeasurements(const QMap<QString, ScopeChannel*> &channels);

    // Statistics
    void enableStatistics(bool enable);
    bool statisticsEnabled() const { return m_statisticsEnabled; }
    void resetStatistics();

    struct MeasurementStats {
        double current = 0.0;
        double minimum = 0.0;
        double maximum = 0.0;
        double average = 0.0;
        double stddev = 0.0;
        int count = 0;
    };
    MeasurementStats getStatistics(int id) const;

signals:
    void measurementsUpdated();
    void measurementAdded(int id);
    void measurementRemoved(int id);

private:
    double calculateMeasurement(MeasurementType type, const ScopeChannel *channel);
    double calculatePhaseMeasurement(MeasurementType type, const ScopeChannel *channelA, const ScopeChannel *channelB);
    void updateStatistics(int id, double value);

    int m_nextId = 0;
    QMap<int, Measurement> m_measurements;
    QMap<int, MeasurementStats> m_statistics;
    bool m_statisticsEnabled = false;
};

#endif // MEASUREMENTENGINE_H
