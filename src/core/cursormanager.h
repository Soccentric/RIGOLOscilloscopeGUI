#ifndef CURSORMANAGER_H
#define CURSORMANAGER_H

#include <QObject>
#include <QString>

/**
 * @brief Manages cursor positions and measurements
 */
class CursorManager : public QObject
{
    Q_OBJECT

public:
    enum class CursorMode {
        Off,
        Manual,
        Track,
        Auto,
        XY
    };
    Q_ENUM(CursorMode)

    enum class CursorType {
        Horizontal,  // Time cursors (X1, X2)
        Vertical,    // Voltage cursors (Y1, Y2)
        Both
    };
    Q_ENUM(CursorType)

    explicit CursorManager(QObject *parent = nullptr);
    ~CursorManager() override = default;

    // Cursor mode
    CursorMode mode() const { return m_mode; }
    void setMode(CursorMode mode);

    CursorType type() const { return m_type; }
    void setType(CursorType type);

    // Source channel
    QString sourceChannel() const { return m_sourceChannel; }
    void setSourceChannel(const QString &channel);

    // Horizontal cursors (time)
    double cursorX1() const { return m_x1; }
    double cursorX2() const { return m_x2; }
    void setCursorX1(double x);
    void setCursorX2(double x);

    // Vertical cursors (voltage)
    double cursorY1() const { return m_y1; }
    double cursorY2() const { return m_y2; }
    void setCursorY1(double y);
    void setCursorY2(double y);

    // Calculated values
    double deltaX() const { return m_x2 - m_x1; }
    double deltaY() const { return m_y2 - m_y1; }
    double frequency() const { return (deltaX() != 0) ? 1.0 / std::abs(deltaX()) : 0.0; }

    // Visibility
    bool isVisible() const { return m_mode != CursorMode::Off; }

signals:
    void modeChanged(CursorMode mode);
    void typeChanged(CursorType type);
    void sourceChannelChanged(const QString &channel);
    void cursorX1Changed(double x);
    void cursorX2Changed(double x);
    void cursorY1Changed(double y);
    void cursorY2Changed(double y);
    void cursorsChanged();

private:
    CursorMode m_mode = CursorMode::Off;
    CursorType m_type = CursorType::Horizontal;
    QString m_sourceChannel = "CH1";
    double m_x1 = 0.0;
    double m_x2 = 0.0;
    double m_y1 = 0.0;
    double m_y2 = 0.0;
};

#endif // CURSORMANAGER_H
