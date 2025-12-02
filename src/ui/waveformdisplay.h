#ifndef WAVEFORMDISPLAY_H
#define WAVEFORMDISPLAY_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVector>
#include <QPointF>
#include <QColor>
#include <QTimer>
#include <QElapsedTimer>
#include <QMap>
#include <QRubberBand>

class ScopeChannel;
class CursorManager;

/**
 * @brief High-performance waveform display widget using OpenGL
 */
class WaveformDisplay : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit WaveformDisplay(CursorManager *cursorManager, QWidget *parent = nullptr);
    ~WaveformDisplay() override = default;

    // Channel management
    void addChannel(ScopeChannel *channel);
    void removeChannel(const QString &name);
    void clearChannels();

    // Grid settings
    void setGridDivisions(int horizontal, int vertical);
    void setGridColor(const QColor &color);
    void setGridStyle(int style); // 0=none, 1=dots, 2=lines

    // Display settings
    void setBackgroundColor(const QColor &color);
    void setAntialiasing(bool enable);
    void setPersistence(bool enable, int decayMs = 1000);
    void setShowLabels(bool show);

    // Zoom and pan
    void setXRange(double min, double max);
    void setYRange(double min, double max);
    void zoomIn();
    void zoomOut();
    void zoomToFit();
    void resetZoom();
    void panLeft();
    void panRight();
    void panUp();
    void panDown();
    void centerView();

    // Time/voltage per division
    double timePerDiv() const { return m_timePerDiv; }
    void setTimePerDiv(double time);

    // Triggering visualization
    void setTriggerLevel(double level);
    void setTriggerPosition(double position);
    void setShowTrigger(bool show);

    // Performance stats
    int framesPerSecond() const { return m_fps; }

signals:
    void cursorMoved(double x, double y);
    void zoomChanged(double xMin, double xMax, double yMin, double yMax);
    void channelClicked(const QString &channelName);
    void toggleCursors();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void drawGrid();
    void drawWaveform(ScopeChannel *channel);
    void drawCursors();
    void drawTrigger();
    void drawLabels();
    void drawMeasurementInfo();
    void drawChannelLabels();
    
    // Coordinate transforms
    QPointF screenToScope(const QPoint &screen) const;
    QPoint scopeToScreen(const QPointF &scope) const;
    
    // Formatting helpers
    QString formatTime(double seconds) const;
    QString formatFrequency(double hz) const;
    QString formatVoltage(double volts) const;
    
    void updateFPS();

    // Channels
    QMap<QString, ScopeChannel*> m_channels;
    
    // Cursor manager
    CursorManager *m_cursorManager;

    // Grid settings
    int m_gridHorizontal = 10;
    int m_gridVertical = 8;
    QColor m_gridColor = QColor(80, 80, 80);
    int m_gridStyle = 2;

    // Display settings
    QColor m_backgroundColor = QColor(10, 10, 10);
    bool m_antialiasing = true;
    bool m_persistence = false;
    int m_persistenceDecay = 1000;
    bool m_showLabels = true;

    // Viewport
    double m_xMin = -5e-3;
    double m_xMax = 5e-3;
    double m_yMin = -4.0;
    double m_yMax = 4.0;
    double m_timePerDiv = 1e-3;

    // Trigger visualization
    double m_triggerLevel = 0.0;
    double m_triggerPosition = 0.0;
    bool m_showTrigger = true;

    // Mouse interaction
    bool m_dragging = false;
    bool m_selecting = false;
    QPoint m_lastMousePos;
    QPoint m_selectionStart;
    QRubberBand *m_rubberBand = nullptr;
    
    // Cursor dragging
    bool m_draggingCursorX1 = false;
    bool m_draggingCursorX2 = false;
    bool m_draggingCursorY1 = false;
    bool m_draggingCursorY2 = false;

    // Performance
    QElapsedTimer m_fpsTimer;
    int m_frameCount = 0;
    int m_fps = 0;
    
    // Persistence data
    struct PersistenceFrame {
        QVector<QPointF> data;
        qint64 timestamp;
    };
    QMap<QString, QVector<PersistenceFrame>> m_persistenceData;
};

#endif // WAVEFORMDISPLAY_H
