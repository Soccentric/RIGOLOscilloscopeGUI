#include "waveformdisplay.h"
#include "../core/cursormanager.h"
#include "../core/scopechannel.h"

#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWheelEvent>
#include <QtMath>

WaveformDisplay::WaveformDisplay(CursorManager *cursorManager, QWidget *parent)
    : QOpenGLWidget(parent), m_cursorManager(cursorManager) {
  setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);

  m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

  m_fpsTimer.start();
}

void WaveformDisplay::addChannel(ScopeChannel *channel) {
  if (channel) {
    m_channels[channel->name()] = channel;
    connect(channel, &ScopeChannel::dataChanged, this, [this]() { update(); });
  }
}

void WaveformDisplay::removeChannel(const QString &name) {
  m_channels.remove(name);
  m_persistenceData.remove(name);
  update();
}

void WaveformDisplay::clearChannels() {
  m_channels.clear();
  m_persistenceData.clear();
  update();
}

void WaveformDisplay::setGridDivisions(int horizontal, int vertical) {
  m_gridHorizontal = horizontal;
  m_gridVertical = vertical;
  update();
}

void WaveformDisplay::setGridColor(const QColor &color) {
  m_gridColor = color;
  update();
}

void WaveformDisplay::setGridStyle(int style) {
  m_gridStyle = style;
  update();
}

void WaveformDisplay::setBackgroundColor(const QColor &color) {
  m_backgroundColor = color;
  update();
}

void WaveformDisplay::setAntialiasing(bool enable) {
  m_antialiasing = enable;
  update();
}

void WaveformDisplay::setPersistence(bool enable, int decayMs) {
  m_persistence = enable;
  m_persistenceDecay = decayMs;
  if (!enable) {
    m_persistenceData.clear();
  }
  update();
}

void WaveformDisplay::setShowLabels(bool show) {
  m_showLabels = show;
  update();
}

void WaveformDisplay::setXRange(double min, double max) {
  m_xMin = min;
  m_xMax = max;
  emit zoomChanged(m_xMin, m_xMax, m_yMin, m_yMax);
  update();
}

void WaveformDisplay::setYRange(double min, double max) {
  m_yMin = min;
  m_yMax = max;
  emit zoomChanged(m_xMin, m_xMax, m_yMin, m_yMax);
  update();
}

void WaveformDisplay::zoomIn() {
  double xCenter = (m_xMin + m_xMax) / 2;
  double yCenter = (m_yMin + m_yMax) / 2;
  double xRange = (m_xMax - m_xMin) * 0.8;
  double yRange = (m_yMax - m_yMin) * 0.8;

  setXRange(xCenter - xRange / 2, xCenter + xRange / 2);
  setYRange(yCenter - yRange / 2, yCenter + yRange / 2);
}

void WaveformDisplay::zoomOut() {
  double xCenter = (m_xMin + m_xMax) / 2;
  double yCenter = (m_yMin + m_yMax) / 2;
  double xRange = (m_xMax - m_xMin) * 1.25;
  double yRange = (m_yMax - m_yMin) * 1.25;

  setXRange(xCenter - xRange / 2, xCenter + xRange / 2);
  setYRange(yCenter - yRange / 2, yCenter + yRange / 2);
}

void WaveformDisplay::zoomToFit() {
  double xMin = 0, xMax = 0, yMin = 0, yMax = 0;
  bool first = true;

  for (auto *channel : m_channels) {
    if (!channel->isEnabled() || channel->data().isEmpty())
      continue;

    for (const QPointF &p : channel->data()) {
      if (first) {
        xMin = xMax = p.x();
        yMin = yMax = p.y();
        first = false;
      } else {
        xMin = qMin(xMin, p.x());
        xMax = qMax(xMax, p.x());
        yMin = qMin(yMin, p.y());
        yMax = qMax(yMax, p.y());
      }
    }
  }

  if (!first) {
    double xMargin = (xMax - xMin) * 0.1;
    double yMargin = (yMax - yMin) * 0.1;
    setXRange(xMin - xMargin, xMax + xMargin);
    setYRange(yMin - yMargin, yMax + yMargin);
  }
}

void WaveformDisplay::resetZoom() {
  m_xMin = -5 * m_timePerDiv;
  m_xMax = 5 * m_timePerDiv;
  m_yMin = -4.0;
  m_yMax = 4.0;
  emit zoomChanged(m_xMin, m_xMax, m_yMin, m_yMax);
  update();
}

void WaveformDisplay::panLeft() {
  double panAmount = (m_xMax - m_xMin) * 0.1;
  m_xMin -= panAmount;
  m_xMax -= panAmount;
  emit zoomChanged(m_xMin, m_xMax, m_yMin, m_yMax);
  update();
}

void WaveformDisplay::panRight() {
  double panAmount = (m_xMax - m_xMin) * 0.1;
  m_xMin += panAmount;
  m_xMax += panAmount;
  emit zoomChanged(m_xMin, m_xMax, m_yMin, m_yMax);
  update();
}

void WaveformDisplay::panUp() {
  double panAmount = (m_yMax - m_yMin) * 0.1;
  m_yMin += panAmount;
  m_yMax += panAmount;
  emit zoomChanged(m_xMin, m_xMax, m_yMin, m_yMax);
  update();
}

void WaveformDisplay::panDown() {
  double panAmount = (m_yMax - m_yMin) * 0.1;
  m_yMin -= panAmount;
  m_yMax -= panAmount;
  emit zoomChanged(m_xMin, m_xMax, m_yMin, m_yMax);
  update();
}

void WaveformDisplay::centerView() {
  double xCenter = (m_xMin + m_xMax) / 2;
  double yCenter = (m_yMin + m_yMax) / 2;
  double xRange = m_xMax - m_xMin;
  double yRange = m_yMax - m_yMin;

  m_xMin = xCenter - xRange / 2;
  m_xMax = xCenter + xRange / 2;
  m_yMin = yCenter - yRange / 2;
  m_yMax = yCenter + yRange / 2;

  emit zoomChanged(m_xMin, m_xMax, m_yMin, m_yMax);
  update();
}

void WaveformDisplay::setTimePerDiv(double time) {
  m_timePerDiv = time;
  m_xMin = -5 * time;
  m_xMax = 5 * time;
  emit zoomChanged(m_xMin, m_xMax, m_yMin, m_yMax);
  update();
}

void WaveformDisplay::setTriggerLevel(double level) {
  m_triggerLevel = level;
  update();
}

void WaveformDisplay::setTriggerPosition(double position) {
  m_triggerPosition = position;
  update();
}

void WaveformDisplay::setShowTrigger(bool show) {
  m_showTrigger = show;
  update();
}

void WaveformDisplay::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(m_backgroundColor.redF(), m_backgroundColor.greenF(),
               m_backgroundColor.blueF(), 1.0f);

  if (m_antialiasing) {
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  }
}

void WaveformDisplay::paintGL() {
  QPainter painter(this);

  if (m_antialiasing) {
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
  }

  // Draw 3D Sunken Border (CRT Bezel)
  QPen lightPen(QColor(255, 255, 255));
  QPen darkPen(QColor(128, 128, 128));
  QPen shadowPen(QColor(0, 0, 0));

  // Outer Bezel
  painter.setPen(shadowPen);
  painter.drawRect(0, 0, width() - 1, height() - 1);

  // Sunken effect
  painter.setPen(darkPen);
  painter.drawLine(0, 0, width(), 0);
  painter.drawLine(0, 0, 0, height());

  painter.setPen(lightPen);
  painter.drawLine(width() - 1, 0, width() - 1, height());
  painter.drawLine(0, height() - 1, width(), height() - 1);

  // Background (CRT Screen)
  painter.fillRect(rect().adjusted(2, 2, -2, -2), Qt::black);

  // Draw grid
  drawGrid();

  // Draw waveforms
  for (auto *channel : m_channels) {
    if (channel->isEnabled()) {
      drawWaveform(channel);
    }
  }

  // Draw cursors
  if (m_cursorManager && m_cursorManager->isVisible()) {
    drawCursors();
  }

  // Draw trigger indicator
  if (m_showTrigger) {
    drawTrigger();
  }

  // Draw labels
  if (m_showLabels) {
    drawLabels();
    drawChannelLabels();
  }

  // Update FPS
  updateFPS();

  painter.end();
}

void WaveformDisplay::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void WaveformDisplay::drawGrid() {
  if (m_gridStyle == 0)
    return;

  QPainter painter(this);
  // Classic Green CRT Grid
  painter.setPen(QPen(QColor(0, 80, 0), 1, Qt::DotLine));

  int w = width();
  int h = height();
  int margin = 50; // Margin for labels

  int plotW = w - 2 * margin;
  int plotH = h - 2 * margin;

  // Draw border
  painter.drawRect(margin, margin, plotW, plotH);

  // Draw vertical divisions (time)
  double divW = plotW / static_cast<double>(m_gridHorizontal);
  for (int i = 1; i < m_gridHorizontal; ++i) {
    int x = margin + static_cast<int>(i * divW);
    painter.drawLine(x, margin, x, margin + plotH);
  }

  // Draw horizontal divisions (voltage)
  double divH = plotH / static_cast<double>(m_gridVertical);
  for (int i = 1; i < m_gridVertical; ++i) {
    int y = margin + static_cast<int>(i * divH);
    painter.drawLine(margin, y, margin + plotW, y);
  }

  // Draw center lines (brighter)
  painter.setPen(QPen(QColor(0, 120, 0), 1, Qt::SolidLine));

  // Vertical center
  int cx = margin + plotW / 2;
  painter.drawLine(cx, margin, cx, margin + plotH);

  // Horizontal center
  int cy = margin + plotH / 2;
  painter.drawLine(margin, cy, margin + plotW, cy);

  painter.end();
}

void WaveformDisplay::drawWaveform(ScopeChannel *channel) {
  if (!channel || channel->data().isEmpty())
    return;

  QPainter painter(this);
  if (m_antialiasing) {
    painter.setRenderHint(QPainter::Antialiasing);
  }

  int margin = 50;
  int plotW = width() - 2 * margin;
  int plotH = height() - 2 * margin;

  // Create path for waveform
  QPainterPath path;
  bool first = true;

  const auto &data = channel->data();
  double xScale = plotW / (m_xMax - m_xMin);
  double yScale = plotH / (m_yMax - m_yMin);

  for (const QPointF &p : data) {
    double x = margin + (p.x() - m_xMin) * xScale;
    double y = margin + plotH - (p.y() - m_yMin) * yScale;

    // Skip points outside visible area
    if (x < margin - 10 || x > margin + plotW + 10)
      continue;

    if (first) {
      path.moveTo(x, y);
      first = false;
    } else {
      path.lineTo(x, y);
    }
  }

  // Draw persistence (faded previous waveforms)
  if (m_persistence && m_persistenceData.contains(channel->name())) {
    qint64 now = m_fpsTimer.elapsed();
    auto &frames = m_persistenceData[channel->name()];

    // Remove expired frames
    frames.erase(std::remove_if(frames.begin(), frames.end(),
                                [now, this](const PersistenceFrame &f) {
                                  return now - f.timestamp > m_persistenceDecay;
                                }),
                 frames.end());

    // Draw old frames with decreasing opacity
    for (const auto &frame : frames) {
      double age = now - frame.timestamp;
      double alpha = 1.0 - (age / m_persistenceDecay);

      QColor fadeColor = channel->color();
      fadeColor.setAlphaF(alpha * 0.5);

      QPainterPath oldPath;
      bool firstOld = true;
      for (const QPointF &p : frame.data) {
        double x = margin + (p.x() - m_xMin) * xScale;
        double y = margin + plotH - (p.y() - m_yMin) * yScale;

        if (firstOld) {
          oldPath.moveTo(x, y);
          firstOld = false;
        } else {
          oldPath.lineTo(x, y);
        }
      }

      painter.setPen(QPen(fadeColor, 1));
      painter.drawPath(oldPath);
    }

    // Add current frame
    PersistenceFrame currentFrame;
    currentFrame.data = data;
    currentFrame.timestamp = now;
    frames.append(currentFrame);
  }

  // Draw current waveform
  painter.setPen(QPen(channel->color(), 2));
  painter.drawPath(path);

  painter.end();
}

void WaveformDisplay::drawCursors() {
  if (!m_cursorManager)
    return;

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  int margin = 50;
  int plotW = width() - 2 * margin;
  int plotH = height() - 2 * margin;

  QColor cursorColor(255, 255, 255, 200);
  QPen cursorPen(cursorColor, 1, Qt::DashLine);
  painter.setPen(cursorPen);

  auto type = m_cursorManager->type();

  if (type == CursorManager::CursorType::Horizontal ||
      type == CursorManager::CursorType::Both) {
    // X cursors (vertical lines)
    double x1 = margin + (m_cursorManager->cursorX1() - m_xMin) * plotW /
                             (m_xMax - m_xMin);
    double x2 = margin + (m_cursorManager->cursorX2() - m_xMin) * plotW /
                             (m_xMax - m_xMin);

    painter.drawLine(QPointF(x1, margin), QPointF(x1, margin + plotH));
    painter.drawLine(QPointF(x2, margin), QPointF(x2, margin + plotH));

    // Labels
    painter.setPen(cursorColor);
    QFont font = painter.font();
    font.setPointSize(9);
    painter.setFont(font);

    QString x1Text =
        QString("X1: %1").arg(formatTime(m_cursorManager->cursorX1()));
    QString x2Text =
        QString("X2: %1").arg(formatTime(m_cursorManager->cursorX2()));
    QString dxText =
        QString("ΔX: %1").arg(formatTime(m_cursorManager->deltaX()));
    QString freqText =
        QString("1/ΔX: %1").arg(formatFrequency(m_cursorManager->frequency()));

    painter.drawText(QPointF(x1 + 5, margin + 15), x1Text);
    painter.drawText(QPointF(x2 + 5, margin + 15), x2Text);
    painter.drawText(QPointF(margin + 5, margin + plotH - 25), dxText);
    painter.drawText(QPointF(margin + 5, margin + plotH - 10), freqText);
  }

  if (type == CursorManager::CursorType::Vertical ||
      type == CursorManager::CursorType::Both) {
    // Y cursors (horizontal lines)
    double y1 =
        margin + plotH -
        (m_cursorManager->cursorY1() - m_yMin) * plotH / (m_yMax - m_yMin);
    double y2 =
        margin + plotH -
        (m_cursorManager->cursorY2() - m_yMin) * plotH / (m_yMax - m_yMin);

    painter.drawLine(QPointF(margin, y1), QPointF(margin + plotW, y1));
    painter.drawLine(QPointF(margin, y2), QPointF(margin + plotW, y2));

    // Labels
    QString y1Text =
        QString("Y1: %1 V").arg(m_cursorManager->cursorY1(), 0, 'f', 3);
    QString y2Text =
        QString("Y2: %1 V").arg(m_cursorManager->cursorY2(), 0, 'f', 3);
    QString dyText =
        QString("ΔY: %1 V").arg(m_cursorManager->deltaY(), 0, 'f', 3);

    painter.drawText(QPointF(margin + plotW - 80, y1 - 5), y1Text);
    painter.drawText(QPointF(margin + plotW - 80, y2 - 5), y2Text);
    painter.drawText(QPointF(margin + plotW - 80, margin + 15), dyText);
  }

  painter.end();
}

void WaveformDisplay::drawTrigger() {
  QPainter painter(this);

  int margin = 50;
  int plotW = width() - 2 * margin;
  int plotH = height() - 2 * margin;

  // Trigger level indicator (on right edge)
  double y =
      margin + plotH - (m_triggerLevel - m_yMin) * plotH / (m_yMax - m_yMin);

  if (y >= margin && y <= margin + plotH) {
    // Draw triangle
    QPolygonF triangle;
    triangle << QPointF(margin + plotW, y)
             << QPointF(margin + plotW + 10, y - 5)
             << QPointF(margin + plotW + 10, y + 5);

    painter.setBrush(QColor(255, 200, 0));
    painter.setPen(Qt::NoPen);
    painter.drawPolygon(triangle);

    // Draw horizontal line
    painter.setPen(QPen(QColor(255, 200, 0, 100), 1, Qt::DotLine));
    painter.drawLine(margin, static_cast<int>(y), margin + plotW,
                     static_cast<int>(y));
  }

  // Trigger position indicator (on top edge)
  double x = margin + (m_triggerPosition - m_xMin) * plotW / (m_xMax - m_xMin);

  if (x >= margin && x <= margin + plotW) {
    QPolygonF triangle;
    triangle << QPointF(x, margin) << QPointF(x - 5, margin - 10)
             << QPointF(x + 5, margin - 10);

    painter.setBrush(QColor(255, 200, 0));
    painter.setPen(Qt::NoPen);
    painter.drawPolygon(triangle);
  }

  painter.end();
}

void WaveformDisplay::drawLabels() {
  QPainter painter(this);
  painter.setRenderHint(QPainter::TextAntialiasing);

  int margin = 50;
  int plotW = width() - 2 * margin;
  int plotH = height() - 2 * margin;

  QFont font = painter.font();
  font.setPointSize(9);
  painter.setFont(font);
  painter.setPen(QColor(180, 180, 180));

  // X-axis labels (time)
  double timeRange = m_xMax - m_xMin;
  double timeStep = timeRange / m_gridHorizontal;

  for (int i = 0; i <= m_gridHorizontal; ++i) {
    double time = m_xMin + i * timeStep;
    int x = margin + static_cast<int>(i * plotW / m_gridHorizontal);

    QString label = formatTime(time);
    QFontMetrics fm(font);
    int textWidth = fm.horizontalAdvance(label);

    painter.drawText(x - textWidth / 2, height() - 10, label);
  }

  // Y-axis labels (voltage)
  double voltRange = m_yMax - m_yMin;
  double voltStep = voltRange / m_gridVertical;

  for (int i = 0; i <= m_gridVertical; ++i) {
    double volt = m_yMax - i * voltStep;
    int y = margin + static_cast<int>(i * plotH / m_gridVertical);

    QString label = QString::number(volt, 'f', 1) + " V";
    QFontMetrics fm(font);
    int textWidth = fm.horizontalAdvance(label);

    painter.drawText(margin - textWidth - 5, y + 4, label);
  }

  // Title
  font.setPointSize(11);
  font.setBold(true);
  painter.setFont(font);
  painter.drawText(margin, 20,
                   QString("Time: %1/div").arg(formatTime(m_timePerDiv)));

  painter.end();
}

void WaveformDisplay::drawChannelLabels() {
  QPainter painter(this);
  painter.setRenderHint(QPainter::TextAntialiasing);

  int margin = 50;
  int y = height() - 35;
  int x = margin;

  QFont font = painter.font();
  font.setPointSize(10);
  font.setBold(true);
  painter.setFont(font);

  for (auto *channel : m_channels) {
    if (!channel->isEnabled())
      continue;

    QString label =
        QString("%1: %2 V/div").arg(channel->name()).arg(channel->scale());

    // Draw background
    QFontMetrics fm(font);
    int textWidth = fm.horizontalAdvance(label);
    painter.fillRect(x - 2, y - 12, textWidth + 8, 16,
                     QColor(channel->color()).darker(200));

    // Draw text
    painter.setPen(channel->color());
    painter.drawText(x + 2, y, label);

    x += textWidth + 20;
  }

  painter.end();
}

QString WaveformDisplay::formatTime(double seconds) const {
  double absSeconds = qAbs(seconds);
  QString prefix = seconds < 0 ? "-" : "";

  if (absSeconds == 0)
    return "0 s";
  else if (absSeconds >= 1)
    return prefix + QString::number(absSeconds, 'f', 3) + " s";
  else if (absSeconds >= 1e-3)
    return prefix + QString::number(absSeconds * 1e3, 'f', 3) + " ms";
  else if (absSeconds >= 1e-6)
    return prefix + QString::number(absSeconds * 1e6, 'f', 3) + " µs";
  else
    return prefix + QString::number(absSeconds * 1e9, 'f', 3) + " ns";
}

QString WaveformDisplay::formatFrequency(double hz) const {
  if (hz == 0)
    return "0 Hz";
  else if (hz >= 1e9)
    return QString::number(hz / 1e9, 'f', 3) + " GHz";
  else if (hz >= 1e6)
    return QString::number(hz / 1e6, 'f', 3) + " MHz";
  else if (hz >= 1e3)
    return QString::number(hz / 1e3, 'f', 3) + " kHz";
  else
    return QString::number(hz, 'f', 3) + " Hz";
}

QString WaveformDisplay::formatVoltage(double volts) const {
  double absVolts = qAbs(volts);
  QString prefix = volts < 0 ? "-" : "";

  if (absVolts == 0)
    return "0 V";
  else if (absVolts >= 1)
    return prefix + QString::number(absVolts, 'f', 3) + " V";
  else if (absVolts >= 1e-3)
    return prefix + QString::number(absVolts * 1e3, 'f', 3) + " mV";
  else
    return prefix + QString::number(absVolts * 1e6, 'f', 3) + " µV";
}

QPointF WaveformDisplay::screenToScope(const QPoint &screen) const {
  int margin = 50;
  int plotW = width() - 2 * margin;
  int plotH = height() - 2 * margin;

  double x = m_xMin + (screen.x() - margin) * (m_xMax - m_xMin) / plotW;
  double y = m_yMax - (screen.y() - margin) * (m_yMax - m_yMin) / plotH;

  return QPointF(x, y);
}

QPoint WaveformDisplay::scopeToScreen(const QPointF &scope) const {
  int margin = 50;
  int plotW = width() - 2 * margin;
  int plotH = height() - 2 * margin;

  int x = margin +
          static_cast<int>((scope.x() - m_xMin) * plotW / (m_xMax - m_xMin));
  int y = margin +
          static_cast<int>((m_yMax - scope.y()) * plotH / (m_yMax - m_yMin));

  return QPoint(x, y);
}

void WaveformDisplay::mousePressEvent(QMouseEvent *event) {
  m_lastMousePos = event->pos();

  if (event->button() == Qt::LeftButton) {
    if (event->modifiers() & Qt::ControlModifier) {
      // Start selection for zoom
      m_selecting = true;
      m_selectionStart = event->pos();
      m_rubberBand->setGeometry(QRect(m_selectionStart, QSize()));
      m_rubberBand->show();
    } else {
      // Check if clicking on a cursor
      // ... cursor hit testing
      m_dragging = true;
    }
  }

  QOpenGLWidget::mousePressEvent(event);
}

void WaveformDisplay::mouseMoveEvent(QMouseEvent *event) {
  QPointF scopePos = screenToScope(event->pos());
  emit cursorMoved(scopePos.x(), scopePos.y());

  if (m_selecting) {
    m_rubberBand->setGeometry(
        QRect(m_selectionStart, event->pos()).normalized());
  } else if (m_dragging && event->buttons() & Qt::LeftButton) {
    // Pan
    QPoint delta = event->pos() - m_lastMousePos;
    double dx = -delta.x() * (m_xMax - m_xMin) / (width() - 100);
    double dy = delta.y() * (m_yMax - m_yMin) / (height() - 100);

    m_xMin += dx;
    m_xMax += dx;
    m_yMin += dy;
    m_yMax += dy;

    emit zoomChanged(m_xMin, m_xMax, m_yMin, m_yMax);
    update();
  }

  m_lastMousePos = event->pos();
  QOpenGLWidget::mouseMoveEvent(event);
}

void WaveformDisplay::mouseReleaseEvent(QMouseEvent *event) {
  if (m_selecting) {
    m_selecting = false;
    m_rubberBand->hide();

    QRect selection = m_rubberBand->geometry();
    if (selection.width() > 10 && selection.height() > 10) {
      QPointF topLeft = screenToScope(selection.topLeft());
      QPointF bottomRight = screenToScope(selection.bottomRight());

      setXRange(topLeft.x(), bottomRight.x());
      setYRange(bottomRight.y(), topLeft.y());
    }
  }

  m_dragging = false;
  m_draggingCursorX1 = false;
  m_draggingCursorX2 = false;
  m_draggingCursorY1 = false;
  m_draggingCursorY2 = false;

  QOpenGLWidget::mouseReleaseEvent(event);
}

void WaveformDisplay::wheelEvent(QWheelEvent *event) {
  double zoomFactor = event->angleDelta().y() > 0 ? 0.9 : 1.1;

  QPointF center = screenToScope(event->position().toPoint());

  if (event->modifiers() & Qt::ShiftModifier) {
    // Horizontal zoom only
    double xRange = (m_xMax - m_xMin) * zoomFactor;
    m_xMin = center.x() - xRange / 2;
    m_xMax = center.x() + xRange / 2;
  } else if (event->modifiers() & Qt::ControlModifier) {
    // Vertical zoom only
    double yRange = (m_yMax - m_yMin) * zoomFactor;
    m_yMin = center.y() - yRange / 2;
    m_yMax = center.y() + yRange / 2;
  } else {
    // Both
    double xRange = (m_xMax - m_xMin) * zoomFactor;
    double yRange = (m_yMax - m_yMin) * zoomFactor;

    m_xMin = center.x() - xRange / 2;
    m_xMax = center.x() + xRange / 2;
    m_yMin = center.y() - yRange / 2;
    m_yMax = center.y() + yRange / 2;
  }

  emit zoomChanged(m_xMin, m_xMax, m_yMin, m_yMax);
  update();

  event->accept();
}

void WaveformDisplay::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Plus:
  case Qt::Key_Equal:
    zoomIn();
    break;
  case Qt::Key_Minus:
    zoomOut();
    break;
  case Qt::Key_0:
    resetZoom();
    break;
  case Qt::Key_F:
    zoomToFit();
    break;
  case Qt::Key_Left:
    panLeft();
    break;
  case Qt::Key_Right:
    panRight();
    break;
  case Qt::Key_Up:
    panUp();
    break;
  case Qt::Key_Down:
    panDown();
    break;
  case Qt::Key_Home:
    centerView();
    break;
  case Qt::Key_R:
    // Toggle rulers/cursors
    emit toggleCursors();
    break;
  default:
    QOpenGLWidget::keyPressEvent(event);
  }
}

void WaveformDisplay::updateFPS() {
  m_frameCount++;

  if (m_fpsTimer.elapsed() >= 1000) {
    m_fps = m_frameCount;
    m_frameCount = 0;
    m_fpsTimer.restart();
  }
}
