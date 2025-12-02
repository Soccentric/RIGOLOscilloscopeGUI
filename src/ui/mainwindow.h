#ifndef MAINWINDOW_NEW_H
#define MAINWINDOW_NEW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QTimer>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QSettings>
#include <memory>

class DeviceConnection;
class WaveformProvider;
class ScopeSettings;
class ScopeChannel;
class MeasurementEngine;
class CursorManager;
class WaveformDisplay;
class ChannelControlPanel;
class TriggerPanel;
class MeasurementPanel;
class ConnectionDialog;
class ProtocolDecoder;
class FFTAnalyzer;
class DataExporter;

/**
 * @brief Main application window for RIGOL Oscilloscope GUI
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    // Connection
    void onConnectAction();
    void onDisconnectAction();
    void onConnectionStateChanged(int state);
    
    // Acquisition control
    void onRunStop();
    void onSingle();
    void onAutoScale();
    void onForceTrigger();
    void onClear();
    
    // View actions
    void onToggleFullScreen();
    void onToggleDarkMode();
    void onResetLayout();
    
    // Tools
    void onExportData();
    void onTakeScreenshot();
    void onOpenFFT();
    void onOpenProtocolDecoder();
    void onOpenMathChannel();
    
    // Help
    void onAbout();
    void onCheckForUpdates();
    
    // Status updates
    void onDeviceIdReceived(const QString &id);
    void updateStatusBar();
    void onWaveformUpdated(const QString &channel);
    void onMeasurementsUpdated();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createDockWidgets();
    void createStatusBar();
    void createCentralWidget();
    void setupConnections();
    void loadSettings();
    void saveSettings();
    void applyTheme(bool dark);
    
    // Core components
    std::unique_ptr<DeviceConnection> m_connection;
    std::unique_ptr<WaveformProvider> m_waveformProvider;
    std::unique_ptr<ScopeSettings> m_scopeSettings;
    std::unique_ptr<MeasurementEngine> m_measurementEngine;
    std::unique_ptr<CursorManager> m_cursorManager;
    std::unique_ptr<FFTAnalyzer> m_fftAnalyzer;
    std::unique_ptr<ProtocolDecoder> m_protocolDecoder;
    std::unique_ptr<DataExporter> m_dataExporter;
    
    // Channels
    QMap<QString, ScopeChannel*> m_channels;
    
    // UI Components
    WaveformDisplay *m_waveformDisplay;
    ChannelControlPanel *m_channelPanel;
    TriggerPanel *m_triggerPanel;
    MeasurementPanel *m_measurementPanel;
    
    // Dock widgets
    QDockWidget *m_channelDock;
    QDockWidget *m_triggerDock;
    QDockWidget *m_measurementDock;
    QDockWidget *m_cursorDock;
    QDockWidget *m_protocolDock;
    
    // Toolbars
    QToolBar *m_mainToolBar;
    QToolBar *m_acquisitionToolBar;
    QToolBar *m_measureToolBar;
    
    // Menus
    QMenu *m_fileMenu;
    QMenu *m_viewMenu;
    QMenu *m_acquisitionMenu;
    QMenu *m_measureMenu;
    QMenu *m_toolsMenu;
    QMenu *m_helpMenu;
    
    // Actions - File
    QAction *m_connectAction;
    QAction *m_disconnectAction;
    QAction *m_exportAction;
    QAction *m_screenshotAction;
    QAction *m_exitAction;
    
    // Actions - View
    QAction *m_fullScreenAction;
    QAction *m_darkModeAction;
    QAction *m_resetLayoutAction;
    QAction *m_showChannelPanelAction;
    QAction *m_showTriggerPanelAction;
    QAction *m_showMeasurementPanelAction;
    
    // Actions - Acquisition
    QAction *m_runStopAction;
    QAction *m_singleAction;
    QAction *m_autoScaleAction;
    QAction *m_forceTriggerAction;
    QAction *m_clearAction;
    
    // Actions - Tools
    QAction *m_fftAction;
    QAction *m_protocolDecoderAction;
    QAction *m_mathChannelAction;
    QAction *m_cursorAction;
    
    // Actions - Help
    QAction *m_aboutAction;
    QAction *m_aboutQtAction;
    QAction *m_checkUpdatesAction;
    
    // Status bar widgets
    QLabel *m_connectionStatusLabel;
    QLabel *m_deviceIdLabel;
    QLabel *m_sampleRateLabel;
    QLabel *m_memoryDepthLabel;
    QLabel *m_triggerStatusLabel;
    
    // Timers
    QTimer *m_statusUpdateTimer;
    
    // State
    bool m_isRunning = false;
    bool m_isDarkMode = true;
    QString m_lastExportPath;
    QString m_lastScreenshotPath;
};

#endif // MAINWINDOW_NEW_H
