#include "measurementpanel.h"
#include "../core/measurementengine.h"
#include "../core/scopechannel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMenu>

MeasurementPanel::MeasurementPanel(MeasurementEngine *engine,
                                   const QMap<QString, ScopeChannel*> &channels,
                                   QWidget *parent)
    : QWidget(parent)
    , m_engine(engine)
    , m_channels(channels)
{
    setupUI();
    
    connect(m_engine, &MeasurementEngine::measurementsUpdated, this, &MeasurementPanel::updateTable);
}

void MeasurementPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);
    
    // Quick measurements panel
    QGroupBox *quickGroup = new QGroupBox(tr("Quick Measurements"));
    QGridLayout *quickLayout = new QGridLayout(quickGroup);
    quickLayout->setSpacing(10);
    
    auto createQuickLabel = [](const QString &name) -> QLabel* {
        QLabel *label = new QLabel(QString("<b>%1:</b> ---").arg(name));
        label->setMinimumWidth(100);
        return label;
    };
    
    m_vppLabel = createQuickLabel("Vpp");
    m_freqLabel = createQuickLabel("Freq");
    m_periodLabel = createQuickLabel("Period");
    m_riseLabel = createQuickLabel("Rise");
    m_fallLabel = createQuickLabel("Fall");
    m_dutyLabel = createQuickLabel("Duty");
    
    quickLayout->addWidget(m_vppLabel, 0, 0);
    quickLayout->addWidget(m_freqLabel, 0, 1);
    quickLayout->addWidget(m_periodLabel, 0, 2);
    quickLayout->addWidget(m_riseLabel, 1, 0);
    quickLayout->addWidget(m_fallLabel, 1, 1);
    quickLayout->addWidget(m_dutyLabel, 1, 2);
    
    mainLayout->addWidget(quickGroup);
    
    // Add measurement controls
    QHBoxLayout *addLayout = new QHBoxLayout();
    
    m_typeCombo = new QComboBox();
    m_typeCombo->addItem(tr("Vpp"), static_cast<int>(MeasurementType::Vpp));
    m_typeCombo->addItem(tr("Vmax"), static_cast<int>(MeasurementType::Vmax));
    m_typeCombo->addItem(tr("Vmin"), static_cast<int>(MeasurementType::Vmin));
    m_typeCombo->addItem(tr("Vavg"), static_cast<int>(MeasurementType::Vavg));
    m_typeCombo->addItem(tr("Vrms"), static_cast<int>(MeasurementType::Vrms));
    m_typeCombo->addItem(tr("Frequency"), static_cast<int>(MeasurementType::Frequency));
    m_typeCombo->addItem(tr("Period"), static_cast<int>(MeasurementType::Period));
    m_typeCombo->addItem(tr("Rise Time"), static_cast<int>(MeasurementType::RiseTime));
    m_typeCombo->addItem(tr("Fall Time"), static_cast<int>(MeasurementType::FallTime));
    m_typeCombo->addItem(tr("Duty Cycle"), static_cast<int>(MeasurementType::DutyCycle));
    m_typeCombo->addItem(tr("+Duty Cycle"), static_cast<int>(MeasurementType::PositiveDutyCycle));
    m_typeCombo->addItem(tr("-Duty Cycle"), static_cast<int>(MeasurementType::NegativeDutyCycle));
    m_typeCombo->addItem(tr("+Width"), static_cast<int>(MeasurementType::PositiveWidth));
    m_typeCombo->addItem(tr("-Width"), static_cast<int>(MeasurementType::NegativeWidth));
    addLayout->addWidget(m_typeCombo);
    
    m_channelCombo = new QComboBox();
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        m_channelCombo->addItem(it.key());
    }
    addLayout->addWidget(m_channelCombo);
    
    m_addButton = new QPushButton(tr("Add"));
    connect(m_addButton, &QPushButton::clicked, this, &MeasurementPanel::onAddMeasurement);
    addLayout->addWidget(m_addButton);
    
    m_removeButton = new QPushButton(tr("Remove"));
    connect(m_removeButton, &QPushButton::clicked, this, &MeasurementPanel::onRemoveMeasurement);
    addLayout->addWidget(m_removeButton);
    
    m_clearButton = new QPushButton(tr("Clear All"));
    connect(m_clearButton, &QPushButton::clicked, this, &MeasurementPanel::onClearAll);
    addLayout->addWidget(m_clearButton);
    
    addLayout->addStretch();
    
    m_statisticsCheck = new QCheckBox(tr("Statistics"));
    connect(m_statisticsCheck, &QCheckBox::toggled, this, &MeasurementPanel::onStatisticsToggled);
    addLayout->addWidget(m_statisticsCheck);
    
    m_resetStatsButton = new QPushButton(tr("Reset Stats"));
    m_resetStatsButton->setEnabled(false);
    connect(m_resetStatsButton, &QPushButton::clicked, this, &MeasurementPanel::onResetStatistics);
    addLayout->addWidget(m_resetStatsButton);
    
    mainLayout->addLayout(addLayout);
    
    // Measurement table
    m_measurementTable = new QTableWidget();
    m_measurementTable->setColumnCount(7);
    m_measurementTable->setHorizontalHeaderLabels({
        tr("Type"), tr("Channel"), tr("Value"), tr("Min"), tr("Max"), tr("Avg"), tr("Std Dev")
    });
    m_measurementTable->horizontalHeader()->setStretchLastSection(true);
    m_measurementTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_measurementTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_measurementTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_measurementTable->setAlternatingRowColors(true);
    m_measurementTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Context menu
    m_measurementTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_measurementTable, &QTableWidget::customContextMenuRequested,
            this, [this](const QPoint &pos) {
        QMenu menu(this);
        menu.addAction(tr("Remove"), this, &MeasurementPanel::onRemoveMeasurement);
        menu.addAction(tr("Clear All"), this, &MeasurementPanel::onClearAll);
        menu.exec(m_measurementTable->viewport()->mapToGlobal(pos));
    });
    
    mainLayout->addWidget(m_measurementTable);
    
    // Hide statistics columns initially
    m_measurementTable->setColumnHidden(3, true);
    m_measurementTable->setColumnHidden(4, true);
    m_measurementTable->setColumnHidden(5, true);
    m_measurementTable->setColumnHidden(6, true);
}

void MeasurementPanel::refresh()
{
    updateTable();
}

void MeasurementPanel::clearAll()
{
    m_engine->clearAllMeasurements();
    m_measurementTable->setRowCount(0);
}

void MeasurementPanel::onAddMeasurement()
{
    auto type = static_cast<MeasurementType>(m_typeCombo->currentData().toInt());
    QString channel = m_channelCombo->currentText();
    
    int id = m_engine->addMeasurement(type, channel);
    emit measurementAdded(id);
    
    // Force update
    m_engine->updateMeasurements(m_channels);
}

void MeasurementPanel::onRemoveMeasurement()
{
    int row = m_measurementTable->currentRow();
    if (row >= 0) {
        QTableWidgetItem *item = m_measurementTable->item(row, 0);
        if (item) {
            int id = item->data(Qt::UserRole).toInt();
            m_engine->removeMeasurement(id);
            emit measurementRemoved(id);
            updateTable();
        }
    }
}

void MeasurementPanel::onClearAll()
{
    clearAll();
}

void MeasurementPanel::onStatisticsToggled(bool enabled)
{
    m_engine->enableStatistics(enabled);
    m_resetStatsButton->setEnabled(enabled);
    
    // Show/hide statistics columns
    m_measurementTable->setColumnHidden(3, !enabled);
    m_measurementTable->setColumnHidden(4, !enabled);
    m_measurementTable->setColumnHidden(5, !enabled);
    m_measurementTable->setColumnHidden(6, !enabled);
    
    updateTable();
}

void MeasurementPanel::onResetStatistics()
{
    m_engine->resetStatistics();
    updateTable();
}

void MeasurementPanel::updateTable()
{
    auto measurements = m_engine->activeMeasurements();
    m_measurementTable->setRowCount(measurements.size());
    
    for (int row = 0; row < measurements.size(); ++row) {
        const Measurement &m = measurements[row];
        int id = row; // Simplified - in real code, store id
        
        QTableWidgetItem *typeItem = new QTableWidgetItem(Measurement::typeToString(m.type));
        typeItem->setData(Qt::UserRole, id);
        m_measurementTable->setItem(row, 0, typeItem);
        
        m_measurementTable->setItem(row, 1, new QTableWidgetItem(m.channelName));
        
        QString valueStr = m.valid ? m.toString() : "---";
        QTableWidgetItem *valueItem = new QTableWidgetItem(valueStr);
        if (m.valid) {
            // Color based on channel
            if (m_channels.contains(m.channelName)) {
                valueItem->setForeground(m_channels[m.channelName]->color());
            }
        }
        m_measurementTable->setItem(row, 2, valueItem);
        
        if (m_statisticsCheck->isChecked()) {
            auto stats = m_engine->getStatistics(id);
            
            auto formatValue = [&m](double val) -> QString {
                Measurement temp = m;
                temp.value = val;
                return temp.toString();
            };
            
            m_measurementTable->setItem(row, 3, new QTableWidgetItem(formatValue(stats.minimum)));
            m_measurementTable->setItem(row, 4, new QTableWidgetItem(formatValue(stats.maximum)));
            m_measurementTable->setItem(row, 5, new QTableWidgetItem(formatValue(stats.average)));
            m_measurementTable->setItem(row, 6, new QTableWidgetItem(formatValue(stats.stddev)));
        }
    }
    
    // Update quick measurements for first enabled channel
    QString firstChannel;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        if (it.value()->isEnabled()) {
            firstChannel = it.key();
            break;
        }
    }
    
    if (!firstChannel.isEmpty() && m_channels.contains(firstChannel)) {
        ScopeChannel *ch = m_channels[firstChannel];
        
        auto formatVoltage = [](double v) -> QString {
            if (std::abs(v) < 1e-3) return QString("%1 mV").arg(v * 1000, 0, 'f', 2);
            return QString("%1 V").arg(v, 0, 'f', 3);
        };
        
        auto formatTime = [](double t) -> QString {
            if (t == 0) return "---";
            if (std::abs(t) < 1e-6) return QString("%1 ns").arg(t * 1e9, 0, 'f', 2);
            if (std::abs(t) < 1e-3) return QString("%1 µs").arg(t * 1e6, 0, 'f', 2);
            if (std::abs(t) < 1) return QString("%1 ms").arg(t * 1e3, 0, 'f', 2);
            return QString("%1 s").arg(t, 0, 'f', 3);
        };
        
        auto formatFreq = [](double f) -> QString {
            if (f == 0) return "---";
            if (f >= 1e9) return QString("%1 GHz").arg(f / 1e9, 0, 'f', 3);
            if (f >= 1e6) return QString("%1 MHz").arg(f / 1e6, 0, 'f', 3);
            if (f >= 1e3) return QString("%1 kHz").arg(f / 1e3, 0, 'f', 3);
            return QString("%1 Hz").arg(f, 0, 'f', 2);
        };
        
        m_vppLabel->setText(QString("<b>Vpp:</b> %1").arg(formatVoltage(ch->measureVpp())));
        m_freqLabel->setText(QString("<b>Freq:</b> %1").arg(formatFreq(ch->measureFrequency())));
        m_periodLabel->setText(QString("<b>Period:</b> %1").arg(formatTime(ch->measurePeriod())));
        m_riseLabel->setText(QString("<b>Rise:</b> %1").arg(formatTime(ch->measureRiseTime())));
        m_fallLabel->setText(QString("<b>Fall:</b> %1").arg(formatTime(ch->measureFallTime())));
        m_dutyLabel->setText(QString("<b>Duty:</b> %1%").arg(ch->measureDutyCycle(), 0, 'f', 1));
    }
}

QString MeasurementPanel::getMeasurementTypeName(MeasurementType type)
{
    return Measurement::typeToString(type);
}
