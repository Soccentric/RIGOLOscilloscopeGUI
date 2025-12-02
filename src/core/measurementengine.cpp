#include "measurementengine.h"
#include "scopechannel.h"
#include <cmath>
#include <algorithm>

QString Measurement::toString() const
{
    if (!valid) return "---";
    
    QString valueStr;
    double absValue = std::abs(value);
    
    if (absValue == 0) {
        valueStr = "0";
    } else if (absValue >= 1e6) {
        valueStr = QString::number(value / 1e6, 'f', 3) + " M";
    } else if (absValue >= 1e3) {
        valueStr = QString::number(value / 1e3, 'f', 3) + " k";
    } else if (absValue >= 1) {
        valueStr = QString::number(value, 'f', 3) + " ";
    } else if (absValue >= 1e-3) {
        valueStr = QString::number(value * 1e3, 'f', 3) + " m";
    } else if (absValue >= 1e-6) {
        valueStr = QString::number(value * 1e6, 'f', 3) + " µ";
    } else if (absValue >= 1e-9) {
        valueStr = QString::number(value * 1e9, 'f', 3) + " n";
    } else {
        valueStr = QString::number(value, 'e', 3) + " ";
    }
    
    return valueStr + unit;
}

QString Measurement::typeToString(MeasurementType type)
{
    switch (type) {
        case MeasurementType::Vpp: return "Vpp";
        case MeasurementType::Vmax: return "Vmax";
        case MeasurementType::Vmin: return "Vmin";
        case MeasurementType::Vavg: return "Vavg";
        case MeasurementType::Vrms: return "Vrms";
        case MeasurementType::Vamplitude: return "Vamp";
        case MeasurementType::Vtop: return "Vtop";
        case MeasurementType::Vbase: return "Vbase";
        case MeasurementType::Vovershoot: return "Overshoot";
        case MeasurementType::Vpreshoot: return "Preshoot";
        case MeasurementType::Frequency: return "Freq";
        case MeasurementType::Period: return "Period";
        case MeasurementType::RiseTime: return "Rise";
        case MeasurementType::FallTime: return "Fall";
        case MeasurementType::PositiveWidth: return "+Width";
        case MeasurementType::NegativeWidth: return "-Width";
        case MeasurementType::DutyCycle: return "Duty";
        case MeasurementType::PositiveDutyCycle: return "+Duty";
        case MeasurementType::NegativeDutyCycle: return "-Duty";
        case MeasurementType::PhaseA_B: return "Phase";
        case MeasurementType::DelayA_B: return "Delay";
    }
    return "Unknown";
}

QString Measurement::typeToUnit(MeasurementType type)
{
    switch (type) {
        case MeasurementType::Vpp:
        case MeasurementType::Vmax:
        case MeasurementType::Vmin:
        case MeasurementType::Vavg:
        case MeasurementType::Vrms:
        case MeasurementType::Vamplitude:
        case MeasurementType::Vtop:
        case MeasurementType::Vbase:
            return "V";
        case MeasurementType::Vovershoot:
        case MeasurementType::Vpreshoot:
        case MeasurementType::DutyCycle:
        case MeasurementType::PositiveDutyCycle:
        case MeasurementType::NegativeDutyCycle:
            return "%";
        case MeasurementType::Frequency:
            return "Hz";
        case MeasurementType::Period:
        case MeasurementType::RiseTime:
        case MeasurementType::FallTime:
        case MeasurementType::PositiveWidth:
        case MeasurementType::NegativeWidth:
        case MeasurementType::DelayA_B:
            return "s";
        case MeasurementType::PhaseA_B:
            return "°";
    }
    return "";
}

MeasurementEngine::MeasurementEngine(QObject *parent)
    : QObject(parent)
{
}

int MeasurementEngine::addMeasurement(MeasurementType type, const QString &channel)
{
    int id = m_nextId++;
    Measurement m;
    m.type = type;
    m.channelName = channel;
    m.unit = Measurement::typeToUnit(type);
    m_measurements[id] = m;
    m_statistics[id] = MeasurementStats();
    emit measurementAdded(id);
    return id;
}

int MeasurementEngine::addPhaseMeasurement(MeasurementType type, const QString &channelA, const QString &channelB)
{
    int id = m_nextId++;
    Measurement m;
    m.type = type;
    m.channelName = channelA;
    m.secondaryChannel = channelB;
    m.unit = Measurement::typeToUnit(type);
    m_measurements[id] = m;
    m_statistics[id] = MeasurementStats();
    emit measurementAdded(id);
    return id;
}

void MeasurementEngine::removeMeasurement(int id)
{
    if (m_measurements.contains(id)) {
        m_measurements.remove(id);
        m_statistics.remove(id);
        emit measurementRemoved(id);
    }
}

void MeasurementEngine::clearAllMeasurements()
{
    m_measurements.clear();
    m_statistics.clear();
    emit measurementsUpdated();
}

QVector<Measurement> MeasurementEngine::activeMeasurements() const
{
    return m_measurements.values().toVector();
}

Measurement MeasurementEngine::getMeasurement(int id) const
{
    return m_measurements.value(id);
}

void MeasurementEngine::updateMeasurements(const QMap<QString, ScopeChannel*> &channels)
{
    for (auto it = m_measurements.begin(); it != m_measurements.end(); ++it) {
        Measurement &m = it.value();
        
        if (m.secondaryChannel.isEmpty()) {
            // Single channel measurement
            ScopeChannel *ch = channels.value(m.channelName);
            if (ch && ch->isEnabled() && !ch->data().isEmpty()) {
                m.value = calculateMeasurement(m.type, ch);
                m.valid = true;
            } else {
                m.valid = false;
            }
        } else {
            // Two channel measurement (phase/delay)
            ScopeChannel *chA = channels.value(m.channelName);
            ScopeChannel *chB = channels.value(m.secondaryChannel);
            if (chA && chB && chA->isEnabled() && chB->isEnabled() &&
                !chA->data().isEmpty() && !chB->data().isEmpty()) {
                m.value = calculatePhaseMeasurement(m.type, chA, chB);
                m.valid = true;
            } else {
                m.valid = false;
            }
        }
        
        if (m.valid && m_statisticsEnabled) {
            updateStatistics(it.key(), m.value);
        }
    }
    
    emit measurementsUpdated();
}

double MeasurementEngine::calculateMeasurement(MeasurementType type, const ScopeChannel *channel)
{
    switch (type) {
        case MeasurementType::Vpp:
            return channel->measureVpp();
        case MeasurementType::Vmax:
            return channel->measureVmax();
        case MeasurementType::Vmin:
            return channel->measureVmin();
        case MeasurementType::Vavg:
            return channel->measureVavg();
        case MeasurementType::Vrms:
            return channel->measureVrms();
        case MeasurementType::Frequency:
            return channel->measureFrequency();
        case MeasurementType::Period:
            return channel->measurePeriod();
        case MeasurementType::RiseTime:
            return channel->measureRiseTime();
        case MeasurementType::FallTime:
            return channel->measureFallTime();
        case MeasurementType::DutyCycle:
            return channel->measureDutyCycle();
        default:
            return 0.0;
    }
}

double MeasurementEngine::calculatePhaseMeasurement(MeasurementType type, 
                                                     const ScopeChannel *channelA, 
                                                     const ScopeChannel *channelB)
{
    const auto &dataA = channelA->data();
    const auto &dataB = channelB->data();
    
    if (dataA.isEmpty() || dataB.isEmpty()) return 0.0;
    
    double periodA = channelA->measurePeriod();
    if (periodA <= 0) return 0.0;
    
    // Find zero crossings for both channels
    double avgA = channelA->measureVavg() / channelA->probeFactor();
    double avgB = channelB->measureVavg() / channelB->probeFactor();
    
    double crossingA = -1, crossingB = -1;
    
    for (int i = 1; i < dataA.size() && crossingA < 0; ++i) {
        if (dataA[i-1].y() < avgA && dataA[i].y() >= avgA) {
            crossingA = dataA[i-1].x() + (avgA - dataA[i-1].y()) * 
                        (dataA[i].x() - dataA[i-1].x()) / (dataA[i].y() - dataA[i-1].y());
        }
    }
    
    for (int i = 1; i < dataB.size() && crossingB < 0; ++i) {
        if (dataB[i-1].y() < avgB && dataB[i].y() >= avgB) {
            crossingB = dataB[i-1].x() + (avgB - dataB[i-1].y()) * 
                        (dataB[i].x() - dataB[i-1].x()) / (dataB[i].y() - dataB[i-1].y());
        }
    }
    
    if (crossingA < 0 || crossingB < 0) return 0.0;
    
    double delay = crossingB - crossingA;
    
    if (type == MeasurementType::DelayA_B) {
        return delay;
    } else { // PhaseA_B
        return (delay / periodA) * 360.0;
    }
}

void MeasurementEngine::enableStatistics(bool enable)
{
    m_statisticsEnabled = enable;
    if (!enable) {
        resetStatistics();
    }
}

void MeasurementEngine::resetStatistics()
{
    for (auto it = m_statistics.begin(); it != m_statistics.end(); ++it) {
        it.value() = MeasurementStats();
    }
}

void MeasurementEngine::updateStatistics(int id, double value)
{
    if (!m_statistics.contains(id)) return;
    
    MeasurementStats &stats = m_statistics[id];
    stats.count++;
    
    if (stats.count == 1) {
        stats.current = value;
        stats.minimum = value;
        stats.maximum = value;
        stats.average = value;
        stats.stddev = 0.0;
    } else {
        stats.current = value;
        stats.minimum = std::min(stats.minimum, value);
        stats.maximum = std::max(stats.maximum, value);
        
        // Online algorithm for mean and variance
        double delta = value - stats.average;
        stats.average += delta / stats.count;
        double delta2 = value - stats.average;
        double m2 = stats.stddev * stats.stddev * (stats.count - 1);
        m2 += delta * delta2;
        stats.stddev = std::sqrt(m2 / stats.count);
    }
}

MeasurementEngine::MeasurementStats MeasurementEngine::getStatistics(int id) const
{
    return m_statistics.value(id);
}
