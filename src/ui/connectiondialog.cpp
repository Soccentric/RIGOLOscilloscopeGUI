#include "connectiondialog.h"
#include "../communication/deviceconnection.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QHostInfo>
#include <QProgressDialog>
#include <QApplication>

ConnectionDialog::ConnectionDialog(QWidget *parent)
    : QDialog(parent)
    , m_settings("RigolScope", "RIGOLOscilloscopeGUI")
{
    setWindowTitle(tr("Connect to Oscilloscope"));
    setMinimumSize(500, 400);
    setupUI();
    loadRecentConnections();
}

void ConnectionDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Connection settings group
    QGroupBox *settingsGroup = new QGroupBox(tr("Connection Settings"));
    QGridLayout *settingsLayout = new QGridLayout(settingsGroup);
    
    settingsLayout->addWidget(new QLabel(tr("Host/IP:")), 0, 0);
    m_hostEdit = new QLineEdit();
    m_hostEdit->setPlaceholderText(tr("e.g., 192.168.1.100"));
    m_hostEdit->setText(m_settings.value("connection/lastHost", "192.168.1.100").toString());
    settingsLayout->addWidget(m_hostEdit, 0, 1);
    
    settingsLayout->addWidget(new QLabel(tr("Port:")), 1, 0);
    m_portSpin = new QSpinBox();
    m_portSpin->setRange(1, 65535);
    m_portSpin->setValue(m_settings.value("connection/lastPort", 5555).toInt());
    settingsLayout->addWidget(m_portSpin, 1, 1);
    
    // Presets
    settingsLayout->addWidget(new QLabel(tr("Preset:")), 2, 0);
    QHBoxLayout *presetLayout = new QHBoxLayout();
    m_presetCombo = new QComboBox();
    m_presetCombo->setMinimumWidth(200);
    loadRecentConnections();
    presetLayout->addWidget(m_presetCombo);
    
    m_savePresetButton = new QPushButton(tr("Save"));
    m_savePresetButton->setMaximumWidth(60);
    connect(m_savePresetButton, &QPushButton::clicked, this, &ConnectionDialog::onSavePreset);
    presetLayout->addWidget(m_savePresetButton);
    
    m_deletePresetButton = new QPushButton(tr("Delete"));
    m_deletePresetButton->setMaximumWidth(60);
    connect(m_deletePresetButton, &QPushButton::clicked, this, &ConnectionDialog::onDeletePreset);
    presetLayout->addWidget(m_deletePresetButton);
    
    settingsLayout->addLayout(presetLayout, 2, 1);
    
    connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ConnectionDialog::onLoadPreset);
    
    mainLayout->addWidget(settingsGroup);
    
    // Scan group
    QGroupBox *scanGroup = new QGroupBox(tr("Network Scan"));
    QVBoxLayout *scanLayout = new QVBoxLayout(scanGroup);
    
    QHBoxLayout *scanButtonLayout = new QHBoxLayout();
    m_scanButton = new QPushButton(tr("Scan for Devices"));
    connect(m_scanButton, &QPushButton::clicked, this, &ConnectionDialog::onScan);
    scanButtonLayout->addWidget(m_scanButton);
    scanButtonLayout->addStretch();
    scanLayout->addLayout(scanButtonLayout);
    
    m_deviceList = new QListWidget();
    m_deviceList->setMinimumHeight(100);
    connect(m_deviceList, &QListWidget::itemDoubleClicked,
            this, &ConnectionDialog::onDeviceSelected);
    scanLayout->addWidget(m_deviceList);
    
    mainLayout->addWidget(scanGroup);
    
    // Status
    m_statusLabel = new QLabel();
    m_statusLabel->setStyleSheet("color: gray;");
    mainLayout->addWidget(m_statusLabel);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_testButton = new QPushButton(tr("Test Connection"));
    connect(m_testButton, &QPushButton::clicked, this, &ConnectionDialog::onTestConnection);
    buttonLayout->addWidget(m_testButton);
    
    buttonLayout->addStretch();
    
    m_cancelButton = new QPushButton(tr("Cancel"));
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(m_cancelButton);
    
    m_connectButton = new QPushButton(tr("Connect"));
    m_connectButton->setDefault(true);
    connect(m_connectButton, &QPushButton::clicked, this, &ConnectionDialog::onConnect);
    buttonLayout->addWidget(m_connectButton);
    
    mainLayout->addLayout(buttonLayout);
}

QString ConnectionDialog::host() const
{
    return m_hostEdit->text().trimmed();
}

quint16 ConnectionDialog::port() const
{
    return static_cast<quint16>(m_portSpin->value());
}

void ConnectionDialog::onScan()
{
    m_deviceList->clear();
    m_statusLabel->setText(tr("Scanning network..."));
    m_scanButton->setEnabled(false);
    
    // Get local network addresses
    QList<QHostAddress> localAddresses = QNetworkInterface::allAddresses();
    
    // For each local address, scan the subnet
    for (const QHostAddress &addr : localAddresses) {
        if (addr.protocol() != QAbstractSocket::IPv4Protocol) continue;
        if (addr.isLoopback()) continue;
        
        QString addrStr = addr.toString();
        QStringList parts = addrStr.split('.');
        if (parts.size() != 4) continue;
        
        QString subnet = parts[0] + "." + parts[1] + "." + parts[2] + ".";
        
        // Scan common oscilloscope ports
        QProgressDialog progress(tr("Scanning %1.x...").arg(subnet.left(subnet.length()-1)), 
                                 tr("Cancel"), 0, 254, this);
        progress.setWindowModality(Qt::WindowModal);
        
        for (int i = 1; i <= 254; ++i) {
            if (progress.wasCanceled()) break;
            progress.setValue(i);
            QApplication::processEvents();
            
            QString testHost = subnet + QString::number(i);
            QTcpSocket socket;
            socket.connectToHost(testHost, 5555);
            
            if (socket.waitForConnected(50)) {
                // Try to identify the device
                socket.write("*IDN?\n");
                socket.flush();
                
                if (socket.waitForReadyRead(500)) {
                    QString response = QString::fromUtf8(socket.readAll()).trimmed();
                    if (response.contains("RIGOL", Qt::CaseInsensitive)) {
                        QListWidgetItem *item = new QListWidgetItem(
                            QString("%1 - %2").arg(testHost, response));
                        item->setData(Qt::UserRole, testHost);
                        m_deviceList->addItem(item);
                    }
                }
                socket.disconnectFromHost();
            }
        }
    }
    
    m_scanButton->setEnabled(true);
    
    if (m_deviceList->count() == 0) {
        m_statusLabel->setText(tr("No RIGOL devices found on the network."));
    } else {
        m_statusLabel->setText(tr("Found %1 device(s).").arg(m_deviceList->count()));
    }
}

void ConnectionDialog::onDeviceSelected(QListWidgetItem *item)
{
    QString host = item->data(Qt::UserRole).toString();
    m_hostEdit->setText(host);
    m_portSpin->setValue(5555);
}

void ConnectionDialog::onConnect()
{
    if (host().isEmpty()) {
        QMessageBox::warning(this, tr("Invalid Input"), 
                            tr("Please enter a host address."));
        return;
    }
    
    saveRecentConnection(host(), port());
    accept();
}

void ConnectionDialog::onTestConnection()
{
    m_statusLabel->setText(tr("Testing connection..."));
    m_testButton->setEnabled(false);
    QApplication::processEvents();
    
    QTcpSocket socket;
    socket.connectToHost(host(), port());
    
    if (socket.waitForConnected(5000)) {
        // Send identification query
        socket.write("*IDN?\n");
        socket.flush();
        
        if (socket.waitForReadyRead(2000)) {
            QString response = QString::fromUtf8(socket.readAll()).trimmed();
            m_statusLabel->setText(tr("Connected: %1").arg(response));
            m_statusLabel->setStyleSheet("color: green;");
        } else {
            m_statusLabel->setText(tr("Connected but no response to *IDN? query."));
            m_statusLabel->setStyleSheet("color: orange;");
        }
        socket.disconnectFromHost();
    } else {
        m_statusLabel->setText(tr("Connection failed: %1").arg(socket.errorString()));
        m_statusLabel->setStyleSheet("color: red;");
    }
    
    m_testButton->setEnabled(true);
}

void ConnectionDialog::onSavePreset()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Save Preset"),
                                         tr("Preset name:"), QLineEdit::Normal,
                                         host(), &ok);
    if (!ok || name.isEmpty()) return;
    
    m_settings.beginGroup("presets");
    m_settings.setValue(name + "/host", host());
    m_settings.setValue(name + "/port", port());
    m_settings.endGroup();
    
    // Reload presets
    loadRecentConnections();
    
    // Select the new preset
    int index = m_presetCombo->findText(name);
    if (index >= 0) {
        m_presetCombo->setCurrentIndex(index);
    }
}

void ConnectionDialog::onLoadPreset()
{
    QString name = m_presetCombo->currentText();
    if (name.isEmpty()) return;
    
    m_settings.beginGroup("presets");
    m_hostEdit->setText(m_settings.value(name + "/host").toString());
    m_portSpin->setValue(m_settings.value(name + "/port", 5555).toInt());
    m_settings.endGroup();
}

void ConnectionDialog::onDeletePreset()
{
    QString name = m_presetCombo->currentText();
    if (name.isEmpty()) return;
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Delete Preset"),
        tr("Are you sure you want to delete preset '%1'?").arg(name),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        m_settings.beginGroup("presets");
        m_settings.remove(name);
        m_settings.endGroup();
        
        loadRecentConnections();
    }
}

void ConnectionDialog::loadRecentConnections()
{
    m_presetCombo->clear();
    m_presetCombo->addItem("");
    
    m_settings.beginGroup("presets");
    QStringList presets = m_settings.childGroups();
    m_settings.endGroup();
    
    for (const QString &preset : presets) {
        m_presetCombo->addItem(preset);
    }
}

void ConnectionDialog::saveRecentConnection(const QString &host, quint16 port)
{
    m_settings.setValue("connection/lastHost", host);
    m_settings.setValue("connection/lastPort", port);
    
    // Also add to recent list
    QStringList recent = m_settings.value("connection/recent").toStringList();
    QString entry = QString("%1:%2").arg(host).arg(port);
    recent.removeAll(entry);
    recent.prepend(entry);
    while (recent.size() > 10) recent.removeLast();
    m_settings.setValue("connection/recent", recent);
}
