#include "scopechannel.h"
#include <cmath>
#include <algorithm>
#include <numeric>

ScopeChannel::ScopeChannel(const QString &name, ChannelType type, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_type(type)
{
    // Set default colors based on channel name
    if (name == "CH1") m_color = QColor(255, 255, 0);      // Yellow
    else if (name == "CH2") m_color = QColor(0, 255, 255); // Cyan
    else if (name == "CH3") m_color = QColor(255, 0, 255); // Magenta
    else if (name == "CH4") m_color = QColor(0, 255, 0);   // Green
    else if (name.startsWith("D")) m_color = QColor(255, 128, 0); // Orange for digital
    else if (name.startsWith("MATH")) m_color = QColor(255, 0, 0); // Red for math
    else m_color = QColor(255, 255, 255); // White default
}

void ScopeChannel::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        emit enabledChanged(enabled);
    }
}

void ScopeChannel::setScale(double scale)
{
    if (scale > 0 && !qFuzzyCompare(m_scale, scale)) {
        m_scale = scale;
        emit scaleChanged(scale);
    }
}

void ScopeChannel::setOffset(double offset)
{
    if (!qFuzzyCompare(m_offset, offset)) {
        m_offset = offset;
        emit offsetChanged(offset);
    }
}

void ScopeChannel::setColor(const QColor &color)
{
    if (m_color != color) {
        m_color = color;
        emit colorChanged(color);
    }
}

void ScopeChannel::setCoupling(Coupling coupling)
{
    if (m_coupling != coupling) {
        m_coupling = coupling;
        emit couplingChanged(coupling);
    }
}

void ScopeChannel::setProbe(Probe probe)
{
    if (m_probe != probe) {
        m_probe = probe;
        emit probeChanged(probe);
    }
}

void ScopeChannel::setData(const QVector<QPointF> &data)
{
    m_data = data;
    emit dataChanged();
}

void ScopeChannel::clearData()
{
    m_data.clear();
    emit dataChanged();
}

double ScopeChannel::probeFactor() const
{
    switch (m_probe) {
        case Probe::X1: return 1.0;
        case Probe::X10: return 10.0;
        case Probe::X100: return 100.0;
        case Probe::X1000: return 1000.0;
    }
    return 1.0;
}

double ScopeChannel::measureVpp() const
{
    if (m_data.isEmpty()) return 0.0;
    auto [minIt, maxIt] = std::minmax_element(m_data.begin(), m_data.end(),
        [](const QPointF &a, const QPointF &b) { return a.y() < b.y(); });
    return (maxIt->y() - minIt->y()) * probeFactor();
}

double ScopeChannel::measureVmax() const
{
    if (m_data.isEmpty()) return 0.0;
    auto maxIt = std::max_element(m_data.begin(), m_data.end(),
        [](const QPointF &a, const QPointF &b) { return a.y() < b.y(); });
    return maxIt->y() * probeFactor();
}

double ScopeChannel::measureVmin() const
{
    if (m_data.isEmpty()) return 0.0;
    auto minIt = std::min_element(m_data.begin(), m_data.end(),
        [](const QPointF &a, const QPointF &b) { return a.y() < b.y(); });
    return minIt->y() * probeFactor();
}

double ScopeChannel::measureVavg() const
{
    if (m_data.isEmpty()) return 0.0;
    double sum = std::accumulate(m_data.begin(), m_data.end(), 0.0,
        [](double acc, const QPointF &p) { return acc + p.y(); });
    return (sum / m_data.size()) * probeFactor();
}

double ScopeChannel::measureVrms() const
{
    if (m_data.isEmpty()) return 0.0;
    double sumSquares = std::accumulate(m_data.begin(), m_data.end(), 0.0,
        [](double acc, const QPointF &p) { return acc + p.y() * p.y(); });
    return std::sqrt(sumSquares / m_data.size()) * probeFactor();
}

double ScopeChannel::measureFrequency() const
{
    double period = measurePeriod();
    return (period > 0) ? 1.0 / period : 0.0;
}

double ScopeChannel::measurePeriod() const
{
    if (m_data.size() < 3) return 0.0;
    
    // Find zero crossings (positive going)
    double avg = measureVavg() / probeFactor();
    QVector<double> crossings;
    
    for (int i = 1; i < m_data.size(); ++i) {
        double y0 = m_data[i-1].y();
        double y1 = m_data[i].y();
        if (y0 < avg && y1 >= avg) {
            // Linear interpolation to find exact crossing point
            double t0 = m_data[i-1].x();
            double t1 = m_data[i].x();
            double t = t0 + (avg - y0) * (t1 - t0) / (y1 - y0);
            crossings.append(t);
        }
    }
    
    if (crossings.size() < 2) return 0.0;
    
    // Average period from multiple crossings
    double totalPeriod = 0.0;
    for (int i = 1; i < crossings.size(); ++i) {
        totalPeriod += crossings[i] - crossings[i-1];
    }
    return totalPeriod / (crossings.size() - 1);
}

double ScopeChannel::measureRiseTime() const
{
    if (m_data.size() < 10) return 0.0;
    
    double vmin = measureVmin() / probeFactor();
    double vmax = measureVmax() / probeFactor();
    double v10 = vmin + 0.1 * (vmax - vmin);
    double v90 = vmin + 0.9 * (vmax - vmin);
    
    // Find first rising edge
    int start = -1, end = -1;
    for (int i = 1; i < m_data.size() && end < 0; ++i) {
        double y0 = m_data[i-1].y();
        double y1 = m_data[i].y();
        if (start < 0 && y0 < v10 && y1 >= v10) {
            start = i;
        } else if (start >= 0 && y0 < v90 && y1 >= v90) {
            end = i;
        }
    }
    
    if (start < 0 || end < 0) return 0.0;
    return m_data[end].x() - m_data[start].x();
}

double ScopeChannel::measureFallTime() const
{
    if (m_data.size() < 10) return 0.0;
    
    double vmin = measureVmin() / probeFactor();
    double vmax = measureVmax() / probeFactor();
    double v10 = vmin + 0.1 * (vmax - vmin);
    double v90 = vmin + 0.9 * (vmax - vmin);
    
    // Find first falling edge
    int start = -1, end = -1;
    for (int i = 1; i < m_data.size() && end < 0; ++i) {
        double y0 = m_data[i-1].y();
        double y1 = m_data[i].y();
        if (start < 0 && y0 > v90 && y1 <= v90) {
            start = i;
        } else if (start >= 0 && y0 > v10 && y1 <= v10) {
            end = i;
        }
    }
    
    if (start < 0 || end < 0) return 0.0;
    return m_data[end].x() - m_data[start].x();
}

double ScopeChannel::measureDutyCycle() const
{
    if (m_data.size() < 10) return 0.0;
    
    double avg = measureVavg() / probeFactor();
    int highCount = std::count_if(m_data.begin(), m_data.end(),
        [avg](const QPointF &p) { return p.y() > avg; });
    
    return 100.0 * highCount / m_data.size();
}

QString ScopeChannel::couplingToString(Coupling coupling)
{
    switch (coupling) {
        case Coupling::DC: return "DC";
        case Coupling::AC: return "AC";
        case Coupling::GND: return "GND";
    }
    return "DC";
}

ScopeChannel::Coupling ScopeChannel::stringToCoupling(const QString &str)
{
    if (str.toUpper() == "AC") return Coupling::AC;
    if (str.toUpper() == "GND") return Coupling::GND;
    return Coupling::DC;
}

QString ScopeChannel::probeToString(Probe probe)
{
    switch (probe) {
        case Probe::X1: return "1X";
        case Probe::X10: return "10X";
        case Probe::X100: return "100X";
        case Probe::X1000: return "1000X";
    }
    return "10X";
}

ScopeChannel::Probe ScopeChannel::stringToProbe(const QString &str)
{
    if (str == "1X") return Probe::X1;
    if (str == "100X") return Probe::X100;
    if (str == "1000X") return Probe::X1000;
    return Probe::X10;
}
