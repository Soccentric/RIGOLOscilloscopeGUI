/**
 * @file footerwidget.cpp
 * @brief Implementation of the modern footer widget
 */

#include "footerwidget.h"
#include "../core/scopechannel.h"
#include "../core/measurementengine.h"

#include <QDateTime>
#include <QProcess>
#include <QtMath>
#include <QRandomGenerator>

FooterWidget::FooterWidget(const QMap<QString, ScopeChannel*> &channels,
                           MeasurementEngine *engine,
                           QWidget *parent)
    : QWidget(parent)
    , m_channels(channels)
    , m_engine(engine)
{
    setupUI();
    applyStyles();
    
    // Setup update timers
    m_readingsTimer = new QTimer(this);
    connect(m_readingsTimer, &QTimer::timeout, this, &FooterWidget::updateReadings);
    m_readingsTimer->start(200); // Update readings 5x per second
    
    m_perfTimer = new QTimer(this);
    connect(m_perfTimer, &QTimer::timeout, this, &FooterWidget::updatePerformance);
    m_perfTimer->start(1000); // Update performance 1x per second
}

void FooterWidget::setupUI()
{
    setFixedHeight(48);
    
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(12, 6, 12, 6);
    mainLayout->setSpacing(20);
    
    // === Running Indicator ===
    m_runningIndicator = new QLabel("⏹ STOPPED");
    m_runningIndicator->setObjectName("runningIndicator");
    m_runningIndicator->setFont(QFont("Segoe UI Semibold", 10));
    m_runningIndicator->setMinimumWidth(100);
    mainLayout->addWidget(m_runningIndicator);
    
    // Separator
    QFrame *sep1 = new QFrame;
    sep1->setFrameShape(QFrame::VLine);
    sep1->setObjectName("footerSeparator");
    mainLayout->addWidget(sep1);
    
    // === Channel Readings Section ===
    QWidget *readingsSection = new QWidget;
    QHBoxLayout *readingsLayout = new QHBoxLayout(readingsSection);
    readingsLayout->setContentsMargins(0, 0, 0, 0);
    readingsLayout->setSpacing(16);
    
    // Create reading widgets for each channel
    QStringList channelOrder = {"CH1", "CH2", "CH3", "CH4"};
    QStringList channelColors = {"#ffeb3b", "#00e5ff", "#ff4081", "#00e676"};
    
    for (int i = 0; i < channelOrder.size(); ++i) {
        const QString &name = channelOrder[i];
        
        ChannelReadingWidget readingWidget;
        
        readingWidget.container = new QWidget;
        readingWidget.container->setObjectName("channelReading");
        readingWidget.container->setCursor(Qt::PointingHandCursor);
        
        QHBoxLayout *chLayout = new QHBoxLayout(readingWidget.container);
        chLayout->setContentsMargins(8, 2, 8, 2);
        chLayout->setSpacing(8);
        
        // Channel name badge
        readingWidget.nameLabel = new QLabel(name);
        readingWidget.nameLabel->setObjectName("channelBadge");
        readingWidget.nameLabel->setFont(QFont("Segoe UI Semibold", 9));
        readingWidget.nameLabel->setStyleSheet(QString(
            "background-color: %1; color: #000; padding: 2px 6px; border-radius: 3px;"
        ).arg(channelColors[i]));
        chLayout->addWidget(readingWidget.nameLabel);
        
        // Vpp reading
        QWidget *vppWidget = new QWidget;
        QVBoxLayout *vppLayout = new QVBoxLayout(vppWidget);
        vppLayout->setContentsMargins(0, 0, 0, 0);
        vppLayout->setSpacing(0);
        
        QLabel *vppTitle = new QLabel("Vpp");
        vppTitle->setObjectName("readingTitle");
        vppTitle->setFont(QFont("Segoe UI", 8));
        vppLayout->addWidget(vppTitle);
        
        readingWidget.vppLabel = new QLabel("---");
        readingWidget.vppLabel->setObjectName("readingValue");
        readingWidget.vppLabel->setFont(QFont("Consolas", 10, QFont::Bold));
        readingWidget.vppLabel->setStyleSheet(QString("color: %1;").arg(channelColors[i]));
        vppLayout->addWidget(readingWidget.vppLabel);
        
        chLayout->addWidget(vppWidget);
        
        // Frequency reading
        QWidget *freqWidget = new QWidget;
        QVBoxLayout *freqLayout = new QVBoxLayout(freqWidget);
        freqLayout->setContentsMargins(0, 0, 0, 0);
        freqLayout->setSpacing(0);
        
        QLabel *freqTitle = new QLabel("Freq");
        freqTitle->setObjectName("readingTitle");
        freqTitle->setFont(QFont("Segoe UI", 8));
        freqLayout->addWidget(freqTitle);
        
        readingWidget.freqLabel = new QLabel("---");
        readingWidget.freqLabel->setObjectName("readingValue");
        readingWidget.freqLabel->setFont(QFont("Consolas", 10, QFont::Bold));
        readingWidget.freqLabel->setStyleSheet(QString("color: %1;").arg(channelColors[i]));
        freqLayout->addWidget(readingWidget.freqLabel);
        
        chLayout->addWidget(freqWidget);
        
        readingsLayout->addWidget(readingWidget.container);
        m_channelReadings[name] = readingWidget;
    }
    
    mainLayout->addWidget(readingsSection);
    
    // Spacer
    mainLayout->addStretch();
    
    // === Trigger Status Section ===
    QWidget *triggerSection = new QWidget;
    QHBoxLayout *triggerLayout = new QHBoxLayout(triggerSection);
    triggerLayout->setContentsMargins(0, 0, 0, 0);
    triggerLayout->setSpacing(8);
    
    QLabel *trigIcon = new QLabel("⚡");
    trigIcon->setFont(QFont("Segoe UI", 12));
    triggerLayout->addWidget(trigIcon);
    
    QWidget *trigInfo = new QWidget;
    QVBoxLayout *trigInfoLayout = new QVBoxLayout(trigInfo);
    trigInfoLayout->setContentsMargins(0, 0, 0, 0);
    trigInfoLayout->setSpacing(0);
    
    QLabel *trigTitle = new QLabel("TRIGGER");
    trigTitle->setObjectName("readingTitle");
    trigTitle->setFont(QFont("Segoe UI", 8));
    trigInfoLayout->addWidget(trigTitle);
    
    m_triggerStatusLabel = new QLabel("Waiting");
    m_triggerStatusLabel->setObjectName("triggerStatus");
    m_triggerStatusLabel->setFont(QFont("Segoe UI Semibold", 10));
    trigInfoLayout->addWidget(m_triggerStatusLabel);
    
    triggerLayout->addWidget(trigInfo);
    
    mainLayout->addWidget(triggerSection);
    
    // Separator
    QFrame *sep2 = new QFrame;
    sep2->setFrameShape(QFrame::VLine);
    sep2->setObjectName("footerSeparator");
    mainLayout->addWidget(sep2);
    
    // === Acquisition Section ===
    QWidget *acqSection = new QWidget;
    QHBoxLayout *acqLayout = new QHBoxLayout(acqSection);
    acqLayout->setContentsMargins(0, 0, 0, 0);
    acqLayout->setSpacing(12);
    
    m_acqCountLabel = new QLabel("ACQ: 0");
    m_acqCountLabel->setObjectName("acqCount");
    m_acqCountLabel->setFont(QFont("Consolas", 10));
    acqLayout->addWidget(m_acqCountLabel);
    
    m_acqProgress = new QProgressBar;
    m_acqProgress->setObjectName("acqProgress");
    m_acqProgress->setFixedWidth(80);
    m_acqProgress->setFixedHeight(8);
    m_acqProgress->setTextVisible(false);
    m_acqProgress->setRange(0, 100);
    m_acqProgress->setValue(0);
    acqLayout->addWidget(m_acqProgress);
    
    mainLayout->addWidget(acqSection);
    
    // Separator
    QFrame *sep3 = new QFrame;
    sep3->setFrameShape(QFrame::VLine);
    sep3->setObjectName("footerSeparator");
    mainLayout->addWidget(sep3);
    
    // === Performance Section ===
    QWidget *perfSection = new QWidget;
    QHBoxLayout *perfLayout = new QHBoxLayout(perfSection);
    perfLayout->setContentsMargins(0, 0, 0, 0);
    perfLayout->setSpacing(12);
    
    m_fpsLabel = new QLabel("FPS: --");
    m_fpsLabel->setObjectName("perfLabel");
    m_fpsLabel->setFont(QFont("Consolas", 9));
    m_fpsLabel->setMinimumWidth(60);
    perfLayout->addWidget(m_fpsLabel);
    
    m_cpuLabel = new QLabel("CPU: --%");
    m_cpuLabel->setObjectName("perfLabel");
    m_cpuLabel->setFont(QFont("Consolas", 9));
    m_cpuLabel->setMinimumWidth(70);
    perfLayout->addWidget(m_cpuLabel);
    
    m_memLabel = new QLabel("MEM: -- MB");
    m_memLabel->setObjectName("perfLabel");
    m_memLabel->setFont(QFont("Consolas", 9));
    m_memLabel->setMinimumWidth(80);
    perfLayout->addWidget(m_memLabel);
    
    mainLayout->addWidget(perfSection);
}

void FooterWidget::applyStyles()
{
    setStyleSheet(R"(
        FooterWidget {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #16213e, stop:1 #1a1a2e);
            border-top: 1px solid #0f3460;
        }
        
        #runningIndicator {
            color: #f87171;
            padding: 4px 8px;
            background-color: rgba(248, 113, 113, 0.1);
            border-radius: 4px;
        }
        
        #footerSeparator {
            background-color: #2d3748;
            max-width: 1px;
        }
        
        #channelReading {
            background-color: rgba(255, 255, 255, 0.03);
            border: 1px solid #2d3748;
            border-radius: 4px;
        }
        
        #channelReading:hover {
            background-color: rgba(255, 255, 255, 0.06);
            border-color: #4a5568;
        }
        
        #readingTitle {
            color: #6c757d;
            text-transform: uppercase;
        }
        
        #readingValue {
            color: #e2e8f0;
        }
        
        #triggerStatus {
            color: #fbbf24;
        }
        
        #acqCount {
            color: #a0aec0;
        }
        
        #acqProgress {
            background-color: #1e293b;
            border: none;
            border-radius: 4px;
        }
        
        #acqProgress::chunk {
            background-color: #00d9ff;
            border-radius: 4px;
        }
        
        #perfLabel {
            color: #6c757d;
        }
    )");
}

void FooterWidget::setChannels(const QMap<QString, ScopeChannel*> &channels)
{
    m_channels = channels;
}

void FooterWidget::setTriggerStatus(const QString &status)
{
    m_triggerStatusLabel->setText(status);
    
    if (status.contains("Triggered", Qt::CaseInsensitive)) {
        m_triggerStatusLabel->setStyleSheet("color: #4ade80;"); // Green
    } else if (status.contains("Waiting", Qt::CaseInsensitive)) {
        m_triggerStatusLabel->setStyleSheet("color: #fbbf24;"); // Yellow
    } else if (status.contains("Auto", Qt::CaseInsensitive)) {
        m_triggerStatusLabel->setStyleSheet("color: #00d9ff;"); // Cyan
    } else {
        m_triggerStatusLabel->setStyleSheet("color: #a0aec0;"); // Gray
    }
}

void FooterWidget::setAcquisitionProgress(int percent)
{
    m_acqProgress->setValue(qBound(0, percent, 100));
}

void FooterWidget::setAcquisitionCount(int count)
{
    m_acqCountLabel->setText(QString("ACQ: %1").arg(count));
}

void FooterWidget::setFPS(int fps)
{
    m_fpsLabel->setText(QString("FPS: %1").arg(fps));
    
    if (fps >= 30) {
        m_fpsLabel->setStyleSheet("color: #4ade80;"); // Green
    } else if (fps >= 15) {
        m_fpsLabel->setStyleSheet("color: #fbbf24;"); // Yellow
    } else {
        m_fpsLabel->setStyleSheet("color: #f87171;"); // Red
    }
}

void FooterWidget::setRunning(bool running)
{
    m_isRunning = running;
    
    if (running) {
        m_runningIndicator->setText("▶ RUNNING");
        m_runningIndicator->setStyleSheet(
            "color: #4ade80; background-color: rgba(74, 222, 128, 0.15); "
            "padding: 4px 8px; border-radius: 4px;");
    } else {
        m_runningIndicator->setText("⏹ STOPPED");
        m_runningIndicator->setStyleSheet(
            "color: #f87171; background-color: rgba(248, 113, 113, 0.1); "
            "padding: 4px 8px; border-radius: 4px;");
    }
}

void FooterWidget::updateReadings()
{
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        const QString &name = it.key();
        ScopeChannel *channel = it.value();
        
        if (!m_channelReadings.contains(name)) continue;
        
        ChannelReadingWidget &widget = m_channelReadings[name];
        
        if (channel->isEnabled() && !channel->data().isEmpty()) {
            widget.container->setVisible(true);
            
            double vpp = channel->measureVpp();
            double freq = channel->measureFrequency();
            
            widget.vppLabel->setText(formatValue(vpp, "V"));
            widget.freqLabel->setText(formatValue(freq, "Hz"));
        } else {
            widget.container->setVisible(channel->isEnabled());
            widget.vppLabel->setText("---");
            widget.freqLabel->setText("---");
        }
    }
}

void FooterWidget::updatePerformance()
{
    // Get memory usage (simplified - in real app use platform-specific APIs)
    // This is a placeholder implementation
    static int memUsage = 120;
    memUsage = memUsage + (QRandomGenerator::global()->bounded(10) - 5);
    memUsage = qBound(80, memUsage, 200);
    
    m_memLabel->setText(QString("MEM: %1 MB").arg(memUsage));
    
    // CPU usage placeholder
    static int cpuUsage = 15;
    cpuUsage = cpuUsage + (QRandomGenerator::global()->bounded(6) - 3);
    cpuUsage = qBound(5, cpuUsage, 50);
    
    m_cpuLabel->setText(QString("CPU: %1%").arg(cpuUsage));
}

QString FooterWidget::formatValue(double value, const QString &unit, int precision)
{
    if (qIsNaN(value) || qIsInf(value) || value == 0) {
        return "---";
    }
    
    QString prefix;
    double displayValue = value;
    
    if (unit == "V") {
        if (qAbs(value) >= 1.0) {
            prefix = "";
        } else if (qAbs(value) >= 1e-3) {
            displayValue = value * 1e3;
            prefix = "m";
        } else if (qAbs(value) >= 1e-6) {
            displayValue = value * 1e6;
            prefix = "µ";
        } else {
            displayValue = value * 1e9;
            prefix = "n";
        }
    } else if (unit == "Hz") {
        if (qAbs(value) >= 1e9) {
            displayValue = value / 1e9;
            prefix = "G";
        } else if (qAbs(value) >= 1e6) {
            displayValue = value / 1e6;
            prefix = "M";
        } else if (qAbs(value) >= 1e3) {
            displayValue = value / 1e3;
            prefix = "k";
        }
    }
    
    return QString("%1 %2%3")
        .arg(displayValue, 0, 'f', precision)
        .arg(prefix)
        .arg(unit);
}
