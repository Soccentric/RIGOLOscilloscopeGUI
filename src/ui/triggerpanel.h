#ifndef TRIGGERPANEL_H
#define TRIGGERPANEL_H

#include <QWidget>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>

class ScopeSettings;
class DeviceConnection;

/**
 * @brief Panel for trigger configuration
 */
class TriggerPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TriggerPanel(ScopeSettings *settings, DeviceConnection *connection, QWidget *parent = nullptr);

    void refresh();
    void syncFromDevice();

signals:
    void triggerSettingsChanged();

private slots:
    void onModeChanged(int index);
    void onSourceChanged(int index);
    void onSlopeChanged(int index);
    void onLevelChanged(double level);
    void onForceClicked();
    void on50PercentClicked();

private:
    void setupUI();
    void updateUI();
    void sendToDevice();

    ScopeSettings *m_settings;
    DeviceConnection *m_connection;

    // Mode
    QComboBox *m_modeCombo;
    
    // Type (Edge, Pulse, Video, etc.)
    QComboBox *m_typeCombo;
    
    // Source
    QComboBox *m_sourceCombo;
    
    // Slope/Edge
    QComboBox *m_slopeCombo;
    
    // Level
    QDoubleSpinBox *m_levelSpin;
    QSlider *m_levelSlider;
    QPushButton *m_50PercentButton;
    
    // Coupling
    QComboBox *m_couplingCombo;
    
    // Holdoff
    QDoubleSpinBox *m_holdoffSpin;
    
    // Noise rejection
    QComboBox *m_noiseRejectCombo;
    
    // Status
    QLabel *m_statusLabel;
    
    // Actions
    QPushButton *m_forceButton;
};

#endif // TRIGGERPANEL_H
