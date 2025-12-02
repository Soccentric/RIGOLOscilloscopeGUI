/**
 * @file digitalcontrolpanel.cpp
 * @brief Implementation of the modern digital control panel
 */

#include "digitalcontrolpanel.h"
#include "../communication/deviceconnection.h"
#include "../communication/scpicommands.h"
#include "../core/measurementengine.h"
#include "../core/scopechannel.h"
#include "../core/scopesettings.h"

#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QScrollArea>

DigitalControlPanel::DigitalControlPanel(
    const QMap<QString, ScopeChannel *> &channels, DeviceConnection *connection,
    ScopeSettings *settings, MeasurementEngine *engine, QWidget *parent)
    : QWidget(parent), m_channels(channels), m_connection(connection),
      m_settings(settings), m_engine(engine) {
  setupUI();
  applyStyles();
}

void DigitalControlPanel::setupUI() {
  setMinimumWidth(240);
  setMaximumWidth(320);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  // === Title ===
  QLabel *titleLabel = new QLabel("DIGITAL CONTROLS");
  titleLabel->setObjectName("panelTitle");
  titleLabel->setFont(QFont("Segoe UI", 10, QFont::Bold));
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setContentsMargins(12, 12, 12, 8);
  mainLayout->addWidget(titleLabel);

  // === Tab Widget ===
  m_tabWidget = new QTabWidget;
  m_tabWidget->setObjectName("controlTabs");
  m_tabWidget->setDocumentMode(true);
  mainLayout->addWidget(m_tabWidget);

  createAcquisitionTab();
  createTriggerTab();
  createMeasurementsTab();
  createAnalysisTab();
}

void DigitalControlPanel::createAcquisitionTab() {
  QWidget *acqTab = new QWidget;
  acqTab->setObjectName("tabContent");
  QVBoxLayout *layout = new QVBoxLayout(acqTab);
  layout->setContentsMargins(12, 12, 12, 12);
  layout->setSpacing(12);

  // === Main Acquisition Buttons ===
  QGroupBox *mainGroup = new QGroupBox("Acquisition Control");
  mainGroup->setObjectName("controlGroup");
  QVBoxLayout *mainLayout = new QVBoxLayout(mainGroup);
  mainLayout->setSpacing(8);

  m_runStopBtn = new QPushButton("▶  RUN");
  m_runStopBtn->setObjectName("runStopBtn");
  m_runStopBtn->setMinimumHeight(48);
  m_runStopBtn->setFont(QFont("Segoe UI", 12, QFont::Bold));
  connect(m_runStopBtn, &QPushButton::clicked, this,
          &DigitalControlPanel::onRunStopClicked);
  mainLayout->addWidget(m_runStopBtn);

  QHBoxLayout *acqBtns = new QHBoxLayout;
  acqBtns->setSpacing(8);

  m_singleBtn = new QPushButton("SINGLE");
  m_singleBtn->setObjectName("acqBtn");
  m_singleBtn->setMinimumHeight(36);
  connect(m_singleBtn, &QPushButton::clicked, this,
          &DigitalControlPanel::singleClicked);
  acqBtns->addWidget(m_singleBtn);

  m_autoBtn = new QPushButton("AUTO");
  m_autoBtn->setObjectName("acqBtn");
  m_autoBtn->setMinimumHeight(36);
  connect(m_autoBtn, &QPushButton::clicked, this,
          &DigitalControlPanel::autoScaleClicked);
  acqBtns->addWidget(m_autoBtn);

  mainLayout->addLayout(acqBtns);

  m_clearBtn = new QPushButton("CLEAR");
  m_clearBtn->setObjectName("clearBtn");
  connect(m_clearBtn, &QPushButton::clicked, this,
          &DigitalControlPanel::clearClicked);
  mainLayout->addWidget(m_clearBtn);

  layout->addWidget(mainGroup);

  // === Acquisition Status ===
  QGroupBox *statusGroup = new QGroupBox("Status");
  statusGroup->setObjectName("controlGroup");
  QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);
  statusLayout->setSpacing(8);

  m_acqStatusLabel = new QLabel("Stopped");
  m_acqStatusLabel->setObjectName("statusLabel");
  m_acqStatusLabel->setFont(QFont("Segoe UI Semibold", 11));
  m_acqStatusLabel->setAlignment(Qt::AlignCenter);
  statusLayout->addWidget(m_acqStatusLabel);

  QHBoxLayout *countLayout = new QHBoxLayout;
  QLabel *countLabel = new QLabel("Acquisition Count:");
  countLabel->setObjectName("settingLabel");
  countLayout->addWidget(countLabel);

  m_acqCountSpin = new QSpinBox;
  m_acqCountSpin->setObjectName("settingInput");
  m_acqCountSpin->setRange(0, 999999);
  m_acqCountSpin->setReadOnly(true);
  m_acqCountSpin->setButtonSymbols(QAbstractSpinBox::NoButtons);
  countLayout->addWidget(m_acqCountSpin);

  statusLayout->addLayout(countLayout);

  layout->addWidget(statusGroup);

  layout->addStretch();

  m_tabWidget->addTab(acqTab, "ACQ");
}

void DigitalControlPanel::createTriggerTab() {
  QScrollArea *scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  scrollArea->setFrameShape(QFrame::NoFrame);
  scrollArea->setObjectName("tabScrollArea");

  QWidget *trigTab = new QWidget;
  trigTab->setObjectName("tabContent");
  QVBoxLayout *layout = new QVBoxLayout(trigTab);
  layout->setContentsMargins(12, 12, 12, 12);
  layout->setSpacing(12);

  // === Trigger Mode ===
  QGroupBox *modeGroup = new QGroupBox("Trigger Mode");
  modeGroup->setObjectName("controlGroup");
  QVBoxLayout *modeLayout = new QVBoxLayout(modeGroup);

  m_trigModeCombo = new QComboBox;
  m_trigModeCombo->setObjectName("settingCombo");
  m_trigModeCombo->addItem("Auto");
  m_trigModeCombo->addItem("Normal");
  m_trigModeCombo->addItem("Single");
  connect(m_trigModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &DigitalControlPanel::onTriggerModeChanged);
  modeLayout->addWidget(m_trigModeCombo);

  layout->addWidget(modeGroup);

  // === Trigger Source & Type ===
  QGroupBox *sourceGroup = new QGroupBox("Source & Type");
  sourceGroup->setObjectName("controlGroup");
  QGridLayout *sourceLayout = new QGridLayout(sourceGroup);
  sourceLayout->setSpacing(8);

  QLabel *typeLabel = new QLabel("Type:");
  typeLabel->setObjectName("settingLabel");
  sourceLayout->addWidget(typeLabel, 0, 0);

  m_trigTypeCombo = new QComboBox;
  m_trigTypeCombo->setObjectName("settingCombo");
  m_trigTypeCombo->addItem("Edge");
  m_trigTypeCombo->addItem("Pulse");
  m_trigTypeCombo->addItem("Slope");
  m_trigTypeCombo->addItem("Video");
  m_trigTypeCombo->addItem("Pattern");
  connect(m_trigTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &DigitalControlPanel::onTriggerTypeChanged);
  sourceLayout->addWidget(m_trigTypeCombo, 0, 1);

  QLabel *srcLabel = new QLabel("Source:");
  srcLabel->setObjectName("settingLabel");
  sourceLayout->addWidget(srcLabel, 1, 0);

  m_trigSourceCombo = new QComboBox;
  m_trigSourceCombo->setObjectName("settingCombo");
  m_trigSourceCombo->addItem("CH1");
  m_trigSourceCombo->addItem("CH2");
  m_trigSourceCombo->addItem("CH3");
  m_trigSourceCombo->addItem("CH4");
  m_trigSourceCombo->addItem("EXT");
  m_trigSourceCombo->addItem("AC Line");
  connect(m_trigSourceCombo,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &DigitalControlPanel::onTriggerSourceChanged);
  sourceLayout->addWidget(m_trigSourceCombo, 1, 1);

  QLabel *slopeLabel = new QLabel("Slope:");
  slopeLabel->setObjectName("settingLabel");
  sourceLayout->addWidget(slopeLabel, 2, 0);

  m_trigSlopeCombo = new QComboBox;
  m_trigSlopeCombo->setObjectName("settingCombo");
  m_trigSlopeCombo->addItem("↑ Rising");
  m_trigSlopeCombo->addItem("↓ Falling");
  m_trigSlopeCombo->addItem("↕ Either");
  connect(m_trigSlopeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &DigitalControlPanel::onTriggerSlopeChanged);
  sourceLayout->addWidget(m_trigSlopeCombo, 2, 1);

  layout->addWidget(sourceGroup);

  // === Trigger Level & Holdoff ===
  QGroupBox *levelGroup = new QGroupBox("Level & Timing");
  levelGroup->setObjectName("controlGroup");
  QGridLayout *levelLayout = new QGridLayout(levelGroup);
  levelLayout->setSpacing(8);

  QLabel *levelLabel = new QLabel("Level:");
  levelLabel->setObjectName("settingLabel");
  levelLayout->addWidget(levelLabel, 0, 0);

  m_trigLevelSpin = new QDoubleSpinBox;
  m_trigLevelSpin->setObjectName("settingInput");
  m_trigLevelSpin->setRange(-100.0, 100.0);
  m_trigLevelSpin->setDecimals(3);
  m_trigLevelSpin->setSuffix(" V");
  m_trigLevelSpin->setSingleStep(0.1);
  levelLayout->addWidget(m_trigLevelSpin, 0, 1);

  m_50PercentBtn = new QPushButton("50%");
  m_50PercentBtn->setObjectName("smallBtn");
  m_50PercentBtn->setToolTip("Set trigger level to 50% of signal");
  levelLayout->addWidget(m_50PercentBtn, 0, 2);

  QLabel *holdoffLabel = new QLabel("Holdoff:");
  holdoffLabel->setObjectName("settingLabel");
  levelLayout->addWidget(holdoffLabel, 1, 0);

  m_trigHoldoffSpin = new QDoubleSpinBox;
  m_trigHoldoffSpin->setObjectName("settingInput");
  m_trigHoldoffSpin->setRange(100e-9, 10.0);
  m_trigHoldoffSpin->setDecimals(9);
  m_trigHoldoffSpin->setSuffix(" s");
  m_trigHoldoffSpin->setValue(100e-9);
  levelLayout->addWidget(m_trigHoldoffSpin, 1, 1, 1, 2);

  layout->addWidget(levelGroup);

  // === Force Trigger ===
  m_forceBtn = new QPushButton("⚡ FORCE TRIGGER");
  m_forceBtn->setObjectName("forceBtn");
  m_forceBtn->setMinimumHeight(36);
  connect(m_forceBtn, &QPushButton::clicked, this,
          &DigitalControlPanel::forceTriggerClicked);
  layout->addWidget(m_forceBtn);

  layout->addStretch();

  scrollArea->setWidget(trigTab);
  m_tabWidget->addTab(scrollArea, "TRIG");
}

void DigitalControlPanel::createMeasurementsTab() {
  QWidget *measTab = new QWidget;
  measTab->setObjectName("tabContent");
  QVBoxLayout *layout = new QVBoxLayout(measTab);
  layout->setContentsMargins(12, 12, 12, 12);
  layout->setSpacing(12);

  // === Add Measurement ===
  QGroupBox *addGroup = new QGroupBox("Add Measurement");
  addGroup->setObjectName("controlGroup");
  QVBoxLayout *addLayout = new QVBoxLayout(addGroup);
  addLayout->setSpacing(8);

  m_measTypeCombo = new QComboBox;
  m_measTypeCombo->setObjectName("settingCombo");
  m_measTypeCombo->addItem("Vpp");
  m_measTypeCombo->addItem("Vmax");
  m_measTypeCombo->addItem("Vmin");
  m_measTypeCombo->addItem("Vavg");
  m_measTypeCombo->addItem("Vrms");
  m_measTypeCombo->addItem("Frequency");
  m_measTypeCombo->addItem("Period");
  m_measTypeCombo->addItem("Rise Time");
  m_measTypeCombo->addItem("Fall Time");
  m_measTypeCombo->addItem("Duty Cycle");
  addLayout->addWidget(m_measTypeCombo);

  m_measChannelCombo = new QComboBox;
  m_measChannelCombo->setObjectName("settingCombo");
  m_measChannelCombo->addItem("CH1");
  m_measChannelCombo->addItem("CH2");
  m_measChannelCombo->addItem("CH3");
  m_measChannelCombo->addItem("CH4");
  addLayout->addWidget(m_measChannelCombo);

  QHBoxLayout *measBtns = new QHBoxLayout;
  measBtns->setSpacing(8);

  m_addMeasBtn = new QPushButton("+ Add");
  m_addMeasBtn->setObjectName("addBtn");
  connect(m_addMeasBtn, &QPushButton::clicked, this,
          &DigitalControlPanel::onAddMeasurement);
  measBtns->addWidget(m_addMeasBtn);

  m_clearMeasBtn = new QPushButton("Clear All");
  m_clearMeasBtn->setObjectName("clearBtn");
  measBtns->addWidget(m_clearMeasBtn);

  addLayout->addLayout(measBtns);

  layout->addWidget(addGroup);

  // === Measurement Table ===
  QGroupBox *tableGroup = new QGroupBox("Active Measurements");
  tableGroup->setObjectName("controlGroup");
  QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);

  m_measTable = new QTableWidget;
  m_measTable->setObjectName("measTable");
  m_measTable->setColumnCount(3);
  m_measTable->setHorizontalHeaderLabels({"Type", "CH", "Value"});
  m_measTable->horizontalHeader()->setStretchLastSection(true);
  m_measTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  m_measTable->verticalHeader()->setVisible(false);
  m_measTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_measTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_measTable->setMaximumHeight(200);
  tableLayout->addWidget(m_measTable);

  m_statsCheck = new QCheckBox("Show Statistics");
  m_statsCheck->setObjectName("settingCheck");
  tableLayout->addWidget(m_statsCheck);

  layout->addWidget(tableGroup);

  layout->addStretch();

  m_tabWidget->addTab(measTab, "MEAS");
}

void DigitalControlPanel::createAnalysisTab() {
  QWidget *analysisTab = new QWidget;
  analysisTab->setObjectName("tabContent");
  QVBoxLayout *layout = new QVBoxLayout(analysisTab);
  layout->setContentsMargins(12, 12, 12, 12);
  layout->setSpacing(12);

  // === Analysis Tools ===
  QGroupBox *toolsGroup = new QGroupBox("Analysis Tools");
  toolsGroup->setObjectName("controlGroup");
  QVBoxLayout *toolsLayout = new QVBoxLayout(toolsGroup);
  toolsLayout->setSpacing(8);

  m_fftBtn = new QPushButton("📊 FFT Spectrum");
  m_fftBtn->setObjectName("toolBtn");
  m_fftBtn->setMinimumHeight(40);
  connect(m_fftBtn, &QPushButton::clicked, this,
          &DigitalControlPanel::fftClicked);
  toolsLayout->addWidget(m_fftBtn);

  m_mathBtn = new QPushButton("➕ Math Channel");
  m_mathBtn->setObjectName("toolBtn");
  m_mathBtn->setMinimumHeight(40);
  toolsLayout->addWidget(m_mathBtn);

  m_cursorBtn = new QPushButton("╋ Cursors");
  m_cursorBtn->setObjectName("toolBtn");
  m_cursorBtn->setCheckable(true);
  m_cursorBtn->setMinimumHeight(40);
  toolsLayout->addWidget(m_cursorBtn);

  layout->addWidget(toolsGroup);

  // === Protocol Decoder ===
  QGroupBox *protocolGroup = new QGroupBox("Protocol Decoder");
  protocolGroup->setObjectName("controlGroup");
  QVBoxLayout *protocolLayout = new QVBoxLayout(protocolGroup);
  protocolLayout->setSpacing(8);

  m_protocolBtn = new QPushButton("🔌 Open Decoder");
  m_protocolBtn->setObjectName("toolBtn");
  m_protocolBtn->setMinimumHeight(40);
  connect(m_protocolBtn, &QPushButton::clicked, this,
          &DigitalControlPanel::protocolDecoderClicked);
  protocolLayout->addWidget(m_protocolBtn);

  QLabel *protocolsLabel = new QLabel("Supported: I2C, SPI, UART, RS232");
  protocolsLabel->setObjectName("infoLabel");
  protocolsLabel->setAlignment(Qt::AlignCenter);
  protocolLayout->addWidget(protocolsLabel);

  layout->addWidget(protocolGroup);

  layout->addStretch();

  m_tabWidget->addTab(analysisTab, "ANLYS");
}

void DigitalControlPanel::applyStyles() {
  // Styles are now handled globally by ThemeManager
}

void DigitalControlPanel::setRunning(bool running) {
  m_isRunning = running;

  if (running) {
    m_runStopBtn->setText("⏹  STOP");
    m_runStopBtn->setProperty("running", true);
    m_acqStatusLabel->setText("Running");
    m_acqStatusLabel->setStyleSheet(
        "color: #4ade80; background-color: rgba(74, 222, 128, 0.1); "
        "padding: 8px; border-radius: 4px;");
  } else {
    m_runStopBtn->setText("▶  RUN");
    m_runStopBtn->setProperty("running", false);
    m_acqStatusLabel->setText("Stopped");
    m_acqStatusLabel->setStyleSheet(
        "color: #f0883e; background-color: rgba(240, 136, 62, 0.1); "
        "padding: 8px; border-radius: 4px;");
  }

  // Force style update
  m_runStopBtn->style()->unpolish(m_runStopBtn);
  m_runStopBtn->style()->polish(m_runStopBtn);
}

void DigitalControlPanel::refresh() { updateMeasurementTable(); }

void DigitalControlPanel::onRunStopClicked() { emit runStopClicked(); }

void DigitalControlPanel::onTriggerModeChanged(int index) {
  if (m_settings) {
    auto mode = static_cast<ScopeSettings::TriggerMode>(index);
    m_settings->setTriggerMode(mode);

    if (m_connection && m_connection->isConnected()) {
      QString modeStr = ScopeSettings::triggerModeToString(mode);
      m_connection->sendCommand(SCPI::triggerMode(modeStr.toUpper()));
    }
  }
  emit triggerSettingsChanged();
}

void DigitalControlPanel::onTriggerTypeChanged(int index) {
  Q_UNUSED(index)
  emit triggerSettingsChanged();
}

void DigitalControlPanel::onTriggerSourceChanged(int index) {
  if (m_settings && m_connection && m_connection->isConnected()) {
    QString source = m_trigSourceCombo->currentText();
    if (source.startsWith("CH")) {
      source = "CHAN" + source.mid(2);
    }
    m_settings->setTriggerSource(source);
    m_connection->sendCommand(SCPI::triggerEdgeSource(source));
  }
  emit triggerSettingsChanged();
}

void DigitalControlPanel::onTriggerSlopeChanged(int index) {
  if (m_settings) {
    auto slope = static_cast<ScopeSettings::TriggerSlope>(index);
    m_settings->setTriggerSlope(slope);

    if (m_connection && m_connection->isConnected()) {
      QString slopeStr;
      switch (slope) {
      case ScopeSettings::TriggerSlope::Rising:
        slopeStr = "POSitive";
        break;
      case ScopeSettings::TriggerSlope::Falling:
        slopeStr = "NEGative";
        break;
      case ScopeSettings::TriggerSlope::Either:
        slopeStr = "RFAL";
        break;
      }
      m_connection->sendCommand(SCPI::triggerEdgeSlope(slopeStr));
    }
  }
  emit triggerSettingsChanged();
}

void DigitalControlPanel::onAddMeasurement() {
  QString type = m_measTypeCombo->currentText();
  QString channel = m_measChannelCombo->currentText();
  emit measurementAdded(type, channel);
  updateMeasurementTable();
}

void DigitalControlPanel::updateMeasurementTable() {
  if (!m_engine)
    return;

  auto measurements = m_engine->activeMeasurements();
  m_measTable->setRowCount(measurements.size());

  for (int row = 0; row < measurements.size(); ++row) {
    const Measurement &m = measurements[row];

    QTableWidgetItem *typeItem =
        new QTableWidgetItem(Measurement::typeToString(m.type));
    m_measTable->setItem(row, 0, typeItem);

    QTableWidgetItem *channelItem = new QTableWidgetItem(m.channelName);
    m_measTable->setItem(row, 1, channelItem);

    QString valueStr = m.valid ? m.toString() : "---";
    QTableWidgetItem *valueItem = new QTableWidgetItem(valueStr);
    m_measTable->setItem(row, 2, valueItem);
  }
}
