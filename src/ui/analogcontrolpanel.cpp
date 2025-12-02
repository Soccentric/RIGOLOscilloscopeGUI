/**
 * @file analogcontrolpanel.cpp
 * @brief Implementation of the modern analog control panel
 */

#include "analogcontrolpanel.h"
#include "../core/scopechannel.h"
#include "../core/scopesettings.h"
#include "../communication/deviceconnection.h"
#include "../communication/scpicommands.h"

#include <QGridLayout>
#include <QFrame>

AnalogControlPanel::AnalogControlPanel(const QMap<QString, ScopeChannel*> &channels,
                                       DeviceConnection *connection,
                                       ScopeSettings *settings,
                                       QWidget *parent)
    : QWidget(parent)
    , m_channels(channels)
    , m_connection(connection)
    , m_settings(settings)
    , m_currentChannel("CH1")
{
    // Initialize scale values (1-2-5 sequence)
    m_scaleValues = {
        0.001, 0.002, 0.005,  // 1mV, 2mV, 5mV
        0.01, 0.02, 0.05,     // 10mV, 20mV, 50mV
        0.1, 0.2, 0.5,        // 100mV, 200mV, 500mV
        1.0, 2.0, 5.0,        // 1V, 2V, 5V
        10.0                   // 10V
    };
    
    setupUI();
    applyStyles();
    updateChannelControls();
}

void AnalogControlPanel::setupUI()
{
    setMinimumWidth(220);
    setMaximumWidth(280);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(16);
    
    // === Title ===
    QLabel *titleLabel = new QLabel("ANALOG CONTROLS");
    titleLabel->setObjectName("panelTitle");
    titleLabel->setFont(QFont("Segoe UI", 10, QFont::Bold));
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // === Channel Selection Group ===
    QGroupBox *channelGroup = new QGroupBox("Channel");
    channelGroup->setObjectName("controlGroup");
    QVBoxLayout *channelLayout = new QVBoxLayout(channelGroup);
    channelLayout->setSpacing(8);
    
    // Channel selector row
    QHBoxLayout *selectorRow = new QHBoxLayout;
    selectorRow->setSpacing(8);
    
    m_channelColorIndicator = new QLabel;
    m_channelColorIndicator->setFixedSize(16, 16);
    m_channelColorIndicator->setObjectName("colorIndicator");
    selectorRow->addWidget(m_channelColorIndicator);
    
    m_channelCombo = new QComboBox;
    m_channelCombo->setObjectName("channelCombo");
    QStringList channelNames = {"CH1", "CH2", "CH3", "CH4"};
    QStringList channelColors = {"#ffeb3b", "#00e5ff", "#ff4081", "#00e676"};
    for (int i = 0; i < channelNames.size(); ++i) {
        m_channelCombo->addItem(channelNames[i]);
    }
    connect(m_channelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AnalogControlPanel::onChannelSelected);
    selectorRow->addWidget(m_channelCombo, 1);
    
    m_enableBtn = new QPushButton("ON");
    m_enableBtn->setObjectName("enableBtn");
    m_enableBtn->setCheckable(true);
    m_enableBtn->setChecked(true);
    m_enableBtn->setFixedWidth(50);
    connect(m_enableBtn, &QPushButton::toggled, this, &AnalogControlPanel::onChannelEnableToggled);
    selectorRow->addWidget(m_enableBtn);
    
    channelLayout->addLayout(selectorRow);
    
    // Quick channel buttons
    QHBoxLayout *quickChannels = new QHBoxLayout;
    quickChannels->setSpacing(4);
    for (int i = 0; i < 4; ++i) {
        QPushButton *btn = new QPushButton(QString::number(i + 1));
        btn->setObjectName("quickChannelBtn");
        btn->setFixedSize(36, 28);
        btn->setStyleSheet(QString("background-color: %1; color: #000;").arg(channelColors[i]));
        connect(btn, &QPushButton::clicked, this, [this, i]() {
            m_channelCombo->setCurrentIndex(i);
        });
        quickChannels->addWidget(btn);
    }
    channelLayout->addLayout(quickChannels);
    
    mainLayout->addWidget(channelGroup);
    
    // === Vertical Scale Group ===
    QGroupBox *scaleGroup = new QGroupBox("Vertical Scale");
    scaleGroup->setObjectName("controlGroup");
    QVBoxLayout *scaleLayout = new QVBoxLayout(scaleGroup);
    scaleLayout->setSpacing(8);
    scaleLayout->setAlignment(Qt::AlignHCenter);
    
    m_scaleValueLabel = new QLabel("1.00 V/div");
    m_scaleValueLabel->setObjectName("dialValueLabel");
    m_scaleValueLabel->setFont(QFont("Consolas", 14, QFont::Bold));
    m_scaleValueLabel->setAlignment(Qt::AlignCenter);
    scaleLayout->addWidget(m_scaleValueLabel);
    
    m_scaleDial = new QDial;
    m_scaleDial->setObjectName("scaleDial");
    m_scaleDial->setRange(0, m_scaleValues.size() - 1);
    m_scaleDial->setValue(9); // Default 1V
    m_scaleDial->setFixedSize(100, 100);
    m_scaleDial->setNotchesVisible(true);
    m_scaleDial->setWrapping(false);
    connect(m_scaleDial, &QDial::valueChanged, this, &AnalogControlPanel::onScaleDialChanged);
    scaleLayout->addWidget(m_scaleDial, 0, Qt::AlignHCenter);
    
    // Scale indicators
    QHBoxLayout *scaleIndicators = new QHBoxLayout;
    QLabel *minScale = new QLabel("1mV");
    minScale->setObjectName("scaleIndicator");
    QLabel *maxScale = new QLabel("10V");
    maxScale->setObjectName("scaleIndicator");
    scaleIndicators->addWidget(minScale);
    scaleIndicators->addStretch();
    scaleIndicators->addWidget(maxScale);
    scaleLayout->addLayout(scaleIndicators);
    
    mainLayout->addWidget(scaleGroup);
    
    // === Vertical Offset Group ===
    QGroupBox *offsetGroup = new QGroupBox("Vertical Offset");
    offsetGroup->setObjectName("controlGroup");
    QVBoxLayout *offsetLayout = new QVBoxLayout(offsetGroup);
    offsetLayout->setSpacing(8);
    
    m_offsetValueLabel = new QLabel("0.00 V");
    m_offsetValueLabel->setObjectName("dialValueLabel");
    m_offsetValueLabel->setFont(QFont("Consolas", 12, QFont::Bold));
    m_offsetValueLabel->setAlignment(Qt::AlignCenter);
    offsetLayout->addWidget(m_offsetValueLabel);
    
    m_offsetSlider = new QSlider(Qt::Horizontal);
    m_offsetSlider->setObjectName("offsetSlider");
    m_offsetSlider->setRange(-1000, 1000);
    m_offsetSlider->setValue(0);
    m_offsetSlider->setTickPosition(QSlider::TicksBelow);
    m_offsetSlider->setTickInterval(200);
    connect(m_offsetSlider, &QSlider::valueChanged, this, &AnalogControlPanel::onOffsetSliderChanged);
    offsetLayout->addWidget(m_offsetSlider);
    
    // Center button
    m_centerBtn = new QPushButton("Center");
    m_centerBtn->setObjectName("actionBtn");
    connect(m_centerBtn, &QPushButton::clicked, this, [this]() {
        m_offsetSlider->setValue(0);
    });
    offsetLayout->addWidget(m_centerBtn);
    
    mainLayout->addWidget(offsetGroup);
    
    // === Coupling & Probe Group ===
    QGroupBox *couplingGroup = new QGroupBox("Input Settings");
    couplingGroup->setObjectName("controlGroup");
    QGridLayout *couplingLayout = new QGridLayout(couplingGroup);
    couplingLayout->setSpacing(8);
    
    QLabel *couplingLabel = new QLabel("Coupling:");
    couplingLabel->setObjectName("settingLabel");
    couplingLayout->addWidget(couplingLabel, 0, 0);
    
    m_couplingCombo = new QComboBox;
    m_couplingCombo->setObjectName("settingCombo");
    m_couplingCombo->addItem("DC");
    m_couplingCombo->addItem("AC");
    m_couplingCombo->addItem("GND");
    connect(m_couplingCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AnalogControlPanel::onCouplingChanged);
    couplingLayout->addWidget(m_couplingCombo, 0, 1);
    
    QLabel *probeLabel = new QLabel("Probe:");
    probeLabel->setObjectName("settingLabel");
    couplingLayout->addWidget(probeLabel, 1, 0);
    
    m_probeCombo = new QComboBox;
    m_probeCombo->setObjectName("settingCombo");
    m_probeCombo->addItem("1X");
    m_probeCombo->addItem("10X");
    m_probeCombo->addItem("100X");
    m_probeCombo->addItem("1000X");
    m_probeCombo->setCurrentIndex(1); // Default 10X
    couplingLayout->addWidget(m_probeCombo, 1, 1);
    
    mainLayout->addWidget(couplingGroup);
    
    // === Trigger Level Group ===
    QGroupBox *triggerGroup = new QGroupBox("Trigger Level");
    triggerGroup->setObjectName("controlGroup");
    QVBoxLayout *triggerLayout = new QVBoxLayout(triggerGroup);
    triggerLayout->setSpacing(8);
    triggerLayout->setAlignment(Qt::AlignHCenter);
    
    m_triggerValueLabel = new QLabel("0.00 V");
    m_triggerValueLabel->setObjectName("dialValueLabel");
    m_triggerValueLabel->setFont(QFont("Consolas", 12, QFont::Bold));
    m_triggerValueLabel->setAlignment(Qt::AlignCenter);
    triggerLayout->addWidget(m_triggerValueLabel);
    
    m_triggerDial = new QDial;
    m_triggerDial->setObjectName("triggerDial");
    m_triggerDial->setRange(-100, 100);
    m_triggerDial->setValue(0);
    m_triggerDial->setFixedSize(80, 80);
    m_triggerDial->setNotchesVisible(true);
    connect(m_triggerDial, &QDial::valueChanged, this, &AnalogControlPanel::onTriggerDialChanged);
    triggerLayout->addWidget(m_triggerDial, 0, Qt::AlignHCenter);
    
    mainLayout->addWidget(triggerGroup);
    
    // === Quick Actions ===
    QHBoxLayout *actionsLayout = new QHBoxLayout;
    actionsLayout->setSpacing(8);
    
    m_autoScaleBtn = new QPushButton("Auto");
    m_autoScaleBtn->setObjectName("actionBtn");
    actionsLayout->addWidget(m_autoScaleBtn);
    
    m_invertBtn = new QPushButton("Invert");
    m_invertBtn->setObjectName("actionBtn");
    m_invertBtn->setCheckable(true);
    actionsLayout->addWidget(m_invertBtn);
    
    mainLayout->addLayout(actionsLayout);
    
    // Spacer
    mainLayout->addStretch();
}

void AnalogControlPanel::applyStyles()
{
    setStyleSheet(R"(
        AnalogControlPanel {
            background-color: #0d1117;
            border-right: 1px solid #21262d;
        }
        
        #panelTitle {
            color: #58a6ff;
            padding: 8px;
            background-color: rgba(88, 166, 255, 0.1);
            border-radius: 4px;
        }
        
        #controlGroup {
            background-color: #161b22;
            border: 1px solid #21262d;
            border-radius: 8px;
            padding: 12px;
            margin-top: 8px;
        }
        
        #controlGroup::title {
            color: #8b949e;
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 4px;
        }
        
        #colorIndicator {
            border-radius: 8px;
            border: 2px solid #30363d;
        }
        
        #channelCombo {
            background-color: #21262d;
            border: 1px solid #30363d;
            border-radius: 4px;
            padding: 6px 10px;
            color: #c9d1d9;
            font-weight: bold;
        }
        
        #channelCombo:hover {
            border-color: #58a6ff;
        }
        
        #channelCombo::drop-down {
            border: none;
            width: 20px;
        }
        
        #enableBtn {
            background-color: #238636;
            border: none;
            border-radius: 4px;
            color: white;
            font-weight: bold;
            padding: 6px;
        }
        
        #enableBtn:checked {
            background-color: #238636;
        }
        
        #enableBtn:!checked {
            background-color: #21262d;
            color: #8b949e;
        }
        
        #quickChannelBtn {
            border: 1px solid #30363d;
            border-radius: 4px;
            font-weight: bold;
        }
        
        #quickChannelBtn:hover {
            border-width: 2px;
        }
        
        #dialValueLabel {
            color: #00d9ff;
            padding: 8px;
            background-color: rgba(0, 217, 255, 0.1);
            border-radius: 4px;
        }
        
        QDial {
            background-color: #21262d;
        }
        
        #scaleDial, #triggerDial {
            background: qradialgradient(cx:0.5, cy:0.5, radius:0.5,
                fx:0.5, fy:0.3,
                stop:0 #2d333b, stop:0.7 #21262d, stop:1 #161b22);
        }
        
        #scaleIndicator {
            color: #6e7681;
            font-size: 10px;
        }
        
        #offsetSlider {
            height: 24px;
        }
        
        #offsetSlider::groove:horizontal {
            height: 6px;
            background: #21262d;
            border-radius: 3px;
        }
        
        #offsetSlider::handle:horizontal {
            background: #58a6ff;
            width: 16px;
            height: 16px;
            margin: -6px 0;
            border-radius: 8px;
        }
        
        #offsetSlider::handle:horizontal:hover {
            background: #79c0ff;
        }
        
        #offsetSlider::sub-page:horizontal {
            background: #388bfd;
            border-radius: 3px;
        }
        
        #settingLabel {
            color: #8b949e;
        }
        
        #settingCombo {
            background-color: #21262d;
            border: 1px solid #30363d;
            border-radius: 4px;
            padding: 4px 8px;
            color: #c9d1d9;
        }
        
        #settingCombo:hover {
            border-color: #58a6ff;
        }
        
        #actionBtn {
            background-color: #21262d;
            border: 1px solid #30363d;
            border-radius: 4px;
            color: #c9d1d9;
            padding: 8px 16px;
            font-weight: 500;
        }
        
        #actionBtn:hover {
            background-color: #30363d;
            border-color: #8b949e;
        }
        
        #actionBtn:checked {
            background-color: #388bfd;
            border-color: #58a6ff;
            color: white;
        }
    )");
}

void AnalogControlPanel::refresh()
{
    updateChannelControls();
}

QString AnalogControlPanel::selectedChannel() const
{
    return m_currentChannel;
}

void AnalogControlPanel::onChannelSelected(int index)
{
    QStringList channels = {"CH1", "CH2", "CH3", "CH4"};
    QStringList colors = {"#ffeb3b", "#00e5ff", "#ff4081", "#00e676"};
    
    if (index >= 0 && index < channels.size()) {
        m_currentChannel = channels[index];
        m_channelColorIndicator->setStyleSheet(
            QString("background-color: %1; border-radius: 8px;").arg(colors[index]));
        
        updateChannelControls();
        emit channelChanged(m_currentChannel);
    }
}

void AnalogControlPanel::onChannelEnableToggled(bool enabled)
{
    if (m_channels.contains(m_currentChannel)) {
        m_channels[m_currentChannel]->setEnabled(enabled);
        m_enableBtn->setText(enabled ? "ON" : "OFF");
        
        if (m_connection && m_connection->isConnected()) {
            m_connection->sendCommand(SCPI::channelDisplay(m_currentChannel, enabled));
        }
    }
}

void AnalogControlPanel::onScaleDialChanged(int value)
{
    if (value >= 0 && value < m_scaleValues.size()) {
        double scale = m_scaleValues[value];
        
        QString text;
        if (scale < 1.0) {
            text = QString("%1 mV/div").arg(scale * 1000, 0, 'f', 0);
        } else {
            text = QString("%1 V/div").arg(scale, 0, 'f', scale < 10 ? 1 : 0);
        }
        m_scaleValueLabel->setText(text);
        
        if (m_channels.contains(m_currentChannel)) {
            m_channels[m_currentChannel]->setScale(scale);
            
            if (m_connection && m_connection->isConnected()) {
                m_connection->sendCommand(SCPI::channelScale(m_currentChannel, scale));
            }
            
            emit scaleChanged(m_currentChannel, scale);
        }
    }
}

void AnalogControlPanel::onOffsetSliderChanged(int value)
{
    double offset = value / 100.0;
    m_offsetValueLabel->setText(QString("%1 V").arg(offset, 0, 'f', 2));
    
    if (m_channels.contains(m_currentChannel)) {
        m_channels[m_currentChannel]->setOffset(offset);
        
        if (m_connection && m_connection->isConnected()) {
            m_connection->sendCommand(SCPI::channelOffset(m_currentChannel, offset));
        }
        
        emit offsetChanged(m_currentChannel, offset);
    }
}

void AnalogControlPanel::onCouplingChanged(int index)
{
    if (!m_channels.contains(m_currentChannel)) return;
    
    auto coupling = static_cast<ScopeChannel::Coupling>(index);
    m_channels[m_currentChannel]->setCoupling(coupling);
    
    if (m_connection && m_connection->isConnected()) {
        QString couplingStr = ScopeChannel::couplingToString(coupling);
        m_connection->sendCommand(SCPI::channelCoupling(m_currentChannel, couplingStr));
    }
}

void AnalogControlPanel::onTriggerDialChanged(int value)
{
    double level = value / 10.0; // -10V to +10V range
    m_triggerValueLabel->setText(QString("%1 V").arg(level, 0, 'f', 2));
    
    if (m_settings) {
        m_settings->setTriggerLevel(level);
    }
    
    if (m_connection && m_connection->isConnected()) {
        m_connection->sendCommand(SCPI::triggerEdgeLevel(level));
    }
    
    emit triggerLevelChanged(level);
}

void AnalogControlPanel::updateChannelControls()
{
    if (!m_channels.contains(m_currentChannel)) return;
    
    ScopeChannel *channel = m_channels[m_currentChannel];
    
    // Update enable button
    m_enableBtn->blockSignals(true);
    m_enableBtn->setChecked(channel->isEnabled());
    m_enableBtn->setText(channel->isEnabled() ? "ON" : "OFF");
    m_enableBtn->blockSignals(false);
    
    // Update scale dial
    double scale = channel->scale();
    int scaleIndex = 0;
    for (int i = 0; i < m_scaleValues.size(); ++i) {
        if (qAbs(m_scaleValues[i] - scale) < 0.0001) {
            scaleIndex = i;
            break;
        }
    }
    m_scaleDial->blockSignals(true);
    m_scaleDial->setValue(scaleIndex);
    m_scaleDial->blockSignals(false);
    
    QString scaleText;
    if (scale < 1.0) {
        scaleText = QString("%1 mV/div").arg(scale * 1000, 0, 'f', 0);
    } else {
        scaleText = QString("%1 V/div").arg(scale, 0, 'f', scale < 10 ? 1 : 0);
    }
    m_scaleValueLabel->setText(scaleText);
    
    // Update offset slider
    m_offsetSlider->blockSignals(true);
    m_offsetSlider->setValue(static_cast<int>(channel->offset() * 100));
    m_offsetSlider->blockSignals(false);
    m_offsetValueLabel->setText(QString("%1 V").arg(channel->offset(), 0, 'f', 2));
    
    // Update coupling
    m_couplingCombo->blockSignals(true);
    m_couplingCombo->setCurrentIndex(static_cast<int>(channel->coupling()));
    m_couplingCombo->blockSignals(false);
    
    // Update probe
    m_probeCombo->blockSignals(true);
    m_probeCombo->setCurrentIndex(static_cast<int>(channel->probe()));
    m_probeCombo->blockSignals(false);
    
    // Update color indicator
    QStringList colors = {"#ffeb3b", "#00e5ff", "#ff4081", "#00e676"};
    int channelIndex = m_channelCombo->currentIndex();
    if (channelIndex >= 0 && channelIndex < colors.size()) {
        m_channelColorIndicator->setStyleSheet(
            QString("background-color: %1; border-radius: 8px;").arg(colors[channelIndex]));
    }
}
