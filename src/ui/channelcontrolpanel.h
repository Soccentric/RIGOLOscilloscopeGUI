#ifndef CHANNELCONTROLPANEL_H
#define CHANNELCONTROLPANEL_H

#include <QWidget>
#include <QMap>
#include <QTabWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QDial>

class ScopeChannel;
class DeviceConnection;

/**
 * @brief Widget for a single channel's controls
 */
class ChannelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelWidget(ScopeChannel *channel, DeviceConnection *connection, QWidget *parent = nullptr);
    void refresh();

signals:
    void settingsChanged();

private slots:
    void onEnableToggled(bool enabled);
    void onScaleChanged(double scale);
    void onOffsetChanged(double offset);
    void onCouplingChanged(int index);
    void onProbeChanged(int index);
    void onBandwidthChanged(int index);
    void onInvertToggled(bool inverted);

private:
    void setupUI();
    void syncFromDevice();
    void sendToDevice();

    ScopeChannel *m_channel;
    DeviceConnection *m_connection;

    // UI elements
    QCheckBox *m_enableCheck;
    QLabel *m_channelLabel;
    QComboBox *m_scaleCombo;
    QDoubleSpinBox *m_offsetSpin;
    QSlider *m_offsetSlider;
    QComboBox *m_couplingCombo;
    QComboBox *m_probeCombo;
    QComboBox *m_bandwidthCombo;
    QCheckBox *m_invertCheck;
    QPushButton *m_colorButton;
};

/**
 * @brief Panel containing controls for all channels
 */
class ChannelControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelControlPanel(const QMap<QString, ScopeChannel*> &channels,
                                  DeviceConnection *connection,
                                  QWidget *parent = nullptr);

    void refreshAll();

signals:
    void channelSettingsChanged(const QString &channelName);

private:
    void setupUI();

    QMap<QString, ScopeChannel*> m_channels;
    DeviceConnection *m_connection;
    QMap<QString, ChannelWidget*> m_channelWidgets;
    QTabWidget *m_tabWidget;
};

#endif // CHANNELCONTROLPANEL_H
