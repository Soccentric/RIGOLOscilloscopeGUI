#ifndef SCOPECHANNEL_H
#define SCOPECHANNEL_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QVector>
#include <QPointF>

/**
 * @brief Represents a single oscilloscope channel with its properties and data
 */
class ScopeChannel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(double scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(double offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(Coupling coupling READ coupling WRITE setCoupling NOTIFY couplingChanged)
    Q_PROPERTY(Probe probe READ probe WRITE setProbe NOTIFY probeChanged)

public:
    enum class ChannelType {
        Analog,
        Digital,
        Math
    };
    Q_ENUM(ChannelType)

    enum class Coupling {
        DC,
        AC,
        GND
    };
    Q_ENUM(Coupling)

    enum class Probe {
        X1,
        X10,
        X100,
        X1000
    };
    Q_ENUM(Probe)

    explicit ScopeChannel(const QString &name, ChannelType type, QObject *parent = nullptr);
    ~ScopeChannel() override = default;

    // Getters
    QString name() const { return m_name; }
    ChannelType type() const { return m_type; }
    bool isEnabled() const { return m_enabled; }
    double scale() const { return m_scale; }
    double offset() const { return m_offset; }
    QColor color() const { return m_color; }
    Coupling coupling() const { return m_coupling; }
    Probe probe() const { return m_probe; }
    const QVector<QPointF>& data() const { return m_data; }

    // Setters
    void setEnabled(bool enabled);
    void setScale(double scale);
    void setOffset(double offset);
    void setColor(const QColor &color);
    void setCoupling(Coupling coupling);
    void setProbe(Probe probe);
    void setData(const QVector<QPointF> &data);
    void clearData();

    // Measurements
    double measureVpp() const;
    double measureVmax() const;
    double measureVmin() const;
    double measureVavg() const;
    double measureVrms() const;
    double measureFrequency() const;
    double measurePeriod() const;
    double measureRiseTime() const;
    double measureFallTime() const;
    double measureDutyCycle() const;

    // Probe factor
    double probeFactor() const;

    // Static helpers
    static QString couplingToString(Coupling coupling);
    static Coupling stringToCoupling(const QString &str);
    static QString probeToString(Probe probe);
    static Probe stringToProbe(const QString &str);

signals:
    void enabledChanged(bool enabled);
    void scaleChanged(double scale);
    void offsetChanged(double offset);
    void colorChanged(const QColor &color);
    void couplingChanged(Coupling coupling);
    void probeChanged(Probe probe);
    void dataChanged();

private:
    QString m_name;
    ChannelType m_type;
    bool m_enabled = false;
    double m_scale = 1.0;      // Volts per division
    double m_offset = 0.0;     // Vertical offset
    QColor m_color;
    Coupling m_coupling = Coupling::DC;
    Probe m_probe = Probe::X10;
    QVector<QPointF> m_data;
};

#endif // SCOPECHANNEL_H
