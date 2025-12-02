#include "triggerpanel.h"
#include "../core/scopesettings.h"
#include "../communication/deviceconnection.h"
#include "../communication/scpicommands.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>

TriggerPanel::TriggerPanel(ScopeSettings *settings, DeviceConnection *connection, QWidget *parent)
    : QWidget(parent)
    , m_settings(settings)
    , m_connection(connection)
{
    setupUI();
    refresh();
    
    // Connect to settings changes
    connect(m_settings, &ScopeSettings::triggerModeChanged, this, &TriggerPanel::updateUI);
    connect(m_settings, &ScopeSettings::triggerSourceChanged, this, &TriggerPanel::updateUI);
    connect(m_settings, &ScopeSettings::triggerSlopeChanged, this, &TriggerPanel::updateUI);
    connect(m_settings, &ScopeSettings::triggerLevelChanged, this, &TriggerPanel::updateUI);
}

void TriggerPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);
    
    // Mode group
    QGroupBox *modeGroup = new QGroupBox(tr("Trigger Mode"));
    QHBoxLayout *modeLayout = new QHBoxLayout(modeGroup);
    
    m_modeCombo = new QComboBox();
    m_modeCombo->addItem(tr("Auto"), static_cast<int>(ScopeSettings::TriggerMode::Auto));
    m_modeCombo->addItem(tr("Normal"), static_cast<int>(ScopeSettings::TriggerMode::Normal));
    m_modeCombo->addItem(tr("Single"), static_cast<int>(ScopeSettings::TriggerMode::Single));
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TriggerPanel::onModeChanged);
    modeLayout->addWidget(m_modeCombo);
    
    mainLayout->addWidget(modeGroup);
    
    // Type group
    QGroupBox *typeGroup = new QGroupBox(tr("Trigger Type"));
    QFormLayout *typeLayout = new QFormLayout(typeGroup);
    
    m_typeCombo = new QComboBox();
    m_typeCombo->addItem(tr("Edge"));
    m_typeCombo->addItem(tr("Pulse"));
    m_typeCombo->addItem(tr("Slope"));
    m_typeCombo->addItem(tr("Video"));
    m_typeCombo->addItem(tr("Pattern"));
    m_typeCombo->addItem(tr("RS232"));
    m_typeCombo->addItem(tr("I2C"));
    m_typeCombo->addItem(tr("SPI"));
    typeLayout->addRow(tr("Type:"), m_typeCombo);
    
    mainLayout->addWidget(typeGroup);
    
    // Source group
    QGroupBox *sourceGroup = new QGroupBox(tr("Trigger Source"));
    QFormLayout *sourceLayout = new QFormLayout(sourceGroup);
    
    m_sourceCombo = new QComboBox();
    m_sourceCombo->addItem("CH1", "CHAN1");
    m_sourceCombo->addItem("CH2", "CHAN2");
    m_sourceCombo->addItem("CH3", "CHAN3");
    m_sourceCombo->addItem("CH4", "CHAN4");
    m_sourceCombo->addItem("EXT", "EXT");
    m_sourceCombo->addItem("AC Line", "ACL");
    connect(m_sourceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TriggerPanel::onSourceChanged);
    sourceLayout->addRow(tr("Source:"), m_sourceCombo);
    
    m_slopeCombo = new QComboBox();
    m_slopeCombo->addItem(tr("Rising ↑"), static_cast<int>(ScopeSettings::TriggerSlope::Rising));
    m_slopeCombo->addItem(tr("Falling ↓"), static_cast<int>(ScopeSettings::TriggerSlope::Falling));
    m_slopeCombo->addItem(tr("Either ↕"), static_cast<int>(ScopeSettings::TriggerSlope::Either));
    connect(m_slopeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TriggerPanel::onSlopeChanged);
    sourceLayout->addRow(tr("Slope:"), m_slopeCombo);
    
    mainLayout->addWidget(sourceGroup);
    
    // Level group
    QGroupBox *levelGroup = new QGroupBox(tr("Trigger Level"));
    QVBoxLayout *levelLayout = new QVBoxLayout(levelGroup);
    
    QHBoxLayout *levelSpinLayout = new QHBoxLayout();
    m_levelSpin = new QDoubleSpinBox();
    m_levelSpin->setRange(-100.0, 100.0);
    m_levelSpin->setSingleStep(0.1);
    m_levelSpin->setDecimals(3);
    m_levelSpin->setSuffix(" V");
    connect(m_levelSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &TriggerPanel::onLevelChanged);
    levelSpinLayout->addWidget(m_levelSpin);
    
    m_50PercentButton = new QPushButton(tr("50%"));
    m_50PercentButton->setToolTip(tr("Set trigger level to 50% of signal amplitude"));
    connect(m_50PercentButton, &QPushButton::clicked, this, &TriggerPanel::on50PercentClicked);
    levelSpinLayout->addWidget(m_50PercentButton);
    
    levelLayout->addLayout(levelSpinLayout);
    
    m_levelSlider = new QSlider(Qt::Horizontal);
    m_levelSlider->setRange(-1000, 1000);
    connect(m_levelSlider, &QSlider::valueChanged, this, [this](int value) {
        double level = value / 100.0;
        m_levelSpin->blockSignals(true);
        m_levelSpin->setValue(level);
        m_levelSpin->blockSignals(false);
        onLevelChanged(level);
    });
    levelLayout->addWidget(m_levelSlider);
    
    mainLayout->addWidget(levelGroup);
    
    // Coupling group
    QGroupBox *couplingGroup = new QGroupBox(tr("Trigger Coupling"));
    QFormLayout *couplingLayout = new QFormLayout(couplingGroup);
    
    m_couplingCombo = new QComboBox();
    m_couplingCombo->addItem("DC");
    m_couplingCombo->addItem("AC");
    m_couplingCombo->addItem("LF Reject");
    m_couplingCombo->addItem("HF Reject");
    couplingLayout->addRow(tr("Coupling:"), m_couplingCombo);
    
    m_noiseRejectCombo = new QComboBox();
    m_noiseRejectCombo->addItem(tr("Off"));
    m_noiseRejectCombo->addItem(tr("On"));
    couplingLayout->addRow(tr("Noise Reject:"), m_noiseRejectCombo);
    
    mainLayout->addWidget(couplingGroup);
    
    // Holdoff
    QGroupBox *holdoffGroup = new QGroupBox(tr("Holdoff"));
    QFormLayout *holdoffLayout = new QFormLayout(holdoffGroup);
    
    m_holdoffSpin = new QDoubleSpinBox();
    m_holdoffSpin->setRange(100e-9, 10.0);
    m_holdoffSpin->setDecimals(9);
    m_holdoffSpin->setValue(100e-9);
    m_holdoffSpin->setSuffix(" s");
    holdoffLayout->addRow(tr("Time:"), m_holdoffSpin);
    
    mainLayout->addWidget(holdoffGroup);
    
    // Status and actions
    QGroupBox *statusGroup = new QGroupBox(tr("Status"));
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);
    
    m_statusLabel = new QLabel(tr("Waiting..."));
    m_statusLabel->setAlignment(Qt::AlignCenter);
    QFont statusFont = m_statusLabel->font();
    statusFont.setPointSize(12);
    statusFont.setBold(true);
    m_statusLabel->setFont(statusFont);
    statusLayout->addWidget(m_statusLabel);
    
    m_forceButton = new QPushButton(tr("Force Trigger"));
    connect(m_forceButton, &QPushButton::clicked, this, &TriggerPanel::onForceClicked);
    statusLayout->addWidget(m_forceButton);
    
    mainLayout->addWidget(statusGroup);
    
    mainLayout->addStretch();
}

void TriggerPanel::refresh()
{
    updateUI();
}

void TriggerPanel::updateUI()
{
    m_modeCombo->blockSignals(true);
    int modeIndex = m_modeCombo->findData(static_cast<int>(m_settings->triggerMode()));
    if (modeIndex >= 0) m_modeCombo->setCurrentIndex(modeIndex);
    m_modeCombo->blockSignals(false);
    
    m_sourceCombo->blockSignals(true);
    // Find matching source
    for (int i = 0; i < m_sourceCombo->count(); ++i) {
        if (m_sourceCombo->itemData(i).toString() == m_settings->triggerSource()) {
            m_sourceCombo->setCurrentIndex(i);
            break;
        }
    }
    m_sourceCombo->blockSignals(false);
    
    m_slopeCombo->blockSignals(true);
    int slopeIndex = m_slopeCombo->findData(static_cast<int>(m_settings->triggerSlope()));
    if (slopeIndex >= 0) m_slopeCombo->setCurrentIndex(slopeIndex);
    m_slopeCombo->blockSignals(false);
    
    m_levelSpin->blockSignals(true);
    m_levelSpin->setValue(m_settings->triggerLevel());
    m_levelSpin->blockSignals(false);
    
    m_levelSlider->blockSignals(true);
    m_levelSlider->setValue(static_cast<int>(m_settings->triggerLevel() * 100));
    m_levelSlider->blockSignals(false);
}

void TriggerPanel::syncFromDevice()
{
    if (!m_connection || !m_connection->isConnected()) return;
    
    // Query trigger mode
    QString response = m_connection->sendQuery(SCPI::TRIGGER_MODE_QUERY);
    m_settings->setTriggerMode(ScopeSettings::stringToTriggerMode(response));
    
    // Query trigger source
    response = m_connection->sendQuery(SCPI::TRIGGER_EDGE_SOURCE_QUERY);
    m_settings->setTriggerSource(response.trimmed());
    
    // Query trigger slope
    response = m_connection->sendQuery(SCPI::TRIGGER_EDGE_SLOPE_QUERY);
    m_settings->setTriggerSlope(ScopeSettings::stringToTriggerSlope(response));
    
    // Query trigger level
    response = m_connection->sendQuery(SCPI::TRIGGER_EDGE_LEVEL_QUERY);
    bool ok;
    double level = response.toDouble(&ok);
    if (ok) m_settings->setTriggerLevel(level);
    
    updateUI();
}

void TriggerPanel::onModeChanged(int index)
{
    auto mode = static_cast<ScopeSettings::TriggerMode>(m_modeCombo->itemData(index).toInt());
    m_settings->setTriggerMode(mode);
    
    if (m_connection && m_connection->isConnected()) {
        QString modeStr = ScopeSettings::triggerModeToString(mode);
        m_connection->sendCommand(SCPI::triggerMode(modeStr.toUpper()));
    }
    
    emit triggerSettingsChanged();
}

void TriggerPanel::onSourceChanged(int index)
{
    QString source = m_sourceCombo->itemData(index).toString();
    m_settings->setTriggerSource(source);
    
    if (m_connection && m_connection->isConnected()) {
        m_connection->sendCommand(SCPI::triggerEdgeSource(source));
    }
    
    emit triggerSettingsChanged();
}

void TriggerPanel::onSlopeChanged(int index)
{
    auto slope = static_cast<ScopeSettings::TriggerSlope>(m_slopeCombo->itemData(index).toInt());
    m_settings->setTriggerSlope(slope);
    
    if (m_connection && m_connection->isConnected()) {
        QString slopeStr;
        switch (slope) {
            case ScopeSettings::TriggerSlope::Rising: slopeStr = "POSitive"; break;
            case ScopeSettings::TriggerSlope::Falling: slopeStr = "NEGative"; break;
            case ScopeSettings::TriggerSlope::Either: slopeStr = "RFAL"; break;
        }
        m_connection->sendCommand(SCPI::triggerEdgeSlope(slopeStr));
    }
    
    emit triggerSettingsChanged();
}

void TriggerPanel::onLevelChanged(double level)
{
    m_settings->setTriggerLevel(level);
    
    m_levelSlider->blockSignals(true);
    m_levelSlider->setValue(static_cast<int>(level * 100));
    m_levelSlider->blockSignals(false);
    
    if (m_connection && m_connection->isConnected()) {
        m_connection->sendCommand(SCPI::triggerEdgeLevel(level));
    }
    
    emit triggerSettingsChanged();
}

void TriggerPanel::onForceClicked()
{
    if (m_connection && m_connection->isConnected()) {
        m_connection->sendCommand(SCPI::FORCE);
    }
}

void TriggerPanel::on50PercentClicked()
{
    if (m_connection && m_connection->isConnected()) {
        // Query the 50% level from device
        // For now, just set to 0
        m_levelSpin->setValue(0.0);
    }
}

void TriggerPanel::sendToDevice()
{
    if (!m_connection || !m_connection->isConnected()) return;
    
    QString modeStr = ScopeSettings::triggerModeToString(m_settings->triggerMode());
    m_connection->sendCommand(SCPI::triggerMode(modeStr.toUpper()));
    
    m_connection->sendCommand(SCPI::triggerEdgeSource(m_settings->triggerSource()));
    
    QString slopeStr;
    switch (m_settings->triggerSlope()) {
        case ScopeSettings::TriggerSlope::Rising: slopeStr = "POSitive"; break;
        case ScopeSettings::TriggerSlope::Falling: slopeStr = "NEGative"; break;
        case ScopeSettings::TriggerSlope::Either: slopeStr = "RFAL"; break;
    }
    m_connection->sendCommand(SCPI::triggerEdgeSlope(slopeStr));
    
    m_connection->sendCommand(SCPI::triggerEdgeLevel(m_settings->triggerLevel()));
}
