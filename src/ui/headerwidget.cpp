/**
 * @file headerwidget.cpp
 * @brief Implementation of the modern header widget
 */

#include "headerwidget.h"
#include "../communication/deviceconnection.h"
#include "../core/scopesettings.h"

#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>

HeaderWidget::HeaderWidget(DeviceConnection *connection, 
                           ScopeSettings *settings,
                           QWidget *parent)
    : QWidget(parent)
    , m_connection(connection)
    , m_settings(settings)
{
    setupUI();
    applyStyles();
    
    // Setup clock timer
    m_clockTimer = new QTimer(this);
    connect(m_clockTimer, &QTimer::timeout, this, &HeaderWidget::updateDateTime);
    m_clockTimer->start(1000);
    updateDateTime();
}

void HeaderWidget::setupUI()
{
    setFixedHeight(56);
    
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(16, 8, 16, 8);
    mainLayout->setSpacing(24);
    
    // === Brand Section ===
    QWidget *brandSection = new QWidget;
    QHBoxLayout *brandLayout = new QHBoxLayout(brandSection);
    brandLayout->setContentsMargins(0, 0, 0, 0);
    brandLayout->setSpacing(8);
    
    m_logoLabel = new QLabel("◎");
    m_logoLabel->setObjectName("logoLabel");
    m_logoLabel->setFont(QFont("Segoe UI", 24, QFont::Bold));
    brandLayout->addWidget(m_logoLabel);
    
    QWidget *brandText = new QWidget;
    QVBoxLayout *brandTextLayout = new QVBoxLayout(brandText);
    brandTextLayout->setContentsMargins(0, 0, 0, 0);
    brandTextLayout->setSpacing(0);
    
    m_appNameLabel = new QLabel("RIGOL Scope Pro");
    m_appNameLabel->setObjectName("appNameLabel");
    m_appNameLabel->setFont(QFont("Segoe UI", 13, QFont::Bold));
    brandTextLayout->addWidget(m_appNameLabel);
    
    QLabel *versionLabel = new QLabel("v2.0.0");
    versionLabel->setObjectName("versionLabel");
    versionLabel->setFont(QFont("Segoe UI", 9));
    brandTextLayout->addWidget(versionLabel);
    
    brandLayout->addWidget(brandText);
    mainLayout->addWidget(brandSection);
    
    // Separator
    QFrame *sep1 = new QFrame;
    sep1->setFrameShape(QFrame::VLine);
    sep1->setObjectName("headerSeparator");
    mainLayout->addWidget(sep1);
    
    // === Device Info Section ===
    QWidget *deviceSection = new QWidget;
    QHBoxLayout *deviceLayout = new QHBoxLayout(deviceSection);
    deviceLayout->setContentsMargins(0, 0, 0, 0);
    deviceLayout->setSpacing(16);
    
    // Connection status
    QWidget *connWidget = new QWidget;
    QHBoxLayout *connLayout = new QHBoxLayout(connWidget);
    connLayout->setContentsMargins(0, 0, 0, 0);
    connLayout->setSpacing(6);
    
    m_connectionIndicator = new QLabel("●");
    m_connectionIndicator->setObjectName("connectionIndicator");
    m_connectionIndicator->setFont(QFont("Segoe UI", 12));
    connLayout->addWidget(m_connectionIndicator);
    
    m_deviceIdLabel = new QLabel("Not Connected");
    m_deviceIdLabel->setObjectName("deviceIdLabel");
    m_deviceIdLabel->setFont(QFont("Segoe UI Semibold", 10));
    connLayout->addWidget(m_deviceIdLabel);
    
    deviceLayout->addWidget(connWidget);
    
    // Sample rate
    QWidget *srWidget = new QWidget;
    QVBoxLayout *srLayout = new QVBoxLayout(srWidget);
    srLayout->setContentsMargins(0, 0, 0, 0);
    srLayout->setSpacing(0);
    
    QLabel *srTitle = new QLabel("SAMPLE RATE");
    srTitle->setObjectName("infoTitle");
    srTitle->setFont(QFont("Segoe UI", 8));
    srLayout->addWidget(srTitle);
    
    m_sampleRateLabel = new QLabel("---");
    m_sampleRateLabel->setObjectName("infoValue");
    m_sampleRateLabel->setFont(QFont("Segoe UI Semibold", 11));
    srLayout->addWidget(m_sampleRateLabel);
    
    deviceLayout->addWidget(srWidget);
    
    // Memory depth
    QWidget *memWidget = new QWidget;
    QVBoxLayout *memLayout = new QVBoxLayout(memWidget);
    memLayout->setContentsMargins(0, 0, 0, 0);
    memLayout->setSpacing(0);
    
    QLabel *memTitle = new QLabel("MEMORY");
    memTitle->setObjectName("infoTitle");
    memTitle->setFont(QFont("Segoe UI", 8));
    memLayout->addWidget(memTitle);
    
    m_memoryDepthLabel = new QLabel("---");
    m_memoryDepthLabel->setObjectName("infoValue");
    m_memoryDepthLabel->setFont(QFont("Segoe UI Semibold", 11));
    memLayout->addWidget(m_memoryDepthLabel);
    
    deviceLayout->addWidget(memWidget);
    
    mainLayout->addWidget(deviceSection);
    
    // Spacer
    mainLayout->addStretch();
    
    // === Global Controls Section ===
    QWidget *controlSection = new QWidget;
    QHBoxLayout *controlLayout = new QHBoxLayout(controlSection);
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->setSpacing(12);
    
    // Run mode
    QWidget *runWidget = new QWidget;
    QVBoxLayout *runLayout = new QVBoxLayout(runWidget);
    runLayout->setContentsMargins(0, 0, 0, 0);
    runLayout->setSpacing(2);
    
    QLabel *runTitle = new QLabel("RUN MODE");
    runTitle->setObjectName("infoTitle");
    runTitle->setFont(QFont("Segoe UI", 8));
    runLayout->addWidget(runTitle);
    
    m_runModeCombo = new QComboBox;
    m_runModeCombo->addItem("Auto");
    m_runModeCombo->addItem("Normal");
    m_runModeCombo->addItem("Single");
    m_runModeCombo->setObjectName("headerCombo");
    m_runModeCombo->setMinimumWidth(90);
    connect(m_runModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &HeaderWidget::onRunModeChanged);
    runLayout->addWidget(m_runModeCombo);
    
    controlLayout->addWidget(runWidget);
    
    // Timebase
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout(tbWidget);
    tbLayout->setContentsMargins(0, 0, 0, 0);
    tbLayout->setSpacing(2);
    
    QLabel *tbTitle = new QLabel("TIME/DIV");
    tbTitle->setObjectName("infoTitle");
    tbTitle->setFont(QFont("Segoe UI", 8));
    tbLayout->addWidget(tbTitle);
    
    m_timebaseCombo = new QComboBox;
    QStringList timebaseOptions = {
        "1 ns", "2 ns", "5 ns", "10 ns", "20 ns", "50 ns",
        "100 ns", "200 ns", "500 ns",
        "1 µs", "2 µs", "5 µs", "10 µs", "20 µs", "50 µs",
        "100 µs", "200 µs", "500 µs",
        "1 ms", "2 ms", "5 ms", "10 ms", "20 ms", "50 ms",
        "100 ms", "200 ms", "500 ms",
        "1 s", "2 s", "5 s"
    };
    m_timebaseCombo->addItems(timebaseOptions);
    m_timebaseCombo->setCurrentIndex(18); // Default 1ms
    m_timebaseCombo->setObjectName("headerCombo");
    m_timebaseCombo->setMinimumWidth(90);
    connect(m_timebaseCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &HeaderWidget::onTimebaseChanged);
    tbLayout->addWidget(m_timebaseCombo);
    
    controlLayout->addWidget(tbWidget);
    
    mainLayout->addWidget(controlSection);
    
    // Separator
    QFrame *sep2 = new QFrame;
    sep2->setFrameShape(QFrame::VLine);
    sep2->setObjectName("headerSeparator");
    mainLayout->addWidget(sep2);
    
    // === DateTime & Settings Section ===
    QWidget *rightSection = new QWidget;
    QHBoxLayout *rightLayout = new QHBoxLayout(rightSection);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(12);
    
    m_dateTimeLabel = new QLabel;
    m_dateTimeLabel->setObjectName("dateTimeLabel");
    m_dateTimeLabel->setFont(QFont("Segoe UI", 10));
    rightLayout->addWidget(m_dateTimeLabel);
    
    m_settingsBtn = new QPushButton("⚙");
    m_settingsBtn->setObjectName("iconButton");
    m_settingsBtn->setFixedSize(32, 32);
    m_settingsBtn->setToolTip("Settings");
    connect(m_settingsBtn, &QPushButton::clicked, this, &HeaderWidget::settingsRequested);
    rightLayout->addWidget(m_settingsBtn);
    
    m_helpBtn = new QPushButton("?");
    m_helpBtn->setObjectName("iconButton");
    m_helpBtn->setFixedSize(32, 32);
    m_helpBtn->setToolTip("Help");
    connect(m_helpBtn, &QPushButton::clicked, this, &HeaderWidget::helpRequested);
    rightLayout->addWidget(m_helpBtn);
    
    mainLayout->addWidget(rightSection);
}

void HeaderWidget::applyStyles()
{
    setStyleSheet(R"(
        HeaderWidget {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #1a1a2e, stop:1 #16213e);
            border-bottom: 1px solid #0f3460;
        }
        
        #logoLabel {
            color: #00d9ff;
        }
        
        #appNameLabel {
            color: #ffffff;
        }
        
        #versionLabel {
            color: #6c757d;
        }
        
        #headerSeparator {
            background-color: #2d3748;
            max-width: 1px;
        }
        
        #connectionIndicator {
            color: #ff6b6b;
        }
        
        #deviceIdLabel {
            color: #a0aec0;
        }
        
        #infoTitle {
            color: #6c757d;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        
        #infoValue {
            color: #00d9ff;
        }
        
        #headerCombo {
            background-color: #1e293b;
            border: 1px solid #334155;
            border-radius: 4px;
            padding: 4px 8px;
            color: #e2e8f0;
            min-height: 24px;
        }
        
        #headerCombo:hover {
            border-color: #00d9ff;
        }
        
        #headerCombo::drop-down {
            border: none;
            width: 20px;
        }
        
        #headerCombo QAbstractItemView {
            background-color: #1e293b;
            border: 1px solid #334155;
            selection-background-color: #0f3460;
        }
        
        #dateTimeLabel {
            color: #a0aec0;
        }
        
        #iconButton {
            background-color: transparent;
            border: 1px solid #334155;
            border-radius: 4px;
            color: #a0aec0;
            font-size: 14px;
        }
        
        #iconButton:hover {
            background-color: #1e293b;
            border-color: #00d9ff;
            color: #00d9ff;
        }
    )");
    
    setConnectionState(false);
}

void HeaderWidget::setDeviceId(const QString &id)
{
    if (id.isEmpty()) {
        m_deviceIdLabel->setText("Not Connected");
    } else {
        // Truncate long device IDs
        QString displayId = id;
        if (displayId.length() > 30) {
            displayId = displayId.left(27) + "...";
        }
        m_deviceIdLabel->setText(displayId);
    }
}

void HeaderWidget::setConnectionState(bool connected)
{
    if (connected) {
        m_connectionIndicator->setStyleSheet("color: #4ade80;"); // Green
        m_connectionIndicator->setText("●");
    } else {
        m_connectionIndicator->setStyleSheet("color: #f87171;"); // Red
        m_connectionIndicator->setText("●");
        m_deviceIdLabel->setText("Not Connected");
    }
}

void HeaderWidget::setSampleRate(const QString &rate)
{
    m_sampleRateLabel->setText(rate);
}

void HeaderWidget::setMemoryDepth(const QString &depth)
{
    m_memoryDepthLabel->setText(depth);
}

void HeaderWidget::setAcquisitionMode(const QString &mode)
{
    int index = m_runModeCombo->findText(mode, Qt::MatchFixedString);
    if (index >= 0) {
        m_runModeCombo->blockSignals(true);
        m_runModeCombo->setCurrentIndex(index);
        m_runModeCombo->blockSignals(false);
    }
}

void HeaderWidget::updateDateTime()
{
    QDateTime now = QDateTime::currentDateTime();
    m_dateTimeLabel->setText(now.toString("yyyy-MM-dd  HH:mm:ss"));
}

void HeaderWidget::onRunModeChanged(int index)
{
    emit runModeChanged(m_runModeCombo->itemText(index));
}

void HeaderWidget::onTimebaseChanged(int index)
{
    // Parse timebase string to double value
    QString text = m_timebaseCombo->itemText(index);
    double value = 0;
    
    if (text.contains("ns")) {
        value = text.remove("ns").trimmed().toDouble() * 1e-9;
    } else if (text.contains("µs")) {
        value = text.remove("µs").trimmed().toDouble() * 1e-6;
    } else if (text.contains("ms")) {
        value = text.remove("ms").trimmed().toDouble() * 1e-3;
    } else if (text.contains("s")) {
        value = text.remove("s").trimmed().toDouble();
    }
    
    if (value > 0) {
        emit timebaseChanged(value);
    }
}
