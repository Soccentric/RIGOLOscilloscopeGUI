/**
 * @file analogcontrolpanel.h
 * @brief Modern analog control panel for the left side
 */

#ifndef ANALOGCONTROLPANEL_H
#define ANALOGCONTROLPANEL_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QMap>
#include <QDial>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>

class ScopeChannel;
class DeviceConnection;
class ScopeSettings;

/**
 * @brief Modern analog control panel with rotary controls and sliders
 * 
 * Features analog-style controls for:
 * - Channel selection and enable
 * - Vertical scale (V/div) with rotary dial
 * - Vertical offset with slider
 * - Coupling selection
 * - Trigger controls
 */
class AnalogControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit AnalogControlPanel(const QMap<QString, ScopeChannel*> &channels,
                                DeviceConnection *connection,
                                ScopeSettings *settings,
                                QWidget *parent = nullptr);
    ~AnalogControlPanel() override = default;

    void refresh();
    QString selectedChannel() const;

signals:
    void channelChanged(const QString &channel);
    void scaleChanged(const QString &channel, double scale);
    void offsetChanged(const QString &channel, double offset);
    void triggerLevelChanged(double level);

private slots:
    void onChannelSelected(int index);
    void onChannelEnableToggled(bool enabled);
    void onScaleDialChanged(int value);
    void onOffsetSliderChanged(int value);
    void onCouplingChanged(int index);
    void onTriggerDialChanged(int value);

private:
    void setupUI();
    void applyStyles();
    void updateChannelControls();

    QMap<QString, ScopeChannel*> m_channels;
    DeviceConnection *m_connection;
    ScopeSettings *m_settings;
    QString m_currentChannel;

    // Channel selection
    QComboBox *m_channelCombo;
    QPushButton *m_enableBtn;
    QLabel *m_channelColorIndicator;

    // Scale control (V/div)
    QDial *m_scaleDial;
    QLabel *m_scaleValueLabel;
    QVector<double> m_scaleValues;

    // Offset control
    QSlider *m_offsetSlider;
    QLabel *m_offsetValueLabel;

    // Coupling
    QComboBox *m_couplingCombo;
    
    // Probe ratio
    QComboBox *m_probeCombo;

    // Trigger level (for selected channel)
    QDial *m_triggerDial;
    QLabel *m_triggerValueLabel;
    
    // Quick buttons
    QPushButton *m_autoScaleBtn;
    QPushButton *m_centerBtn;
    QPushButton *m_invertBtn;
};

#endif // ANALOGCONTROLPANEL_H
