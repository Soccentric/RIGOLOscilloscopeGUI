/**
 * @file mainwindow.cpp
 * @brief Implementation of the modern professional main window
 */

#include "mainwindow.h"
#include "headerwidget.h"
#include "footerwidget.h"
#include "analogcontrolpanel.h"
#include "digitalcontrolpanel.h"
#include "waveformdisplay.h"
#include "connectiondialog.h"
#include "fftwindow.h"
#include "../communication/scpicommands.h"
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
#include <QShortcut>
#include <QFrame>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_connection(std::make_unique<DeviceConnection>(this))
    , m_scopeSettings(std::make_unique<ScopeSettings>(this))
    , m_measurementEngine(std::make_unique<MeasurementEngine>(this))
    , m_cursorManager(std::make_unique<CursorManager>(this))
    , m_dataExporter(std::make_unique<DataExporter>(this))
{
    setWindowTitle("RIGOL Oscilloscope Pro");
    setWindowIcon(QIcon(":/icons/oscilloscope.png"));
    
    // Set minimum and default size
    setMinimumSize(1280, 800);
    resize(1600, 1000);
    
    // Apply modern dark theme to window
    setStyleSheet(R"(
        QMainWindow {
            background-color: #0d1117;
        }
        QMenuBar {
            background-color: #161b22;
            color: #c9d1d9;
            border-bottom: 1px solid #21262d;
            padding: 4px;
        }
        QMenuBar::item {
            padding: 6px 12px;
            border-radius: 4px;
        }
        QMenuBar::item:selected {
            background-color: #21262d;
        }
        QMenu {
            background-color: #161b22;
            border: 1px solid #30363d;
            border-radius: 6px;
            padding: 4px;
        }
        QMenu::item {
            padding: 8px 24px;
            border-radius: 4px;
            color: #c9d1d9;
        }
        QMenu::item:selected {
            background-color: #388bfd;
            color: white;
        }
        QMenu::separator {
            height: 1px;
            background-color: #21262d;
            margin: 4px 8px;
        }
    )");
    
    // Create waveform provider after connection
    m_waveformProvider = std::make_unique<WaveformProvider>(m_connection.get(), this);
    
    // Create protocol decoder
    m_protocolDecoder = std::make_unique<ProtocolDecoder>(this);
    
    // Create channels with professional colors
    QStringList channelColors = {"#ffeb3b", "#00e5ff", "#ff4081", "#00e676"};
    for (int i = 1; i <= 4; ++i) {
        QString name = QString("CH%1").arg(i);
        auto *channel = new ScopeChannel(name, ScopeChannel::ChannelType::Analog, this);
        channel->setColor(QColor(channelColors[i-1]));
        if (i == 1) channel->setEnabled(true); // Enable CH1 by default
        m_channels[name] = channel;
    }
    
    // Create UI components
    createActions();
    createMenus();
    createCentralLayout();
    createShortcuts();
    setupConnections();
    
    // Load settings
    loadSettings();
    
    // Start status update timer
    m_statusUpdateTimer = new QTimer(this);
    connect(m_statusUpdateTimer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
    m_statusUpdateTimer->start(500);
    
    // Acquisition timer for acquisition count
    m_acquisitionTimer = new QTimer(this);
    connect(m_acquisitionTimer, &QTimer::timeout, this, [this]() {
        if (m_isRunning) {
            m_acquisitionCount++;
            m_footerWidget->setAcquisitionCount(m_acquisitionCount);
        }
    });
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
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
    
    if (!event->spontaneous()) {
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect screenGeometry = screen->availableGeometry();
            move((screenGeometry.width() - width()) / 2,
                 (screenGeometry.height() - height()) / 2);
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Global keyboard shortcuts
    switch (event->key()) {
        case Qt::Key_Space:
            onRunStop();
            break;
        case Qt::Key_Escape:
            if (isFullScreen()) {
                showNormal();
            }
            break;
        default:
            QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    // Layout automatically adjusts - splitter handles flexible sizing
}

void MainWindow::createActions()
{
    // File actions
    m_connectAction = new QAction(tr("&Connect..."), this);
    m_connectAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    m_connectAction->setStatusTip(tr("Connect to oscilloscope"));
    connect(m_connectAction, &QAction::triggered, this, &MainWindow::onConnectAction);
    
    m_disconnectAction = new QAction(tr("&Disconnect"), this);
    m_disconnectAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
    m_disconnectAction->setEnabled(false);
    connect(m_disconnectAction, &QAction::triggered, this, &MainWindow::onDisconnectAction);
    
    m_exportAction = new QAction(tr("&Export Data..."), this);
    m_exportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    connect(m_exportAction, &QAction::triggered, this, &MainWindow::onExportData);
    
    m_screenshotAction = new QAction(tr("Take &Screenshot"), this);
    m_screenshotAction->setShortcut(QKeySequence(Qt::Key_F5));
    connect(m_screenshotAction, &QAction::triggered, this, &MainWindow::onTakeScreenshot);
    
    m_exitAction = new QAction(tr("E&xit"), this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    connect(m_exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    // View actions
    m_fullScreenAction = new QAction(tr("&Full Screen"), this);
    m_fullScreenAction->setShortcut(QKeySequence::FullScreen);
    m_fullScreenAction->setCheckable(true);
    connect(m_fullScreenAction, &QAction::triggered, this, &MainWindow::onToggleFullScreen);
    
    m_darkModeAction = new QAction(tr("&Dark Mode"), this);
    m_darkModeAction->setCheckable(true);
    m_darkModeAction->setChecked(true);
    connect(m_darkModeAction, &QAction::triggered, this, &MainWindow::onToggleDarkMode);
    
    m_resetLayoutAction = new QAction(tr("&Reset Layout"), this);
    connect(m_resetLayoutAction, &QAction::triggered, this, &MainWindow::onResetLayout);
    
    m_toggleLeftPanelAction = new QAction(tr("&Analog Controls"), this);
    m_toggleLeftPanelAction->setCheckable(true);
    m_toggleLeftPanelAction->setChecked(true);
    m_toggleLeftPanelAction->setShortcut(QKeySequence(Qt::Key_F1));
    connect(m_toggleLeftPanelAction, &QAction::triggered, this, &MainWindow::onToggleLeftPanel);
    
    m_toggleRightPanelAction = new QAction(tr("&Digital Controls"), this);
    m_toggleRightPanelAction->setCheckable(true);
    m_toggleRightPanelAction->setChecked(true);
    m_toggleRightPanelAction->setShortcut(QKeySequence(Qt::Key_F2));
    connect(m_toggleRightPanelAction, &QAction::triggered, this, &MainWindow::onToggleRightPanel);
    
    m_toggleFFTDisplayAction = new QAction(tr("&FFT Display"), this);
    m_toggleFFTDisplayAction->setCheckable(true);
    m_toggleFFTDisplayAction->setShortcut(QKeySequence(Qt::Key_F3));
    connect(m_toggleFFTDisplayAction, &QAction::triggered, this, &MainWindow::onToggleFFTDisplay);
    
    // Acquisition actions
    m_runStopAction = new QAction(tr("&Run/Stop"), this);
    m_runStopAction->setShortcut(QKeySequence(Qt::Key_Space));
    connect(m_runStopAction, &QAction::triggered, this, &MainWindow::onRunStop);
    
    m_singleAction = new QAction(tr("&Single"), this);
    m_singleAction->setShortcut(QKeySequence(Qt::Key_S));
    connect(m_singleAction, &QAction::triggered, this, &MainWindow::onSingle);
    
    m_autoScaleAction = new QAction(tr("&Auto Scale"), this);
    m_autoScaleAction->setShortcut(QKeySequence(Qt::Key_A));
    connect(m_autoScaleAction, &QAction::triggered, this, &MainWindow::onAutoScale);
    
    m_forceTriggerAction = new QAction(tr("&Force Trigger"), this);
    m_forceTriggerAction->setShortcut(QKeySequence(Qt::Key_F));
    connect(m_forceTriggerAction, &QAction::triggered, this, &MainWindow::onForceTrigger);
    
    m_clearAction = new QAction(tr("&Clear"), this);
    m_clearAction->setShortcut(QKeySequence(Qt::Key_C));
    connect(m_clearAction, &QAction::triggered, this, &MainWindow::onClear);
    
    // Channel actions
    m_channel1Action = new QAction(tr("Channel &1"), this);
    m_channel1Action->setShortcut(QKeySequence(Qt::Key_1));
    m_channel1Action->setCheckable(true);
    m_channel1Action->setChecked(true);
    connect(m_channel1Action, &QAction::triggered, this, [this]() { toggleChannel("CH1"); });
    
    m_channel2Action = new QAction(tr("Channel &2"), this);
    m_channel2Action->setShortcut(QKeySequence(Qt::Key_2));
    m_channel2Action->setCheckable(true);
    connect(m_channel2Action, &QAction::triggered, this, [this]() { toggleChannel("CH2"); });
    
    m_channel3Action = new QAction(tr("Channel &3"), this);
    m_channel3Action->setShortcut(QKeySequence(Qt::Key_3));
    m_channel3Action->setCheckable(true);
    connect(m_channel3Action, &QAction::triggered, this, [this]() { toggleChannel("CH3"); });
    
    m_channel4Action = new QAction(tr("Channel &4"), this);
    m_channel4Action->setShortcut(QKeySequence(Qt::Key_4));
    m_channel4Action->setCheckable(true);
    connect(m_channel4Action, &QAction::triggered, this, [this]() { toggleChannel("CH4"); });
    
    // Tools actions
    m_fftAction = new QAction(tr("&FFT Analysis"), this);
    m_fftAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
    connect(m_fftAction, &QAction::triggered, this, &MainWindow::onOpenFFT);
    
    m_protocolDecoderAction = new QAction(tr("&Protocol Decoder"), this);
    m_protocolDecoderAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));
    connect(m_protocolDecoderAction, &QAction::triggered, this, &MainWindow::onOpenProtocolDecoder);
    
    m_mathChannelAction = new QAction(tr("&Math Channel"), this);
    m_mathChannelAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_M));
    connect(m_mathChannelAction, &QAction::triggered, this, &MainWindow::onOpenMathChannel);
    
    m_cursorAction = new QAction(tr("&Cursors"), this);
    m_cursorAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
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
    panelsMenu->addAction(m_toggleLeftPanelAction);
    panelsMenu->addAction(m_toggleRightPanelAction);
    panelsMenu->addAction(m_toggleFFTDisplayAction);
    
    QMenu *channelsMenu = m_viewMenu->addMenu(tr("&Channels"));
    channelsMenu->addAction(m_channel1Action);
    channelsMenu->addAction(m_channel2Action);
    channelsMenu->addAction(m_channel3Action);
    channelsMenu->addAction(m_channel4Action);
    
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
    m_measureMenu->addAction(m_fftAction);
    
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

void MainWindow::createCentralLayout()
{
    // Create main central container
    m_centralContainer = new QWidget;
    m_centralContainer->setObjectName("centralContainer");
    setCentralWidget(m_centralContainer);
    
    // Main vertical layout: Header | Content | Footer
    m_mainLayout = new QVBoxLayout(m_centralContainer);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // === HEADER ===
    m_headerWidget = new HeaderWidget(m_connection.get(), m_scopeSettings.get(), this);
    m_mainLayout->addWidget(m_headerWidget);
    
    // === CONTENT AREA ===
    // Horizontal layout: Left Panel | Center | Right Panel
    QWidget *contentWidget = new QWidget;
    contentWidget->setObjectName("contentWidget");
    m_contentLayout = new QHBoxLayout(contentWidget);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(0);
    
    // Horizontal splitter for resizable panels
    m_horizontalSplitter = new QSplitter(Qt::Horizontal);
    m_horizontalSplitter->setObjectName("mainSplitter");
    m_horizontalSplitter->setHandleWidth(4);
    m_horizontalSplitter->setStyleSheet(R"(
        QSplitter::handle {
            background-color: #21262d;
        }
        QSplitter::handle:hover {
            background-color: #388bfd;
        }
    )");
    
    // === LEFT PANEL (Analog Controls) ===
    m_analogPanel = new AnalogControlPanel(m_channels, m_connection.get(), 
                                           m_scopeSettings.get(), this);
    m_horizontalSplitter->addWidget(m_analogPanel);
    
    // === CENTER SECTION ===
    m_centerWidget = new QWidget;
    m_centerWidget->setObjectName("centerWidget");
    QVBoxLayout *centerLayout = new QVBoxLayout(m_centerWidget);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(0);
    
    // Vertical splitter for waveform displays
    m_verticalSplitter = new QSplitter(Qt::Vertical);
    m_verticalSplitter->setObjectName("displaySplitter");
    m_verticalSplitter->setHandleWidth(4);
    m_verticalSplitter->setStyleSheet(R"(
        QSplitter::handle {
            background-color: #21262d;
        }
        QSplitter::handle:hover {
            background-color: #388bfd;
        }
    )");
    
    // Main analog waveform display
    m_analogDisplay = new WaveformDisplay(m_cursorManager.get(), this);
    m_analogDisplay->setObjectName("analogDisplay");
    m_analogDisplay->setMinimumHeight(300);
    m_verticalSplitter->addWidget(m_analogDisplay);
    
    // Connect channels to display
    for (auto *channel : m_channels) {
        m_analogDisplay->addChannel(channel);
    }
    
    // Secondary display (FFT/Digital) - hidden by default
    m_digitalDisplay = new WaveformDisplay(m_cursorManager.get(), this);
    m_digitalDisplay->setObjectName("digitalDisplay");
    m_digitalDisplay->setMinimumHeight(150);
    m_digitalDisplay->hide();
    m_verticalSplitter->addWidget(m_digitalDisplay);
    
    // Set initial splitter sizes
    m_verticalSplitter->setSizes({700, 300});
    
    centerLayout->addWidget(m_verticalSplitter);
    m_horizontalSplitter->addWidget(m_centerWidget);
    
    // === RIGHT PANEL (Digital Controls) ===
    m_digitalPanel = new DigitalControlPanel(m_channels, m_connection.get(),
                                             m_scopeSettings.get(), 
                                             m_measurementEngine.get(), this);
    m_horizontalSplitter->addWidget(m_digitalPanel);
    
    // Set splitter stretch factors (center takes all available space)
    m_horizontalSplitter->setStretchFactor(0, 0);  // Left panel - fixed
    m_horizontalSplitter->setStretchFactor(1, 1);  // Center - stretch
    m_horizontalSplitter->setStretchFactor(2, 0);  // Right panel - fixed
    
    // Set initial sizes (left: 250, center: flexible, right: 280)
    m_horizontalSplitter->setSizes({250, 800, 280});
    
    m_contentLayout->addWidget(m_horizontalSplitter);
    m_mainLayout->addWidget(contentWidget, 1);  // Content area takes remaining space
    
    // === FOOTER ===
    m_footerWidget = new FooterWidget(m_channels, m_measurementEngine.get(), this);
    m_mainLayout->addWidget(m_footerWidget);
}

void MainWindow::createShortcuts()
{
    // Additional global shortcuts
    new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(onToggleFullScreen()));
    new QShortcut(QKeySequence(Qt::Key_R), this, [this]() {
        m_cursorAction->setChecked(!m_cursorAction->isChecked());
    });
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
            m_analogDisplay->update();
            if (m_digitalDisplay->isVisible()) {
                m_digitalDisplay->update();
            }
        });
    }
    
    // Header widget connections
    connect(m_headerWidget, &HeaderWidget::connectRequested, 
            this, &MainWindow::onConnectAction);
    connect(m_headerWidget, &HeaderWidget::disconnectRequested,
            this, &MainWindow::onDisconnectAction);
    connect(m_headerWidget, &HeaderWidget::runModeChanged,
            this, &MainWindow::onRunModeChanged);
    connect(m_headerWidget, &HeaderWidget::timebaseChanged,
            this, &MainWindow::onTimebaseChanged);
    
    // Analog panel connections
    connect(m_analogPanel, &AnalogControlPanel::channelChanged,
            this, &MainWindow::onChannelSettingsChanged);
    connect(m_analogPanel, &AnalogControlPanel::scaleChanged,
            this, [this](const QString &channel, double) {
        onChannelSettingsChanged(channel);
    });
    connect(m_analogPanel, &AnalogControlPanel::offsetChanged,
            this, [this](const QString &channel, double) {
        onChannelSettingsChanged(channel);
    });
    connect(m_analogPanel, &AnalogControlPanel::triggerLevelChanged,
            this, &MainWindow::onTriggerLevelChanged);
    
    // Digital panel connections
    connect(m_digitalPanel, &DigitalControlPanel::runStopClicked,
            this, &MainWindow::onRunStop);
    connect(m_digitalPanel, &DigitalControlPanel::singleClicked,
            this, &MainWindow::onSingle);
    connect(m_digitalPanel, &DigitalControlPanel::autoScaleClicked,
            this, &MainWindow::onAutoScale);
    connect(m_digitalPanel, &DigitalControlPanel::forceTriggerClicked,
            this, &MainWindow::onForceTrigger);
    connect(m_digitalPanel, &DigitalControlPanel::clearClicked,
            this, &MainWindow::onClear);
    connect(m_digitalPanel, &DigitalControlPanel::fftClicked,
            this, &MainWindow::onOpenFFT);
    connect(m_digitalPanel, &DigitalControlPanel::protocolDecoderClicked,
            this, &MainWindow::onOpenProtocolDecoder);
    connect(m_digitalPanel, &DigitalControlPanel::measurementAdded,
            this, &MainWindow::onMeasurementAdded);
    
    // Waveform display signals
    connect(m_analogDisplay, &WaveformDisplay::toggleCursors, this, [this]() {
        m_cursorAction->setChecked(!m_cursorAction->isChecked());
    });
}

void MainWindow::loadSettings()
{
    QSettings settings("RigolScope", "RIGOLOscilloscopeGUI");
    
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    m_isDarkMode = settings.value("darkMode", true).toBool();
    m_lastExportPath = settings.value("lastExportPath").toString();
    m_lastScreenshotPath = settings.value("lastScreenshotPath").toString();
    m_leftPanelVisible = settings.value("leftPanelVisible", true).toBool();
    m_rightPanelVisible = settings.value("rightPanelVisible", true).toBool();
    settings.endGroup();
    
    m_darkModeAction->setChecked(m_isDarkMode);
    m_toggleLeftPanelAction->setChecked(m_leftPanelVisible);
    m_toggleRightPanelAction->setChecked(m_rightPanelVisible);
    
    m_analogPanel->setVisible(m_leftPanelVisible);
    m_digitalPanel->setVisible(m_rightPanelVisible);
}

void MainWindow::saveSettings()
{
    QSettings settings("RigolScope", "RIGOLOscilloscopeGUI");
    
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("darkMode", m_isDarkMode);
    settings.setValue("lastExportPath", m_lastExportPath);
    settings.setValue("lastScreenshotPath", m_lastScreenshotPath);
    settings.setValue("leftPanelVisible", m_leftPanelVisible);
    settings.setValue("rightPanelVisible", m_rightPanelVisible);
    settings.endGroup();
}

void MainWindow::updateUIState()
{
    // Update all UI elements based on current state
    m_footerWidget->setRunning(m_isRunning);
    m_digitalPanel->setRunning(m_isRunning);
    m_analogPanel->refresh();
    m_digitalPanel->refresh();
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
            m_headerWidget->setConnectionState(false);
            m_headerWidget->setDeviceId("");
            m_headerWidget->setSampleRate("---");
            m_headerWidget->setMemoryDepth("---");
            m_footerWidget->setTriggerStatus("Disconnected");
            m_connectAction->setEnabled(true);
            m_disconnectAction->setEnabled(false);
            m_isRunning = false;
            m_acquisitionTimer->stop();
            updateUIState();
            break;
            
        case DeviceConnection::ConnectionState::Connecting:
            m_headerWidget->setDeviceId("Connecting...");
            m_connectAction->setEnabled(false);
            m_disconnectAction->setEnabled(false);
            break;
            
        case DeviceConnection::ConnectionState::Connected: {
            m_headerWidget->setConnectionState(true);
            m_footerWidget->setTriggerStatus("Waiting");
            m_connectAction->setEnabled(false);
            m_disconnectAction->setEnabled(true);
            
            // Query device info
            m_headerWidget->setSampleRate("1 GSa/s");
            m_headerWidget->setMemoryDepth("1M pts");
            
            // Setup channels for acquisition
            QStringList activeChannels;
            for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
                if (it.value()->isEnabled()) {
                    activeChannels << it.key();
                }
            }
            if (activeChannels.isEmpty()) {
                activeChannels << "CH1";
                m_channels["CH1"]->setEnabled(true);
            }
            m_waveformProvider->acquireAllChannels(activeChannels);
            break;
        }
            
        case DeviceConnection::ConnectionState::Error:
            m_headerWidget->setConnectionState(false);
            m_headerWidget->setDeviceId("Connection Error");
            m_footerWidget->setTriggerStatus("Error");
            m_connectAction->setEnabled(true);
            m_disconnectAction->setEnabled(false);
            break;
    }
}

void MainWindow::onDeviceIdReceived(const QString &id)
{
    m_headerWidget->setDeviceId(id);
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
        m_scopeSettings->setRunState(ScopeSettings::RunState::Running);
        m_footerWidget->setTriggerStatus("Auto");
        m_acquisitionCount = 0;
        m_acquisitionTimer->start(100);
        
        QStringList activeChannels;
        for (auto it = m_channels.begin(); it != m_channels.end(); ++it) {
            if (it.value()->isEnabled()) {
                activeChannels << it.key();
            }
        }
        m_waveformProvider->acquireAllChannels(activeChannels);
        m_waveformProvider->startContinuousAcquisition(100);
    } else {
        m_scopeSettings->setRunState(ScopeSettings::RunState::Stopped);
        m_footerWidget->setTriggerStatus("Stopped");
        m_waveformProvider->stopContinuousAcquisition();
        m_acquisitionTimer->stop();
    }
    
    updateUIState();
}

void MainWindow::onSingle()
{
    if (!m_connection->isConnected()) {
        QMessageBox::warning(this, tr("Not Connected"),
            tr("Please connect to an oscilloscope first."));
        return;
    }
    
    m_scopeSettings->setRunState(ScopeSettings::RunState::Single);
    m_footerWidget->setTriggerStatus("Waiting");
    m_connection->sendCommand(":SINGle");
    
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
    if (!m_connection->isConnected()) return;
    
    m_connection->sendCommand(":AUToscale");
    m_footerWidget->setTriggerStatus("Auto-scaling...");
}

void MainWindow::onForceTrigger()
{
    if (!m_connection->isConnected()) return;
    
    m_connection->sendCommand(":TFORce");
    m_footerWidget->setTriggerStatus("Forced");
}

void MainWindow::onClear()
{
    if (m_connection->isConnected()) {
        m_connection->sendCommand(":CLEar");
    }
    
    for (auto *channel : m_channels) {
        channel->clearData();
    }
    
    m_acquisitionCount = 0;
    m_footerWidget->setAcquisitionCount(0);
    m_analogDisplay->update();
    if (m_digitalDisplay->isVisible()) {
        m_digitalDisplay->update();
    }
}

void MainWindow::onToggleFullScreen()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
    m_fullScreenAction->setChecked(isFullScreen());
}

void MainWindow::onToggleDarkMode()
{
    m_isDarkMode = m_darkModeAction->isChecked();
    // Theme is already dark by design
}

void MainWindow::onResetLayout()
{
    // Reset panel visibility
    m_leftPanelVisible = true;
    m_rightPanelVisible = true;
    m_showFFTDisplay = false;
    
    m_analogPanel->setVisible(true);
    m_digitalPanel->setVisible(true);
    m_digitalDisplay->hide();
    
    m_toggleLeftPanelAction->setChecked(true);
    m_toggleRightPanelAction->setChecked(true);
    m_toggleFFTDisplayAction->setChecked(false);
    
    // Reset splitter sizes
    m_horizontalSplitter->setSizes({250, 800, 280});
    m_verticalSplitter->setSizes({700, 300});
}

void MainWindow::onToggleLeftPanel()
{
    m_leftPanelVisible = m_toggleLeftPanelAction->isChecked();
    m_analogPanel->setVisible(m_leftPanelVisible);
}

void MainWindow::onToggleRightPanel()
{
    m_rightPanelVisible = m_toggleRightPanelAction->isChecked();
    m_digitalPanel->setVisible(m_rightPanelVisible);
}

void MainWindow::onToggleFFTDisplay()
{
    m_showFFTDisplay = m_toggleFFTDisplayAction->isChecked();
    m_digitalDisplay->setVisible(m_showFFTDisplay);
    
    if (m_showFFTDisplay) {
        m_verticalSplitter->setSizes({500, 300});
    }
}

void MainWindow::onExportData()
{
    QString filter = tr("CSV Files (*.csv);;Binary Files (*.bin);;MATLAB Files (*.mat)");
    QString filename = QFileDialog::getSaveFileName(this, tr("Export Waveform Data"),
        m_lastExportPath, filter);
    
    if (filename.isEmpty()) return;
    
    m_lastExportPath = QFileInfo(filename).absolutePath();
    
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
    
    DataExporter::Format format = DataExporter::Format::CSV;
    if (filename.endsWith(".bin", Qt::CaseInsensitive)) {
        format = DataExporter::Format::Binary;
    } else if (filename.endsWith(".mat", Qt::CaseInsensitive)) {
        format = DataExporter::Format::MATLAB;
    }
    
    if (m_dataExporter->exportWaveforms(filename, waveforms, format)) {
        m_footerWidget->setTriggerStatus("Exported");
    } else {
        QMessageBox::warning(this, tr("Export Failed"),
            tr("Failed to export data to file."));
    }
}

void MainWindow::onTakeScreenshot()
{
    QPixmap screenshot = m_analogDisplay->grab();
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Screenshot"),
        m_lastScreenshotPath, tr("PNG Files (*.png);;JPEG Files (*.jpg)"));
    
    if (!filename.isEmpty()) {
        screenshot.save(filename);
        m_lastScreenshotPath = QFileInfo(filename).absolutePath();
        m_footerWidget->setTriggerStatus("Screenshot saved");
    }
}

void MainWindow::onOpenFFT()
{
    if (!m_fftWindow) {
        m_fftWindow = new FFTWindow(this);
    }

    ScopeChannel *sourceChannel = nullptr;
    for (auto *channel : m_channels) {
        if (channel->isEnabled()) {
            sourceChannel = channel;
            break;
        }
    }

    if (sourceChannel) {
        m_fftWindow->setSourceChannel(sourceChannel);
    }

    m_fftWindow->show();
    m_fftWindow->raise();
    m_fftWindow->activateWindow();
}

void MainWindow::onOpenProtocolDecoder()
{
    QMessageBox::information(this, tr("Protocol Decoder"),
        tr("Protocol Decoder feature coming soon."));
}

void MainWindow::onOpenMathChannel()
{
    QMessageBox::information(this, tr("Math Channel"),
        tr("Math Channel feature coming soon."));
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, tr("About RIGOL Oscilloscope Pro"),
        tr("<h2>RIGOL Oscilloscope Pro</h2>"
           "<p>Version 2.0.0</p>"
           "<p>A professional-grade oscilloscope control application for RIGOL MSO/DS series.</p>"
           "<p><b>Features:</b></p>"
           "<ul>"
           "<li>Real-time waveform display with OpenGL rendering</li>"
           "<li>Four-channel support with independent controls</li>"
           "<li>Advanced triggering modes</li>"
           "<li>Automated measurements and statistics</li>"
           "<li>FFT spectrum analysis</li>"
           "<li>Protocol decoding (I2C, SPI, UART)</li>"
           "<li>Data export (CSV, MATLAB, WAV)</li>"
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
        // Update FPS in footer
        m_footerWidget->setFPS(m_analogDisplay->framesPerSecond());
        
        if (m_isRunning) {
            m_footerWidget->setAcquisitionProgress(
                (m_acquisitionCount % 100));
        }
    }
}

void MainWindow::onWaveformUpdated(const QString &channel)
{
    Q_UNUSED(channel)
    m_analogDisplay->update();
    if (m_digitalDisplay->isVisible()) {
        m_digitalDisplay->update();
    }
}

void MainWindow::onMeasurementsUpdated()
{
    m_digitalPanel->refresh();
}

void MainWindow::toggleChannel(const QString &channelName)
{
    if (!m_channels.contains(channelName)) return;
    
    ScopeChannel *channel = m_channels[channelName];
    bool enabled = !channel->isEnabled();
    channel->setEnabled(enabled);
    
    if (channelName == "CH1") m_channel1Action->setChecked(enabled);
    else if (channelName == "CH2") m_channel2Action->setChecked(enabled);
    else if (channelName == "CH3") m_channel3Action->setChecked(enabled);
    else if (channelName == "CH4") m_channel4Action->setChecked(enabled);
    
    if (m_connection && m_connection->isConnected()) {
        m_connection->sendCommand(SCPI::channelDisplay(channelName, enabled));
    }
    
    m_analogDisplay->update();
    m_measurementEngine->updateMeasurements(m_channels);
}

void MainWindow::onRunModeChanged(const QString &mode)
{
    if (m_scopeSettings) {
        if (mode == "Auto") {
            m_scopeSettings->setTriggerMode(ScopeSettings::TriggerMode::Auto);
        } else if (mode == "Normal") {
            m_scopeSettings->setTriggerMode(ScopeSettings::TriggerMode::Normal);
        } else if (mode == "Single") {
            m_scopeSettings->setTriggerMode(ScopeSettings::TriggerMode::Single);
        }
        
        if (m_connection && m_connection->isConnected()) {
            m_connection->sendCommand(SCPI::triggerMode(mode.toUpper()));
        }
    }
}

void MainWindow::onTimebaseChanged(double timePerDiv)
{
    m_analogDisplay->setTimePerDiv(timePerDiv);
    
    if (m_connection && m_connection->isConnected()) {
        m_connection->sendCommand(SCPI::timebaseScale(timePerDiv));
    }
}

void MainWindow::onChannelSettingsChanged(const QString &channel)
{
    Q_UNUSED(channel)
    m_analogDisplay->update();
}

void MainWindow::onTriggerLevelChanged(double level)
{
    m_analogDisplay->setTriggerLevel(level);
}

void MainWindow::onMeasurementAdded(const QString &type, const QString &channel)
{
    // Convert type string to enum
    MeasurementType measType = MeasurementType::Vpp;
    
    if (type == "Vpp") measType = MeasurementType::Vpp;
    else if (type == "Vmax") measType = MeasurementType::Vmax;
    else if (type == "Vmin") measType = MeasurementType::Vmin;
    else if (type == "Vavg") measType = MeasurementType::Vavg;
    else if (type == "Vrms") measType = MeasurementType::Vrms;
    else if (type == "Frequency") measType = MeasurementType::Frequency;
    else if (type == "Period") measType = MeasurementType::Period;
    else if (type == "Rise Time") measType = MeasurementType::RiseTime;
    else if (type == "Fall Time") measType = MeasurementType::FallTime;
    else if (type == "Duty Cycle") measType = MeasurementType::DutyCycle;
    
    m_measurementEngine->addMeasurement(measType, channel);
    m_measurementEngine->updateMeasurements(m_channels);
}
