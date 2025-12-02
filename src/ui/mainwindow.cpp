#include "mainwindow.h"
#include "waveformdisplay.h"
#include "channelcontrolpanel.h"
#include "triggerpanel.h"
#include "measurementpanel.h"
#include "connectiondialog.h"
#include "../communication/deviceconnection.h"
#include "../communication/waveformprovider.h"
#include "../core/scopesettings.h"
#include "../core/scopechannel.h"
#include "../core/measurementengine.h"
#include "../core/cursormanager.h"
#include "../analysis/fftanalyzer.h"
#include "../analysis/protocoldecoder.h"
#include "../utils/dataexporter.h"

#include <QApplication>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QStyle>
#include <QScreen>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_connection(std::make_unique<DeviceConnection>(this))
    , m_scopeSettings(std::make_unique<ScopeSettings>(this))
    , m_measurementEngine(std::make_unique<MeasurementEngine>(this))
    , m_cursorManager(std::make_unique<CursorManager>(this))
    , m_dataExporter(std::make_unique<DataExporter>(this))
{
    setWindowTitle("RIGOL Oscilloscope Control");
    setWindowIcon(QIcon(":/icons/oscilloscope.png"));
    
    // Set minimum size
    setMinimumSize(1280, 800);
    
    // Create waveform provider after connection
    m_waveformProvider = std::make_unique<WaveformProvider>(m_connection.get(), this);
    
    // Create FFT analyzer
    m_fftAnalyzer = std::make_unique<FFTAnalyzer>(this);
    
    // Create protocol decoder
    m_protocolDecoder = std::make_unique<ProtocolDecoder>(this);
    
    // Create channels
    for (int i = 1; i <= 4; ++i) {
        QString name = QString("CH%1").arg(i);
        auto *channel = new ScopeChannel(name, ScopeChannel::ChannelType::Analog, this);
        m_channels[name] = channel;
    }
    
    // Create UI
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createCentralWidget();
    createDockWidgets();
    setupConnections();
    
    // Load settings
    loadSettings();
    
    // Apply initial theme
    applyTheme(m_isDarkMode);
    
    // Start status update timer
    m_statusUpdateTimer = new QTimer(this);
    connect(m_statusUpdateTimer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
    m_statusUpdateTimer->start(500);
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Confirm exit if connected
    if (m_connection->isConnected()) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Confirm Exit",
            "You are still connected to the oscilloscope. Do you want to disconnect and exit?",
            QMessageBox::Yes | QMessageBox::No
        );
        
        if (reply == QMessageBox::No) {
            event->ignore();
            return;
        }
        
        m_connection->disconnectFromDevice();
    }
    
    saveSettings();
    event->accept();
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    
    // Center on screen
    if (!event->spontaneous()) {
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect screenGeometry = screen->availableGeometry();
            move((screenGeometry.width() - width()) / 2,
                 (screenGeometry.height() - height()) / 2);
        }
    }
}

void MainWindow::createActions()
{
    // File actions
    m_connectAction = new QAction(QIcon::fromTheme("network-connect"), tr("&Connect..."), this);
    m_connectAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    m_connectAction->setStatusTip(tr("Connect to oscilloscope"));
    connect(m_connectAction, &QAction::triggered, this, &MainWindow::onConnectAction);
    
    m_disconnectAction = new QAction(QIcon::fromTheme("network-disconnect"), tr("&Disconnect"), this);
    m_disconnectAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
    m_disconnectAction->setStatusTip(tr("Disconnect from oscilloscope"));
    m_disconnectAction->setEnabled(false);
    connect(m_disconnectAction, &QAction::triggered, this, &MainWindow::onDisconnectAction);
    
    m_exportAction = new QAction(QIcon::fromTheme("document-save-as"), tr("&Export Data..."), this);
    m_exportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    m_exportAction->setStatusTip(tr("Export waveform data"));
    connect(m_exportAction, &QAction::triggered, this, &MainWindow::onExportData);
    
    m_screenshotAction = new QAction(QIcon::fromTheme("camera-photo"), tr("Take &Screenshot"), this);
    m_screenshotAction->setShortcut(QKeySequence(Qt::Key_F5));
    m_screenshotAction->setStatusTip(tr("Capture oscilloscope screenshot"));
    connect(m_screenshotAction, &QAction::triggered, this, &MainWindow::onTakeScreenshot);
    
    m_exitAction = new QAction(QIcon::fromTheme("application-exit"), tr("E&xit"), this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip(tr("Exit the application"));
    connect(m_exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    // View actions
    m_fullScreenAction = new QAction(QIcon::fromTheme("view-fullscreen"), tr("&Full Screen"), this);
    m_fullScreenAction->setShortcut(QKeySequence::FullScreen);
    m_fullScreenAction->setCheckable(true);
    connect(m_fullScreenAction, &QAction::triggered, this, &MainWindow::onToggleFullScreen);
    
    m_darkModeAction = new QAction(tr("&Dark Mode"), this);
    m_darkModeAction->setCheckable(true);
    m_darkModeAction->setChecked(true);
    connect(m_darkModeAction, &QAction::triggered, this, &MainWindow::onToggleDarkMode);
    
    m_resetLayoutAction = new QAction(tr("&Reset Layout"), this);
    connect(m_resetLayoutAction, &QAction::triggered, this, &MainWindow::onResetLayout);
    
    m_showChannelPanelAction = new QAction(tr("&Channels"), this);
    m_showChannelPanelAction->setCheckable(true);
    m_showChannelPanelAction->setChecked(true);
    
    m_showTriggerPanelAction = new QAction(tr("&Trigger"), this);
    m_showTriggerPanelAction->setCheckable(true);
    m_showTriggerPanelAction->setChecked(true);
    
    m_showMeasurementPanelAction = new QAction(tr("&Measurements"), this);
    m_showMeasurementPanelAction->setCheckable(true);
    m_showMeasurementPanelAction->setChecked(true);
    
    // Acquisition actions
    m_runStopAction = new QAction(QIcon(":/icons/run.png"), tr("&Run/Stop"), this);
    m_runStopAction->setShortcut(QKeySequence(Qt::Key_Space));
    m_runStopAction->setStatusTip(tr("Start or stop acquisition"));
    connect(m_runStopAction, &QAction::triggered, this, &MainWindow::onRunStop);
    
    m_singleAction = new QAction(QIcon(":/icons/single.png"), tr("&Single"), this);
    m_singleAction->setShortcut(QKeySequence(Qt::Key_S));
    m_singleAction->setStatusTip(tr("Single trigger acquisition"));
    connect(m_singleAction, &QAction::triggered, this, &MainWindow::onSingle);
    
    m_autoScaleAction = new QAction(QIcon(":/icons/autoscale.png"), tr("&Auto Scale"), this);
    m_autoScaleAction->setShortcut(QKeySequence(Qt::Key_A));
    m_autoScaleAction->setStatusTip(tr("Auto-configure oscilloscope"));
    connect(m_autoScaleAction, &QAction::triggered, this, &MainWindow::onAutoScale);
    
    m_forceTriggerAction = new QAction(tr("&Force Trigger"), this);
    m_forceTriggerAction->setShortcut(QKeySequence(Qt::Key_F));
    connect(m_forceTriggerAction, &QAction::triggered, this, &MainWindow::onForceTrigger);
    
    m_clearAction = new QAction(QIcon::fromTheme("edit-clear"), tr("&Clear"), this);
    m_clearAction->setShortcut(QKeySequence(Qt::Key_C));
    connect(m_clearAction, &QAction::triggered, this, &MainWindow::onClear);
    
    // Tools actions
    m_fftAction = new QAction(QIcon(":/icons/fft.png"), tr("&FFT Analysis"), this);
    m_fftAction->setStatusTip(tr("Open FFT analysis window"));
    connect(m_fftAction, &QAction::triggered, this, &MainWindow::onOpenFFT);
    
    m_protocolDecoderAction = new QAction(QIcon(":/icons/protocol.png"), tr("&Protocol Decoder"), this);
    m_protocolDecoderAction->setStatusTip(tr("Open protocol decoder"));
    connect(m_protocolDecoderAction, &QAction::triggered, this, &MainWindow::onOpenProtocolDecoder);
    
    m_mathChannelAction = new QAction(tr("&Math Channel"), this);
    m_mathChannelAction->setStatusTip(tr("Configure math channel"));
    connect(m_mathChannelAction, &QAction::triggered, this, &MainWindow::onOpenMathChannel);
    
    m_cursorAction = new QAction(QIcon(":/icons/cursor.png"), tr("&Cursors"), this);
    m_cursorAction->setCheckable(true);
    
    // Help actions
    m_aboutAction = new QAction(tr("&About"), this);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    
    m_aboutQtAction = new QAction(tr("About &Qt"), this);
    connect(m_aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
    
    m_checkUpdatesAction = new QAction(tr("Check for &Updates"), this);
    connect(m_checkUpdatesAction, &QAction::triggered, this, &MainWindow::onCheckForUpdates);
}

void MainWindow::createMenus()
{
    // File menu
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_connectAction);
    m_fileMenu->addAction(m_disconnectAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exportAction);
    m_fileMenu->addAction(m_screenshotAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);
    
    // View menu
    m_viewMenu = menuBar()->addMenu(tr("&View"));
    m_viewMenu->addAction(m_fullScreenAction);
    m_viewMenu->addAction(m_darkModeAction);
    m_viewMenu->addSeparator();
    
    QMenu *panelsMenu = m_viewMenu->addMenu(tr("&Panels"));
    panelsMenu->addAction(m_showChannelPanelAction);
    panelsMenu->addAction(m_showTriggerPanelAction);
    panelsMenu->addAction(m_showMeasurementPanelAction);
    
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_resetLayoutAction);
    
    // Acquisition menu
    m_acquisitionMenu = menuBar()->addMenu(tr("&Acquisition"));
    m_acquisitionMenu->addAction(m_runStopAction);
    m_acquisitionMenu->addAction(m_singleAction);
    m_acquisitionMenu->addAction(m_autoScaleAction);
    m_acquisitionMenu->addSeparator();
    m_acquisitionMenu->addAction(m_forceTriggerAction);
    m_acquisitionMenu->addAction(m_clearAction);
    
    // Measure menu
    m_measureMenu = menuBar()->addMenu(tr("&Measure"));
    m_measureMenu->addAction(m_cursorAction);
    m_measureMenu->addSeparator();
    
    QMenu *addMeasureMenu = m_measureMenu->addMenu(tr("&Add Measurement"));
    addMeasureMenu->addAction(tr("Vpp"));
    addMeasureMenu->addAction(tr("Frequency"));
    addMeasureMenu->addAction(tr("Period"));
    addMeasureMenu->addAction(tr("Rise Time"));
    addMeasureMenu->addAction(tr("Fall Time"));
    addMeasureMenu->addAction(tr("Duty Cycle"));
    
    // Tools menu
    m_toolsMenu = menuBar()->addMenu(tr("&Tools"));
    m_toolsMenu->addAction(m_fftAction);
    m_toolsMenu->addAction(m_mathChannelAction);
    m_toolsMenu->addSeparator();
    m_toolsMenu->addAction(m_protocolDecoderAction);
    
    // Help menu
    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_checkUpdatesAction);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_aboutAction);
    m_helpMenu->addAction(m_aboutQtAction);
}

void MainWindow::createToolBars()
{
    // Main toolbar
    m_mainToolBar = addToolBar(tr("Main"));
    m_mainToolBar->setObjectName("MainToolBar");
    m_mainToolBar->addAction(m_connectAction);
    m_mainToolBar->addAction(m_disconnectAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_exportAction);
    m_mainToolBar->addAction(m_screenshotAction);
    
    // Acquisition toolbar
    m_acquisitionToolBar = addToolBar(tr("Acquisition"));
    m_acquisitionToolBar->setObjectName("AcquisitionToolBar");
    m_acquisitionToolBar->addAction(m_runStopAction);
    m_acquisitionToolBar->addAction(m_singleAction);
    m_acquisitionToolBar->addAction(m_autoScaleAction);
    m_acquisitionToolBar->addSeparator();
    m_acquisitionToolBar->addAction(m_clearAction);
    
    // Measure toolbar
    m_measureToolBar = addToolBar(tr("Measure"));
    m_measureToolBar->setObjectName("MeasureToolBar");
    m_measureToolBar->addAction(m_cursorAction);
    m_measureToolBar->addAction(m_fftAction);
    m_measureToolBar->addAction(m_protocolDecoderAction);
}

void MainWindow::createStatusBar()
{
    m_connectionStatusLabel = new QLabel("Disconnected");
    m_connectionStatusLabel->setMinimumWidth(120);
    statusBar()->addWidget(m_connectionStatusLabel);
    
    m_deviceIdLabel = new QLabel("");
    m_deviceIdLabel->setMinimumWidth(200);
    statusBar()->addWidget(m_deviceIdLabel);
    
    m_sampleRateLabel = new QLabel("");
    m_sampleRateLabel->setMinimumWidth(150);
    statusBar()->addWidget(m_sampleRateLabel);
    
    m_memoryDepthLabel = new QLabel("");
    m_memoryDepthLabel->setMinimumWidth(100);
    statusBar()->addWidget(m_memoryDepthLabel);
    
    m_triggerStatusLabel = new QLabel("");
    m_triggerStatusLabel->setMinimumWidth(100);
    statusBar()->addPermanentWidget(m_triggerStatusLabel);
}

void MainWindow::createCentralWidget()
{
    m_waveformDisplay = new WaveformDisplay(m_cursorManager.get(), this);
    setCentralWidget(m_waveformDisplay);
    
    // Connect channels to display
    for (auto *channel : m_channels) {
        m_waveformDisplay->addChannel(channel);
    }
}

void MainWindow::createDockWidgets()
{
    // Channel control dock
    m_channelDock = new QDockWidget(tr("Channels"), this);
    m_channelDock->setObjectName("ChannelDock");
    m_channelPanel = new ChannelControlPanel(m_channels, m_connection.get(), this);
    m_channelDock->setWidget(m_channelPanel);
    addDockWidget(Qt::RightDockWidgetArea, m_channelDock);
    connect(m_showChannelPanelAction, &QAction::toggled, m_channelDock, &QDockWidget::setVisible);
    connect(m_channelDock, &QDockWidget::visibilityChanged, m_showChannelPanelAction, &QAction::setChecked);
    
    // Trigger dock
    m_triggerDock = new QDockWidget(tr("Trigger"), this);
    m_triggerDock->setObjectName("TriggerDock");
    m_triggerPanel = new TriggerPanel(m_scopeSettings.get(), m_connection.get(), this);
    m_triggerDock->setWidget(m_triggerPanel);
    addDockWidget(Qt::RightDockWidgetArea, m_triggerDock);
    connect(m_showTriggerPanelAction, &QAction::toggled, m_triggerDock, &QDockWidget::setVisible);
    connect(m_triggerDock, &QDockWidget::visibilityChanged, m_showTriggerPanelAction, &QAction::setChecked);
    
    // Measurement dock
    m_measurementDock = new QDockWidget(tr("Measurements"), this);
    m_measurementDock->setObjectName("MeasurementDock");
    m_measurementPanel = new MeasurementPanel(m_measurementEngine.get(), m_channels, this);
    m_measurementDock->setWidget(m_measurementPanel);
    addDockWidget(Qt::BottomDockWidgetArea, m_measurementDock);
    connect(m_showMeasurementPanelAction, &QAction::toggled, m_measurementDock, &QDockWidget::setVisible);
    connect(m_measurementDock, &QDockWidget::visibilityChanged, m_showMeasurementPanelAction, &QAction::setChecked);
    
    // Stack docks
    tabifyDockWidget(m_channelDock, m_triggerDock);
    m_channelDock->raise();
}

void MainWindow::setupConnections()
{
    // Connection state changes
    connect(m_connection.get(), &DeviceConnection::connectionStateChanged,
            this, [this](DeviceConnection::ConnectionState state) {
        onConnectionStateChanged(static_cast<int>(state));
    });
    
    connect(m_connection.get(), &DeviceConnection::deviceIdChanged,
            this, &MainWindow::onDeviceIdReceived);
    
    // Waveform updates
    connect(m_waveformProvider.get(), &WaveformProvider::waveformDataReady,
            this, [this](const QString &channel, const QVector<QPointF> &data) {
        if (m_channels.contains(channel)) {
            m_channels[channel]->setData(data);
            onWaveformUpdated(channel);
        }
    });
    
    // Measurement updates
    connect(m_measurementEngine.get(), &MeasurementEngine::measurementsUpdated,
            this, &MainWindow::onMeasurementsUpdated);
    
    // Channel changes
    for (auto *channel : m_channels) {
        connect(channel, &ScopeChannel::dataChanged, this, [this, channel]() {
            m_measurementEngine->updateMeasurements(m_channels);
            m_waveformDisplay->update();
        });
    }
}

void MainWindow::loadSettings()
{
    QSettings settings("RigolScope", "RIGOLOscilloscopeGUI");
    
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    m_isDarkMode = settings.value("darkMode", true).toBool();
    m_lastExportPath = settings.value("lastExportPath").toString();
    m_lastScreenshotPath = settings.value("lastScreenshotPath").toString();
    settings.endGroup();
    
    m_darkModeAction->setChecked(m_isDarkMode);
}

void MainWindow::saveSettings()
{
    QSettings settings("RigolScope", "RIGOLOscilloscopeGUI");
    
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.setValue("darkMode", m_isDarkMode);
    settings.setValue("lastExportPath", m_lastExportPath);
    settings.setValue("lastScreenshotPath", m_lastScreenshotPath);
    settings.endGroup();
}

void MainWindow::applyTheme(bool dark)
{
    QString styleSheet;
    
    if (dark) {
        styleSheet = R"(
            QMainWindow {
                background-color: #1e1e1e;
            }
            QMenuBar {
                background-color: #2d2d2d;
                color: #ffffff;
            }
            QMenuBar::item:selected {
                background-color: #3d3d3d;
            }
            QMenu {
                background-color: #2d2d2d;
                color: #ffffff;
                border: 1px solid #3d3d3d;
            }
            QMenu::item:selected {
                background-color: #094771;
            }
            QToolBar {
                background-color: #2d2d2d;
                border: none;
                spacing: 3px;
            }
            QDockWidget {
                color: #ffffff;
                titlebar-close-icon: url(:/icons/close.png);
            }
            QDockWidget::title {
                background-color: #2d2d2d;
                padding: 5px;
            }
            QWidget {
                background-color: #252526;
                color: #cccccc;
            }
            QGroupBox {
                border: 1px solid #3d3d3d;
                margin-top: 10px;
                padding-top: 10px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0 5px;
            }
            QPushButton {
                background-color: #0e639c;
                color: #ffffff;
                border: none;
                padding: 5px 15px;
                border-radius: 2px;
            }
            QPushButton:hover {
                background-color: #1177bb;
            }
            QPushButton:pressed {
                background-color: #094771;
            }
            QPushButton:disabled {
                background-color: #3d3d3d;
                color: #808080;
            }
            QComboBox {
                background-color: #3c3c3c;
                border: 1px solid #3d3d3d;
                padding: 3px;
                border-radius: 2px;
            }
            QComboBox:hover {
                border-color: #0e639c;
            }
            QComboBox::drop-down {
                border: none;
            }
            QLineEdit, QSpinBox, QDoubleSpinBox {
                background-color: #3c3c3c;
                border: 1px solid #3d3d3d;
                padding: 3px;
                border-radius: 2px;
            }
            QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {
                border-color: #0e639c;
            }
            QSlider::groove:horizontal {
                height: 4px;
                background: #3d3d3d;
            }
            QSlider::handle:horizontal {
                background: #0e639c;
                width: 16px;
                margin: -6px 0;
                border-radius: 8px;
            }
            QStatusBar {
                background-color: #007acc;
                color: #ffffff;
            }
            QScrollBar:vertical {
                background-color: #1e1e1e;
                width: 14px;
            }
            QScrollBar::handle:vertical {
                background-color: #5a5a5a;
                min-height: 30px;
            }
            QScrollBar::handle:vertical:hover {
                background-color: #6a6a6a;
            }
            QTabWidget::pane {
                border: 1px solid #3d3d3d;
            }
            QTabBar::tab {
                background-color: #2d2d2d;
                color: #808080;
                padding: 8px 16px;
                border: none;
            }
            QTabBar::tab:selected {
                background-color: #1e1e1e;
                color: #ffffff;
            }
            QTableWidget {
                background-color: #1e1e1e;
                gridline-color: #3d3d3d;
            }
            QTableWidget::item:selected {
                background-color: #094771;
            }
            QHeaderView::section {
                background-color: #2d2d2d;
                padding: 5px;
                border: 1px solid #3d3d3d;
            }
        )";
    } else {
        styleSheet = ""; // Use system default for light mode
    }
    
    qApp->setStyleSheet(styleSheet);
}

void MainWindow::onConnectAction()
{
    ConnectionDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        m_connection->connectToDevice(dialog.host(), dialog.port());
    }
}

void MainWindow::onDisconnectAction()
{
    if (m_waveformProvider->isContinuousAcquisitionActive()) {
        m_waveformProvider->stopContinuousAcquisition();
    }
    m_connection->disconnectFromDevice();
}

void MainWindow::onConnectionStateChanged(int state)
{
    auto connState = static_cast<DeviceConnection::ConnectionState>(state);
    
    switch (connState) {
        case DeviceConnection::ConnectionState::Disconnected:
            m_connectionStatusLabel->setText("Disconnected");
            m_connectionStatusLabel->setStyleSheet("color: #ff6b6b;");
            m_connectAction->setEnabled(true);
            m_disconnectAction->setEnabled(false);
            m_isRunning = false;
            break;
            
        case DeviceConnection::ConnectionState::Connecting:
            m_connectionStatusLabel->setText("Connecting...");
            m_connectionStatusLabel->setStyleSheet("color: #ffd93d;");
            m_connectAction->setEnabled(false);
            m_disconnectAction->setEnabled(false);
            break;
            
        case DeviceConnection::ConnectionState::Connected: {
            m_connectionStatusLabel->setText("Connected");
            m_connectionStatusLabel->setStyleSheet("color: #6bcb77;");
            m_connectAction->setEnabled(false);
            m_disconnectAction->setEnabled(true);
            
            // Setup channels for acquisition
            QStringList activeChannels;
            for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
                if (it.value()->isEnabled()) {
                    activeChannels << it.key();
                }
            }
            if (activeChannels.isEmpty()) {
                activeChannels << "CH1"; // Default to CH1
                m_channels["CH1"]->setEnabled(true);
            }
            m_waveformProvider->acquireAllChannels(activeChannels);
            break;
        }
            
        case DeviceConnection::ConnectionState::Error:
            m_connectionStatusLabel->setText("Error");
            m_connectionStatusLabel->setStyleSheet("color: #ff6b6b;");
            m_connectAction->setEnabled(true);
            m_disconnectAction->setEnabled(false);
            break;
    }
}

void MainWindow::onDeviceIdReceived(const QString &id)
{
    m_deviceIdLabel->setText(id);
    statusBar()->showMessage(tr("Connected to: %1").arg(id), 5000);
}

void MainWindow::onRunStop()
{
    if (!m_connection->isConnected()) {
        QMessageBox::warning(this, tr("Not Connected"),
            tr("Please connect to an oscilloscope first."));
        return;
    }
    
    m_isRunning = !m_isRunning;
    
    if (m_isRunning) {
        m_runStopAction->setText(tr("Stop"));
        m_runStopAction->setIcon(QIcon(":/icons/stop.png"));
        m_scopeSettings->setRunState(ScopeSettings::RunState::Running);
        
        // Start continuous acquisition
        QStringList activeChannels;
        for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
            if (it.value()->isEnabled()) {
                activeChannels << it.key();
            }
        }
        m_waveformProvider->acquireAllChannels(activeChannels);
        m_waveformProvider->startContinuousAcquisition(100);
    } else {
        m_runStopAction->setText(tr("Run"));
        m_runStopAction->setIcon(QIcon(":/icons/run.png"));
        m_scopeSettings->setRunState(ScopeSettings::RunState::Stopped);
        m_waveformProvider->stopContinuousAcquisition();
    }
}

void MainWindow::onSingle()
{
    if (!m_connection->isConnected()) {
        QMessageBox::warning(this, tr("Not Connected"),
            tr("Please connect to an oscilloscope first."));
        return;
    }
    
    m_scopeSettings->setRunState(ScopeSettings::RunState::Single);
    m_connection->sendCommand(":SINGle");
    
    // Acquire after trigger
    QStringList activeChannels;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        if (it.value()->isEnabled()) {
            activeChannels << it.key();
        }
    }
    m_waveformProvider->acquireAllChannels(activeChannels);
}

void MainWindow::onAutoScale()
{
    if (!m_connection->isConnected()) {
        return;
    }
    
    m_connection->sendCommand(":AUToscale");
    statusBar()->showMessage(tr("Auto-scaling..."), 2000);
}

void MainWindow::onForceTrigger()
{
    if (!m_connection->isConnected()) {
        return;
    }
    
    m_connection->sendCommand(":TFORce");
}

void MainWindow::onClear()
{
    if (!m_connection->isConnected()) {
        return;
    }
    
    m_connection->sendCommand(":CLEar");
    
    for (auto *channel : m_channels) {
        channel->clearData();
    }
    
    m_waveformDisplay->update();
}

void MainWindow::onToggleFullScreen()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

void MainWindow::onToggleDarkMode()
{
    m_isDarkMode = m_darkModeAction->isChecked();
    applyTheme(m_isDarkMode);
}

void MainWindow::onResetLayout()
{
    // Reset dock positions
    removeDockWidget(m_channelDock);
    removeDockWidget(m_triggerDock);
    removeDockWidget(m_measurementDock);
    
    addDockWidget(Qt::RightDockWidgetArea, m_channelDock);
    addDockWidget(Qt::RightDockWidgetArea, m_triggerDock);
    addDockWidget(Qt::BottomDockWidgetArea, m_measurementDock);
    
    tabifyDockWidget(m_channelDock, m_triggerDock);
    m_channelDock->raise();
    
    m_channelDock->show();
    m_triggerDock->show();
    m_measurementDock->show();
}

void MainWindow::onExportData()
{
    QString filter = tr("CSV Files (*.csv);;Binary Files (*.bin);;MATLAB Files (*.mat)");
    QString filename = QFileDialog::getSaveFileName(this, tr("Export Waveform Data"),
        m_lastExportPath, filter);
    
    if (filename.isEmpty()) {
        return;
    }
    
    m_lastExportPath = QFileInfo(filename).absolutePath();
    
    // Convert channels to waveform data
    QVector<DataExporter::WaveformData> waveforms;
    for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
        if (it.value()->isEnabled()) {
            DataExporter::WaveformData wfData;
            wfData.name = it.key();
            wfData.data = it.value()->data();
            wfData.voltageScale = it.value()->scale();
            wfData.voltageOffset = it.value()->offset();
            wfData.unit = "V";
            waveforms.append(wfData);
        }
    }
    
    // Export data
    bool success = false;
    DataExporter::Format format = DataExporter::Format::CSV;
    
    if (filename.endsWith(".csv", Qt::CaseInsensitive)) {
        format = DataExporter::Format::CSV;
    } else if (filename.endsWith(".bin", Qt::CaseInsensitive)) {
        format = DataExporter::Format::Binary;
    } else if (filename.endsWith(".mat", Qt::CaseInsensitive)) {
        format = DataExporter::Format::MATLAB;
    }
    
    success = m_dataExporter->exportWaveforms(filename, waveforms, format);
    
    if (success) {
        statusBar()->showMessage(tr("Data exported to: %1").arg(filename), 5000);
    } else {
        QMessageBox::warning(this, tr("Export Failed"),
            tr("Failed to export data to file."));
    }
}

void MainWindow::onTakeScreenshot()
{
    if (!m_connection->isConnected()) {
        // Take local screenshot
        QPixmap screenshot = m_waveformDisplay->grab();
        QString filename = QFileDialog::getSaveFileName(this, tr("Save Screenshot"),
            m_lastScreenshotPath, tr("PNG Files (*.png);;JPEG Files (*.jpg)"));
        
        if (!filename.isEmpty()) {
            screenshot.save(filename);
            m_lastScreenshotPath = QFileInfo(filename).absolutePath();
            statusBar()->showMessage(tr("Screenshot saved to: %1").arg(filename), 5000);
        }
        return;
    }
    
    // Request screenshot from device
    m_connection->sendCommand(":DISP:DATA? ON,OFF,PNG");
    
    QByteArray imageData = m_connection->readBinaryData(-1, 10000);
    if (!imageData.isEmpty()) {
        QString filename = QFileDialog::getSaveFileName(this, tr("Save Screenshot"),
            m_lastScreenshotPath, tr("PNG Files (*.png)"));
        
        if (!filename.isEmpty()) {
            QFile file(filename);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(imageData);
                file.close();
                m_lastScreenshotPath = QFileInfo(filename).absolutePath();
                statusBar()->showMessage(tr("Screenshot saved to: %1").arg(filename), 5000);
            }
        }
    }
}

void MainWindow::onOpenFFT()
{
    // TODO: Open FFT window
    QMessageBox::information(this, tr("FFT Analysis"),
        tr("FFT Analysis feature coming soon."));
}

void MainWindow::onOpenProtocolDecoder()
{
    // TODO: Open protocol decoder
    QMessageBox::information(this, tr("Protocol Decoder"),
        tr("Protocol Decoder feature coming soon."));
}

void MainWindow::onOpenMathChannel()
{
    // TODO: Open math channel dialog
    QMessageBox::information(this, tr("Math Channel"),
        tr("Math Channel feature coming soon."));
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, tr("About RIGOL Oscilloscope GUI"),
        tr("<h2>RIGOL Oscilloscope GUI</h2>"
           "<p>Version 1.0.0</p>"
           "<p>A professional oscilloscope control application for RIGOL MSO/DS series.</p>"
           "<p>Features:</p>"
           "<ul>"
           "<li>Real-time waveform display</li>"
           "<li>Multi-channel support</li>"
           "<li>Automatic measurements</li>"
           "<li>Cursor measurements</li>"
           "<li>FFT analysis</li>"
           "<li>Protocol decoding</li>"
           "<li>Data export</li>"
           "</ul>"
           "<p>Copyright © 2024</p>"));
}

void MainWindow::onCheckForUpdates()
{
    QDesktopServices::openUrl(QUrl("https://github.com/rigol-oscilloscope-gui/releases"));
}

void MainWindow::updateStatusBar()
{
    if (m_connection->isConnected()) {
        // Update sample rate and memory depth
        // These would be queried from the device
        m_sampleRateLabel->setText("SR: 1 GSa/s");
        m_memoryDepthLabel->setText("1M pts");
        
        // Update trigger status
        if (m_isRunning) {
            m_triggerStatusLabel->setText("Running");
            m_triggerStatusLabel->setStyleSheet("color: #6bcb77;");
        } else {
            m_triggerStatusLabel->setText("Stopped");
            m_triggerStatusLabel->setStyleSheet("color: #ffd93d;");
        }
    } else {
        m_sampleRateLabel->setText("");
        m_memoryDepthLabel->setText("");
        m_triggerStatusLabel->setText("");
    }
}

void MainWindow::onWaveformUpdated(const QString &channel)
{
    Q_UNUSED(channel)
    m_waveformDisplay->update();
}

void MainWindow::onMeasurementsUpdated()
{
    m_measurementPanel->refresh();
}
