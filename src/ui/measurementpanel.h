#ifndef MEASUREMENTPANEL_H
#define MEASUREMENTPANEL_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QMap>

class MeasurementEngine;
class ScopeChannel;
enum class MeasurementType;

/**
 * @brief Panel for displaying and managing measurements
 */
class MeasurementPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MeasurementPanel(MeasurementEngine *engine,
                              const QMap<QString, ScopeChannel*> &channels,
                              QWidget *parent = nullptr);

    void refresh();
    void clearAll();

signals:
    void measurementAdded(int id);
    void measurementRemoved(int id);

private slots:
    void onAddMeasurement();
    void onRemoveMeasurement();
    void onClearAll();
    void onStatisticsToggled(bool enabled);
    void onResetStatistics();
    void updateTable();

private:
    void setupUI();
    QString getMeasurementTypeName(MeasurementType type);

    MeasurementEngine *m_engine;
    QMap<QString, ScopeChannel*> m_channels;

    // UI elements
    QComboBox *m_typeCombo;
    QComboBox *m_channelCombo;
    QPushButton *m_addButton;
    QPushButton *m_removeButton;
    QPushButton *m_clearButton;
    
    QCheckBox *m_statisticsCheck;
    QPushButton *m_resetStatsButton;
    
    QTableWidget *m_measurementTable;
    
    // Quick measurement labels
    QLabel *m_vppLabel;
    QLabel *m_freqLabel;
    QLabel *m_periodLabel;
    QLabel *m_riseLabel;
    QLabel *m_fallLabel;
    QLabel *m_dutyLabel;
};

#endif // MEASUREMENTPANEL_H
