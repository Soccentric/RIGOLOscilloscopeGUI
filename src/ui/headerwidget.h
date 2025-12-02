/**
 * @file headerwidget.h
 * @brief Modern header widget with system-level controls and device info
 */

#ifndef HEADERWIDGET_H
#define HEADERWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QHBoxLayout>
#include <QTimer>

class DeviceConnection;
class ScopeSettings;

/**
 * @brief Professional header widget displaying device info and system controls
 * 
 * Layout: [Logo/Brand] [Device Info] [System Status] [Global Controls] [Settings]
 */
class HeaderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HeaderWidget(DeviceConnection *connection, 
                         ScopeSettings *settings,
                         QWidget *parent = nullptr);
    ~HeaderWidget() override = default;

    void setDeviceId(const QString &id);
    void setConnectionState(bool connected);
    void setSampleRate(const QString &rate);
    void setMemoryDepth(const QString &depth);
    void setAcquisitionMode(const QString &mode);

signals:
    void connectRequested();
    void disconnectRequested();
    void settingsRequested();
    void helpRequested();
    void runModeChanged(const QString &mode);
    void timebaseChanged(double timePerDiv);

private slots:
    void updateDateTime();
    void onRunModeChanged(int index);
    void onTimebaseChanged(int index);

private:
    void setupUI();
    void applyStyles();

    DeviceConnection *m_connection;
    ScopeSettings *m_settings;

    // Brand section
    QLabel *m_logoLabel;
    QLabel *m_appNameLabel;

    // Device info section
    QLabel *m_deviceIdLabel;
    QLabel *m_connectionIndicator;
    QLabel *m_sampleRateLabel;
    QLabel *m_memoryDepthLabel;

    // System status
    QLabel *m_dateTimeLabel;
    QTimer *m_clockTimer;

    // Global controls
    QComboBox *m_runModeCombo;
    QComboBox *m_timebaseCombo;
    QPushButton *m_settingsBtn;
    QPushButton *m_helpBtn;
};

#endif // HEADERWIDGET_H
