#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QSettings>

/**
 * @brief Dialog for establishing connection to oscilloscope
 */
class ConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionDialog(QWidget *parent = nullptr);

    QString host() const;
    quint16 port() const;

private slots:
    void onScan();
    void onDeviceSelected(QListWidgetItem *item);
    void onConnect();
    void onTestConnection();
    void onSavePreset();
    void onLoadPreset();
    void onDeletePreset();

private:
    void setupUI();
    void loadRecentConnections();
    void saveRecentConnection(const QString &host, quint16 port);
    void scanForDevices();

    QLineEdit *m_hostEdit;
    QSpinBox *m_portSpin;
    QPushButton *m_scanButton;
    QPushButton *m_testButton;
    QPushButton *m_connectButton;
    QPushButton *m_cancelButton;
    
    QListWidget *m_deviceList;
    QLabel *m_statusLabel;
    
    // Presets
    QComboBox *m_presetCombo;
    QPushButton *m_savePresetButton;
    QPushButton *m_deletePresetButton;
    
    QSettings m_settings;
};

#endif // CONNECTIONDIALOG_H
