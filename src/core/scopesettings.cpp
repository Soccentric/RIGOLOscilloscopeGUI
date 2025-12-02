#include "scopesettings.h"

ScopeSettings::ScopeSettings(QObject *parent)
    : QObject(parent)
    , m_settings("RigolScope", "RIGOLOscilloscopeGUI")
{
    loadSettings();
}

void ScopeSettings::setTimebase(double timebase)
{
    if (timebase > 0 && !qFuzzyCompare(m_timebase, timebase)) {
        m_timebase = timebase;
        emit timebaseChanged(timebase);
    }
}

void ScopeSettings::setHorizontalOffset(double offset)
{
    if (!qFuzzyCompare(m_horizontalOffset, offset)) {
        m_horizontalOffset = offset;
        emit horizontalOffsetChanged(offset);
    }
}

void ScopeSettings::setAcquisitionMode(AcquisitionMode mode)
{
    if (m_acquisitionMode != mode) {
        m_acquisitionMode = mode;
        emit acquisitionModeChanged(mode);
    }
}

void ScopeSettings::setAverageCount(int count)
{
    if (count > 0 && m_averageCount != count) {
        m_averageCount = count;
        emit averageCountChanged(count);
    }
}

void ScopeSettings::setMemoryDepth(int depth)
{
    if (depth > 0 && m_memoryDepth != depth) {
        m_memoryDepth = depth;
        emit memoryDepthChanged(depth);
    }
}

void ScopeSettings::setTriggerMode(TriggerMode mode)
{
    if (m_triggerMode != mode) {
        m_triggerMode = mode;
        emit triggerModeChanged(mode);
    }
}

void ScopeSettings::setTriggerSlope(TriggerSlope slope)
{
    if (m_triggerSlope != slope) {
        m_triggerSlope = slope;
        emit triggerSlopeChanged(slope);
    }
}

void ScopeSettings::setTriggerSource(const QString &source)
{
    if (m_triggerSource != source) {
        m_triggerSource = source;
        emit triggerSourceChanged(source);
    }
}

void ScopeSettings::setTriggerLevel(double level)
{
    if (!qFuzzyCompare(m_triggerLevel, level)) {
        m_triggerLevel = level;
        emit triggerLevelChanged(level);
    }
}

void ScopeSettings::setRunState(RunState state)
{
    if (m_runState != state) {
        m_runState = state;
        emit runStateChanged(state);
    }
}

void ScopeSettings::saveSettings()
{
    m_settings.beginGroup("Timebase");
    m_settings.setValue("value", m_timebase);
    m_settings.setValue("offset", m_horizontalOffset);
    m_settings.endGroup();

    m_settings.beginGroup("Acquisition");
    m_settings.setValue("mode", static_cast<int>(m_acquisitionMode));
    m_settings.setValue("averageCount", m_averageCount);
    m_settings.setValue("memoryDepth", m_memoryDepth);
    m_settings.endGroup();

    m_settings.beginGroup("Trigger");
    m_settings.setValue("mode", static_cast<int>(m_triggerMode));
    m_settings.setValue("slope", static_cast<int>(m_triggerSlope));
    m_settings.setValue("source", m_triggerSource);
    m_settings.setValue("level", m_triggerLevel);
    m_settings.endGroup();
}

void ScopeSettings::loadSettings()
{
    m_settings.beginGroup("Timebase");
    m_timebase = m_settings.value("value", 1e-3).toDouble();
    m_horizontalOffset = m_settings.value("offset", 0.0).toDouble();
    m_settings.endGroup();

    m_settings.beginGroup("Acquisition");
    m_acquisitionMode = static_cast<AcquisitionMode>(m_settings.value("mode", 0).toInt());
    m_averageCount = m_settings.value("averageCount", 16).toInt();
    m_memoryDepth = m_settings.value("memoryDepth", 1000000).toInt();
    m_settings.endGroup();

    m_settings.beginGroup("Trigger");
    m_triggerMode = static_cast<TriggerMode>(m_settings.value("mode", 0).toInt());
    m_triggerSlope = static_cast<TriggerSlope>(m_settings.value("slope", 0).toInt());
    m_triggerSource = m_settings.value("source", "CH1").toString();
    m_triggerLevel = m_settings.value("level", 0.0).toDouble();
    m_settings.endGroup();
}

QString ScopeSettings::acquisitionModeToString(AcquisitionMode mode)
{
    switch (mode) {
        case AcquisitionMode::Normal: return "Normal";
        case AcquisitionMode::Average: return "Average";
        case AcquisitionMode::HighResolution: return "High Resolution";
        case AcquisitionMode::PeakDetect: return "Peak Detect";
    }
    return "Normal";
}

ScopeSettings::AcquisitionMode ScopeSettings::stringToAcquisitionMode(const QString &str)
{
    if (str == "Average") return AcquisitionMode::Average;
    if (str == "High Resolution") return AcquisitionMode::HighResolution;
    if (str == "Peak Detect") return AcquisitionMode::PeakDetect;
    return AcquisitionMode::Normal;
}

QString ScopeSettings::triggerModeToString(TriggerMode mode)
{
    switch (mode) {
        case TriggerMode::Auto: return "Auto";
        case TriggerMode::Normal: return "Normal";
        case TriggerMode::Single: return "Single";
    }
    return "Auto";
}

ScopeSettings::TriggerMode ScopeSettings::stringToTriggerMode(const QString &str)
{
    if (str == "Normal") return TriggerMode::Normal;
    if (str == "Single") return TriggerMode::Single;
    return TriggerMode::Auto;
}

QString ScopeSettings::triggerSlopeToString(TriggerSlope slope)
{
    switch (slope) {
        case TriggerSlope::Rising: return "Rising";
        case TriggerSlope::Falling: return "Falling";
        case TriggerSlope::Either: return "Either";
    }
    return "Rising";
}

ScopeSettings::TriggerSlope ScopeSettings::stringToTriggerSlope(const QString &str)
{
    if (str == "Falling") return TriggerSlope::Falling;
    if (str == "Either") return TriggerSlope::Either;
    return TriggerSlope::Rising;
}

QVector<double> ScopeSettings::standardTimebases()
{
    return {
        2e-9, 5e-9, 10e-9, 20e-9, 50e-9, 100e-9, 200e-9, 500e-9,
        1e-6, 2e-6, 5e-6, 10e-6, 20e-6, 50e-6, 100e-6, 200e-6, 500e-6,
        1e-3, 2e-3, 5e-3, 10e-3, 20e-3, 50e-3, 100e-3, 200e-3, 500e-3,
        1.0, 2.0, 5.0, 10.0, 20.0, 50.0
    };
}

QVector<double> ScopeSettings::standardVoltageScales()
{
    return {
        1e-3, 2e-3, 5e-3, 10e-3, 20e-3, 50e-3, 100e-3, 200e-3, 500e-3,
        1.0, 2.0, 5.0, 10.0
    };
}
