#include "channelcontrolpanel.h"
#include "../core/scopechannel.h"
#include "../core/scopesettings.h"
#include "../communication/deviceconnection.h"
#include "../communication/scpicommands.h"

#include <QColorDialog>
#include <QGridLayout>
#include <QFrame>

ChannelWidget::ChannelWidget(ScopeChannel *channel, DeviceConnection *connection, QWidget *parent)
    : QWidget(parent)
    , m_channel(channel)
    , m_connection(connection)
{
    setupUI();
    refresh();
}

void ChannelWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    // Enable checkbox with channel name/color
    QHBoxLayout *headerLayout = new QHBoxLayout();
    m_enableCheck = new QCheckBox();
    m_enableCheck->setChecked(m_channel->isEnabled());
    connect(m_enableCheck, &QCheckBox::toggled, this, &ChannelWidget::onEnableToggled);
    
    m_channelLabel = new QLabel(m_channel->name());
    QFont font = m_channelLabel->font();
    font.setBold(true);
    font.setPointSize(12);
    m_channelLabel->setFont(font);
    
    m_colorButton = new QPushButton();
    m_colorButton->setFixedSize(24, 24);
    m_colorButton->setStyleSheet(QString("background-color: %1; border: 1px solid gray;")
                                 .arg(m_channel->color().name()));
    connect(m_colorButton, &QPushButton::clicked, this, [this]() {
        QColor color = QColorDialog::getColor(m_channel->color(), this, tr("Select Channel Color"));
        if (color.isValid()) {
            m_channel->setColor(color);
            m_colorButton->setStyleSheet(QString("background-color: %1; border: 1px solid gray;")
                                         .arg(color.name()));
        }
    });
    
    headerLayout->addWidget(m_enableCheck);
    headerLayout->addWidget(m_channelLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_colorButton);
    mainLayout->addLayout(headerLayout);
    
    // Separator
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);
    
    // Scale (V/div)
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(8);
    
    QLabel *scaleLabel = new QLabel(tr("Scale:"));
    m_scaleCombo = new QComboBox();
    QVector<double> scales = ScopeSettings::standardVoltageScales();
    for (double scale : scales) {
        QString text;
        if (scale < 1.0) {
            text = QString("%1 mV/div").arg(scale * 1000);
        } else {
            text = QString("%1 V/div").arg(scale);
        }
        m_scaleCombo->addItem(text, scale);
    }
    m_scaleCombo->setCurrentIndex(m_scaleCombo->findData(m_channel->scale()));
    connect(m_scaleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        double scale = m_scaleCombo->itemData(index).toDouble();
        onScaleChanged(scale);
    });
    
    gridLayout->addWidget(scaleLabel, 0, 0);
    gridLayout->addWidget(m_scaleCombo, 0, 1);
    
    // Offset
    QLabel *offsetLabel = new QLabel(tr("Offset:"));
    m_offsetSpin = new QDoubleSpinBox();
    m_offsetSpin->setRange(-100.0, 100.0);
    m_offsetSpin->setSingleStep(0.1);
    m_offsetSpin->setValue(m_channel->offset());
    m_offsetSpin->setSuffix(" V");
    connect(m_offsetSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ChannelWidget::onOffsetChanged);
    
    gridLayout->addWidget(offsetLabel, 1, 0);
    gridLayout->addWidget(m_offsetSpin, 1, 1);
    
    // Offset slider for fine adjustment
    m_offsetSlider = new QSlider(Qt::Horizontal);
    m_offsetSlider->setRange(-1000, 1000);
    m_offsetSlider->setValue(static_cast<int>(m_channel->offset() * 100));
    connect(m_offsetSlider, &QSlider::valueChanged, this, [this](int value) {
        double offset = value / 100.0;
        m_offsetSpin->blockSignals(true);
        m_offsetSpin->setValue(offset);
        m_offsetSpin->blockSignals(false);
        onOffsetChanged(offset);
    });
    gridLayout->addWidget(m_offsetSlider, 2, 0, 1, 2);
    
    // Coupling
    QLabel *couplingLabel = new QLabel(tr("Coupling:"));
    m_couplingCombo = new QComboBox();
    m_couplingCombo->addItem("DC", static_cast<int>(ScopeChannel::Coupling::DC));
    m_couplingCombo->addItem("AC", static_cast<int>(ScopeChannel::Coupling::AC));
    m_couplingCombo->addItem("GND", static_cast<int>(ScopeChannel::Coupling::GND));
    m_couplingCombo->setCurrentIndex(static_cast<int>(m_channel->coupling()));
    connect(m_couplingCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ChannelWidget::onCouplingChanged);
    
    gridLayout->addWidget(couplingLabel, 3, 0);
    gridLayout->addWidget(m_couplingCombo, 3, 1);
    
    // Probe attenuation
    QLabel *probeLabel = new QLabel(tr("Probe:"));
    m_probeCombo = new QComboBox();
    m_probeCombo->addItem("1X", static_cast<int>(ScopeChannel::Probe::X1));
    m_probeCombo->addItem("10X", static_cast<int>(ScopeChannel::Probe::X10));
    m_probeCombo->addItem("100X", static_cast<int>(ScopeChannel::Probe::X100));
    m_probeCombo->addItem("1000X", static_cast<int>(ScopeChannel::Probe::X1000));
    m_probeCombo->setCurrentIndex(static_cast<int>(m_channel->probe()));
    connect(m_probeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ChannelWidget::onProbeChanged);
    
    gridLayout->addWidget(probeLabel, 4, 0);
    gridLayout->addWidget(m_probeCombo, 4, 1);
    
    // Bandwidth limit
    QLabel *bwLabel = new QLabel(tr("BW Limit:"));
    m_bandwidthCombo = new QComboBox();
    m_bandwidthCombo->addItem("OFF");
    m_bandwidthCombo->addItem("20 MHz");
    m_bandwidthCombo->addItem("100 MHz");
    m_bandwidthCombo->addItem("200 MHz");
    connect(m_bandwidthCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ChannelWidget::onBandwidthChanged);
    
    gridLayout->addWidget(bwLabel, 5, 0);
    gridLayout->addWidget(m_bandwidthCombo, 5, 1);
    
    mainLayout->addLayout(gridLayout);
    
    // Invert
    m_invertCheck = new QCheckBox(tr("Invert"));
    connect(m_invertCheck, &QCheckBox::toggled, this, &ChannelWidget::onInvertToggled);
    mainLayout->addWidget(m_invertCheck);
    
    mainLayout->addStretch();
}

void ChannelWidget::refresh()
{
    m_enableCheck->blockSignals(true);
    m_enableCheck->setChecked(m_channel->isEnabled());
    m_enableCheck->blockSignals(false);
    
    int scaleIndex = m_scaleCombo->findData(m_channel->scale());
    if (scaleIndex >= 0) {
        m_scaleCombo->blockSignals(true);
        m_scaleCombo->setCurrentIndex(scaleIndex);
        m_scaleCombo->blockSignals(false);
    }
    
    m_offsetSpin->blockSignals(true);
    m_offsetSpin->setValue(m_channel->offset());
    m_offsetSpin->blockSignals(false);
    
    m_offsetSlider->blockSignals(true);
    m_offsetSlider->setValue(static_cast<int>(m_channel->offset() * 100));
    m_offsetSlider->blockSignals(false);
    
    m_couplingCombo->blockSignals(true);
    m_couplingCombo->setCurrentIndex(static_cast<int>(m_channel->coupling()));
    m_couplingCombo->blockSignals(false);
    
    m_probeCombo->blockSignals(true);
    m_probeCombo->setCurrentIndex(static_cast<int>(m_channel->probe()));
    m_probeCombo->blockSignals(false);
}

void ChannelWidget::onEnableToggled(bool enabled)
{
    m_channel->setEnabled(enabled);
    
    if (m_connection && m_connection->isConnected()) {
        m_connection->sendCommand(SCPI::channelDisplay(m_channel->name(), enabled));
    }
    
    emit settingsChanged();
}

void ChannelWidget::onScaleChanged(double scale)
{
    m_channel->setScale(scale);
    
    if (m_connection && m_connection->isConnected()) {
        m_connection->sendCommand(SCPI::channelScale(m_channel->name(), scale));
    }
    
    emit settingsChanged();
}

void ChannelWidget::onOffsetChanged(double offset)
{
    m_channel->setOffset(offset);
    
    m_offsetSlider->blockSignals(true);
    m_offsetSlider->setValue(static_cast<int>(offset * 100));
    m_offsetSlider->blockSignals(false);
    
    if (m_connection && m_connection->isConnected()) {
        m_connection->sendCommand(SCPI::channelOffset(m_channel->name(), offset));
    }
    
    emit settingsChanged();
}

void ChannelWidget::onCouplingChanged(int index)
{
    auto coupling = static_cast<ScopeChannel::Coupling>(m_couplingCombo->itemData(index).toInt());
    m_channel->setCoupling(coupling);
    
    if (m_connection && m_connection->isConnected()) {
        QString couplingStr = ScopeChannel::couplingToString(coupling);
        m_connection->sendCommand(SCPI::channelCoupling(m_channel->name(), couplingStr));
    }
    
    emit settingsChanged();
}

void ChannelWidget::onProbeChanged(int index)
{
    auto probe = static_cast<ScopeChannel::Probe>(m_probeCombo->itemData(index).toInt());
    m_channel->setProbe(probe);
    
    if (m_connection && m_connection->isConnected()) {
        double ratio = m_channel->probeFactor();
        m_connection->sendCommand(SCPI::channelProbe(m_channel->name(), ratio));
    }
    
    emit settingsChanged();
}

void ChannelWidget::onBandwidthChanged(int index)
{
    if (m_connection && m_connection->isConnected()) {
        QString bw;
        switch (index) {
            case 0: bw = "OFF"; break;
            case 1: bw = "20M"; break;
            case 2: bw = "100M"; break;
            case 3: bw = "200M"; break;
        }
        m_connection->sendCommand(SCPI::channelBandwidth(m_channel->name(), bw));
    }
    
    emit settingsChanged();
}

void ChannelWidget::onInvertToggled(bool inverted)
{
    if (m_connection && m_connection->isConnected()) {
        m_connection->sendCommand(SCPI::channelInvert(m_channel->name(), inverted));
    }
    
    emit settingsChanged();
}

void ChannelWidget::syncFromDevice()
{
    if (!m_connection || !m_connection->isConnected()) return;
    
    // Query display state
    QString response = m_connection->sendQuery(SCPI::channelDisplayQuery(m_channel->name()));
    m_channel->setEnabled(response.contains("ON", Qt::CaseInsensitive));
    
    // Query scale
    response = m_connection->sendQuery(SCPI::channelScaleQuery(m_channel->name()));
    bool ok;
    double scale = response.toDouble(&ok);
    if (ok) m_channel->setScale(scale);
    
    // Query offset
    response = m_connection->sendQuery(SCPI::channelOffsetQuery(m_channel->name()));
    double offset = response.toDouble(&ok);
    if (ok) m_channel->setOffset(offset);
    
    // Query coupling
    response = m_connection->sendQuery(SCPI::channelCouplingQuery(m_channel->name()));
    m_channel->setCoupling(ScopeChannel::stringToCoupling(response));
    
    // Query probe
    response = m_connection->sendQuery(SCPI::channelProbeQuery(m_channel->name()));
    m_channel->setProbe(ScopeChannel::stringToProbe(response));
    
    refresh();
}

void ChannelWidget::sendToDevice()
{
    if (!m_connection || !m_connection->isConnected()) return;
    
    m_connection->sendCommand(SCPI::channelDisplay(m_channel->name(), m_channel->isEnabled()));
    m_connection->sendCommand(SCPI::channelScale(m_channel->name(), m_channel->scale()));
    m_connection->sendCommand(SCPI::channelOffset(m_channel->name(), m_channel->offset()));
    m_connection->sendCommand(SCPI::channelCoupling(m_channel->name(), 
                              ScopeChannel::couplingToString(m_channel->coupling())));
    m_connection->sendCommand(SCPI::channelProbe(m_channel->name(), m_channel->probeFactor()));
}

// ChannelControlPanel implementation

ChannelControlPanel::ChannelControlPanel(const QMap<QString, ScopeChannel*> &channels,
                                         DeviceConnection *connection,
                                         QWidget *parent)
    : QWidget(parent)
    , m_channels(channels)
    , m_connection(connection)
{
    setupUI();
}

void ChannelControlPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    m_tabWidget = new QTabWidget();
    m_tabWidget->setDocumentMode(true);
    
    // Create tabs for analog channels
    QStringList channelOrder = {"CH1", "CH2", "CH3", "CH4"};
    for (const QString &name : channelOrder) {
        if (m_channels.contains(name)) {
            ScopeChannel *channel = m_channels[name];
            ChannelWidget *widget = new ChannelWidget(channel, m_connection, this);
            m_channelWidgets[name] = widget;
            
            // Set tab color to channel color
            m_tabWidget->addTab(widget, name);
            int index = m_tabWidget->count() - 1;
            m_tabWidget->tabBar()->setTabTextColor(index, channel->color());
            
            connect(widget, &ChannelWidget::settingsChanged, this, [this, name]() {
                emit channelSettingsChanged(name);
            });
        }
    }
    
    mainLayout->addWidget(m_tabWidget);
    
    // Quick access buttons
    QHBoxLayout *quickLayout = new QHBoxLayout();
    
    for (int i = 1; i <= 4; ++i) {
        QString name = QString("CH%1").arg(i);
        if (m_channels.contains(name)) {
            QPushButton *btn = new QPushButton(name);
            btn->setCheckable(true);
            btn->setChecked(m_channels[name]->isEnabled());
            btn->setStyleSheet(QString("QPushButton:checked { background-color: %1; color: black; }")
                              .arg(m_channels[name]->color().name()));
            
            connect(btn, &QPushButton::toggled, this, [this, name, btn](bool checked) {
                m_channels[name]->setEnabled(checked);
                if (m_channelWidgets.contains(name)) {
                    m_channelWidgets[name]->refresh();
                }
                if (m_connection && m_connection->isConnected()) {
                    m_connection->sendCommand(SCPI::channelDisplay(name, checked));
                }
                emit channelSettingsChanged(name);
            });
            
            // Connect to channel enable changes
            connect(m_channels[name], &ScopeChannel::enabledChanged, btn, &QPushButton::setChecked);
            
            quickLayout->addWidget(btn);
        }
    }
    
    mainLayout->addLayout(quickLayout);
}

void ChannelControlPanel::refreshAll()
{
    for (auto *widget : m_channelWidgets) {
        widget->refresh();
    }
}
