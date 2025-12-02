/**
 * @file digitalcontrolpanel.h
 * @brief Modern digital control panel for the right side
 */

#ifndef DIGITALCONTROLPANEL_H
#define DIGITALCONTROLPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QMap>

class ScopeChannel;
class DeviceConnection;
class ScopeSettings;
class MeasurementEngine;

/**
 * @brief Modern digital control panel with measurements, triggers, and analysis
 * 
 * Features digital-style controls for:
 * - Acquisition controls (Run/Stop, Single, Auto)
 * - Trigger settings
 * - Measurements configuration
 * - FFT/Analysis shortcuts
 * - Protocol decoder access
 */
class DigitalControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit DigitalControlPanel(const QMap<QString, ScopeChannel*> &channels,
                                 DeviceConnection *connection,
                                 ScopeSettings *settings,
                                 MeasurementEngine *engine,
                                 QWidget *parent = nullptr);
    ~DigitalControlPanel() override = default;

    void setRunning(bool running);
    void refresh();

signals:
    void runStopClicked();
    void singleClicked();
    void autoScaleClicked();
    void forceTriggerClicked();
    void clearClicked();
    void fftClicked();
    void protocolDecoderClicked();
    void measurementAdded(const QString &type, const QString &channel);
    void triggerSettingsChanged();

private slots:
    void onRunStopClicked();
    void onTriggerModeChanged(int index);
    void onTriggerTypeChanged(int index);
    void onTriggerSourceChanged(int index);
    void onTriggerSlopeChanged(int index);
    void onAddMeasurement();
    void updateMeasurementTable();

private:
    void setupUI();
    void applyStyles();
    void createAcquisitionTab();
    void createTriggerTab();
    void createMeasurementsTab();
    void createAnalysisTab();

    QMap<QString, ScopeChannel*> m_channels;
    DeviceConnection *m_connection;
    ScopeSettings *m_settings;
    MeasurementEngine *m_engine;

    QTabWidget *m_tabWidget;
    
    // Acquisition controls
    QPushButton *m_runStopBtn;
    QPushButton *m_singleBtn;
    QPushButton *m_autoBtn;
    QPushButton *m_clearBtn;
    QLabel *m_acqStatusLabel;
    QSpinBox *m_acqCountSpin;

    // Trigger controls
    QComboBox *m_trigModeCombo;
    QComboBox *m_trigTypeCombo;
    QComboBox *m_trigSourceCombo;
    QComboBox *m_trigSlopeCombo;
    QDoubleSpinBox *m_trigLevelSpin;
    QDoubleSpinBox *m_trigHoldoffSpin;
    QPushButton *m_forceBtn;
    QPushButton *m_50PercentBtn;

    // Measurements
    QComboBox *m_measTypeCombo;
    QComboBox *m_measChannelCombo;
    QPushButton *m_addMeasBtn;
    QPushButton *m_clearMeasBtn;
    QTableWidget *m_measTable;
    QCheckBox *m_statsCheck;

    // Analysis
    QPushButton *m_fftBtn;
    QPushButton *m_mathBtn;
    QPushButton *m_protocolBtn;
    QPushButton *m_cursorBtn;
    
    bool m_isRunning = false;
};

#endif // DIGITALCONTROLPANEL_H
