/**
 * @file mainwindow.h
 * @brief Modern professional main window for RIGOL Oscilloscope GUI
 * 
 * Layout structure:
 * ┌──────────────────────────────────────────────────────────────────┐
 * │                         HEADER WIDGET                            │
 * │  [Logo/Brand] [Device Info] [System Status] [Global Controls]    │
 * ├─────────┬────────────────────────────────────────────┬───────────┤
 * │         │              CENTER SECTION                │           │
 * │  ANALOG │  ┌──────────────────────────────────────┐  │  DIGITAL  │
 * │ CONTROLS│  │       ANALOG WAVEFORM DISPLAY        │  │ CONTROLS  │
 * │  (LEFT) │  │         (Flexible Width)             │  │  (RIGHT)  │
 * │         │  ├──────────────────────────────────────┤  │           │
 * │ min/max │  │       DIGITAL/FFT DISPLAY            │  │  min/max  │
 * │  width  │  │         (Optional)                   │  │   width   │
 * │         │  └──────────────────────────────────────┘  │           │
 * ├─────────┴────────────────────────────────────────────┴───────────┤
 * │                         FOOTER WIDGET                            │
 * │  [Channel Readings] [Trigger Status] [Acq Progress] [Performance]│
 * └──────────────────────────────────────────────────────────────────┘
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QSettings>
#include <QKeyEvent>
#include <memory>

class DeviceConnection;
class WaveformProvider;
class ScopeSettings;
class ScopeChannel;
class MeasurementEngine;
class CursorManager;
class WaveformDisplay;
class HeaderWidget;
class FooterWidget;
class AnalogControlPanel;
class DigitalControlPanel;
class ConnectionDialog;
class FFTWindow;
class ProtocolDecoder;
class DataExporter;

/**
 * @brief Modern professional main application window for RIGOL Oscilloscope GUI
 *
 * The MainWindow class serves as the primary interface for the RIGOL Oscilloscope GUI application.
 * It provides a comprehensive oscilloscope control interface with real-time waveform display,
 * channel management, measurement tools, and data export capabilities. The window features
 * a modern dark-themed UI with dockable panels, customizable layouts, and professional-grade
 * controls suitable for engineering and research applications.
 *
 * Key features include:
 * - Multi-channel waveform display with OpenGL acceleration
 * - Real-time data acquisition and control
 * - Advanced triggering and measurement capabilities
 * - FFT analysis and protocol decoding
 * - Data export in multiple formats
 * - Customizable UI with dark theme
 * - Keyboard shortcuts and menu system
 *
 * The window layout consists of a header widget for global controls, a central waveform
 * display area with left and right control panels, and a footer widget for status information.
 * All components are designed to work seamlessly together for an efficient workflow.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the main window with all necessary components
     *
     * Initializes the main window by creating all UI components, setting up the layout,
     * creating menus and actions, establishing signal-slot connections, and loading
     * user settings. The constructor sets up the complete application interface
     * ready for oscilloscope operation.
     *
     * @param parent Parent widget for this window, defaults to nullptr for top-level window
     */
    explicit MainWindow(QWidget *parent = nullptr);
    /**
     * @brief Destroys the main window and cleans up all resources
     *
     * Saves current settings, disconnects from the device if connected,
     * and properly cleans up all dynamically allocated components including
     * core engine objects, UI widgets, and timers.
     */
    ~MainWindow() override;

protected:
    /**
     * @brief Handles window close events
     *
     * Prompts user to save unsaved data if necessary, saves window settings,
     * and ensures proper cleanup before closing the application.
     *
     * @param event The close event to handle
     */
    void closeEvent(QCloseEvent *event) override;

    /**
     * @brief Handles window show events
     *
     * Performs initialization tasks that require the window to be visible,
     * such as updating UI state and starting background timers.
     *
     * @param event The show event to handle
     */
    void showEvent(QShowEvent *event) override;

    /**
     * @brief Handles key press events for keyboard shortcuts
     *
     * Processes keyboard input for application shortcuts and hotkeys,
     * providing quick access to common functions like run/stop, channel toggle, etc.
     *
     * @param event The key press event to handle
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * @brief Handles window resize events
     *
     * Adjusts the layout and component sizes when the window is resized,
     * ensuring proper proportions and usability at different window sizes.
     *
     * @param event The resize event to handle
     */
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // Connection
    /**
     * @brief Initiates device connection process
     *
     * Opens the connection dialog to allow user to select and connect
     * to a RIGOL oscilloscope device via USB, Ethernet, or other interfaces.
     */
    void onConnectAction();

    /**
     * @brief Disconnects from the currently connected device
     *
     * Safely disconnects from the oscilloscope, stops any ongoing acquisition,
     * and updates the UI to reflect the disconnected state.
     */
    void onDisconnectAction();

    /**
     * @brief Handles changes in connection state
     *
     * Updates UI elements based on connection status changes, such as
     * enabling/disabling controls and updating status indicators.
     *
     * @param state The new connection state (connected, disconnected, error, etc.)
     */
    void onConnectionStateChanged(int state);
    
    // Acquisition control
    /**
     * @brief Toggles between run and stop acquisition modes
     *
     * Starts or stops continuous waveform acquisition. When running,
     * continuously acquires and displays waveform data from enabled channels.
     */
    void onRunStop();

    /**
     * @brief Performs a single acquisition cycle
     *
     * Acquires one complete waveform from all enabled channels and
     * updates the display, then stops acquisition.
     */
    void onSingle();

    /**
     * @brief Automatically scales all enabled channels
     *
     * Adjusts vertical scale and offset for each channel to optimally
     * display the current waveform signals within the display area.
     */
    void onAutoScale();

    /**
     * @brief Forces a trigger event
     *
     * Manually triggers acquisition regardless of trigger conditions,
     * useful for capturing signals that don't meet normal trigger criteria.
     */
    void onForceTrigger();

    /**
     * @brief Clears all waveform data and measurements
     *
     * Resets the display to show no waveforms and clears all measurement
     * results, providing a clean slate for new acquisitions.
     */
    void onClear();
    
    // View actions
    /**
     * @brief Toggles fullscreen mode
     *
     * Switches between normal windowed mode and fullscreen display,
     * maximizing the waveform viewing area for detailed analysis.
     */
    void onToggleFullScreen();

    /**
     * @brief Toggles between light and dark themes
     *
     * Switches the application theme between light and dark modes,
     * affecting all UI elements and providing user preference options.
     */
    void onToggleDarkMode();

    /**
     * @brief Resets the window layout to default
     *
     * Restores all dockable panels and splitters to their default positions
     * and sizes, useful when the layout becomes disorganized.
     */
    void onResetLayout();

    /**
     * @brief Toggles visibility of the left control panel
     *
     * Shows or hides the analog control panel on the left side,
     * allowing more space for waveform display when needed.
     */
    void onToggleLeftPanel();

    /**
     * @brief Toggles visibility of the right control panel
     *
     * Shows or hides the digital control panel on the right side,
     * providing flexible layout options for different workflows.
     */
    void onToggleRightPanel();

    /**
     * @brief Toggles FFT display mode
     *
     * Switches between time-domain waveform display and frequency-domain
     * FFT spectrum display for spectral analysis of signals.
     */
    void onToggleFFTDisplay();
    
    // Tools
    /**
     * @brief Exports waveform data to file
     *
     * Opens a dialog for selecting export format (CSV, MATLAB, WAV, etc.)
     * and location, then exports current waveform data for external analysis.
     */
    void onExportData();

    /**
     * @brief Captures and saves a screenshot of the current display
     *
     * Takes a screenshot of the entire application window or just the
     * waveform display area and saves it as an image file.
     */
    void onTakeScreenshot();

    /**
     * @brief Opens the FFT analysis window
     *
     * Launches a dedicated FFT analysis window for detailed frequency-domain
     * analysis of waveform signals with various windowing options.
     */
    void onOpenFFT();

    /**
     * @brief Opens the protocol decoder window
     *
     * Launches the protocol decoding interface for analyzing digital
     * signals and decoding protocols like I2C, SPI, UART, etc.
     */
    void onOpenProtocolDecoder();

    /**
     * @brief Opens the math channel configuration
     *
     * Provides interface for creating mathematical operations on channels,
     * such as addition, subtraction, multiplication of waveforms.
     */
    void onOpenMathChannel();
    
    // Help
    /**
     * @brief Shows the about dialog
     *
     * Displays application information including version, copyright,
     * and credits for the RIGOL Oscilloscope GUI software.
     */
    void onAbout();

    /**
     * @brief Checks for software updates
     *
     * Connects to update server to check for newer versions of the
     * application and provides download/installation options if available.
     */
    void onCheckForUpdates();
    
    // Status updates
    /**
     * @brief Handles receipt of device identification information
     *
     * Processes the device ID string received from the connected oscilloscope
     * and updates UI elements to reflect the specific device model and capabilities.
     *
     * @param id The device identification string from the oscilloscope
     */
    void onDeviceIdReceived(const QString &id);

    /**
     * @brief Updates the status bar with current information
     *
     * Refreshes all status bar indicators including connection status,
     * acquisition state, trigger status, and performance metrics.
     */
    void updateStatusBar();

    /**
     * @brief Handles waveform update notifications
     *
     * Processes notifications when new waveform data is available for a channel,
     * triggering display updates and measurement recalculations.
     *
     * @param channel The name of the channel that was updated
     */
    void onWaveformUpdated(const QString &channel);

    /**
     * @brief Handles measurement update notifications
     *
     * Updates the measurement display when new measurement results are calculated,
     * ensuring real-time display of signal parameters.
     */
    void onMeasurementsUpdated();

    /**
     * @brief Toggles the enabled state of a channel
     *
     * Enables or disables waveform acquisition and display for the specified channel,
     * updating both the hardware settings and UI indicators.
     *
     * @param channelName The name of the channel to toggle (e.g., "CH1", "CH2")
     */
    void toggleChannel(const QString &channelName);
    
    // Header signals
    /**
     * @brief Handles changes in acquisition run mode
     *
     * Updates the acquisition system when the user changes between
     * different run modes (continuous, single shot, etc.).
     *
     * @param mode The new run mode selected by the user
     */
    void onRunModeChanged(const QString &mode);

    /**
     * @brief Handles timebase changes from the header
     *
     * Updates the oscilloscope timebase setting when the user adjusts
     * the time per division in the header controls.
     *
     * @param timePerDiv The new time per division value in seconds
     */
    void onTimebaseChanged(double timePerDiv);
    
    // Panel signals
    /**
     * @brief Handles channel settings changes
     *
     * Processes updates to channel-specific settings such as vertical scale,
     * offset, coupling, and probe attenuation from the control panels.
     *
     * @param channel The name of the channel whose settings changed
     */
    void onChannelSettingsChanged(const QString &channel);

    /**
     * @brief Handles trigger level changes
     *
     * Updates the trigger level setting when the user adjusts the trigger
     * threshold in the trigger control panel.
     *
     * @param level The new trigger level value
     */
    void onTriggerLevelChanged(double level);

    /**
     * @brief Handles addition of new measurements
     *
     * Processes requests to add new measurement types to the measurement
     * engine for the specified channel.
     *
     * @param type The type of measurement to add (e.g., "frequency", "amplitude")
     * @param channel The channel to measure
     */
    void onMeasurementAdded(const QString &type, const QString &channel);

private:
    /**
     * @brief Creates all menu actions
     *
     * Initializes all QAction objects for menus and toolbars,
     * setting up their text, icons, shortcuts, and initial states.
     */
    void createActions();

    /**
     * @brief Creates and organizes the menu bar structure
     *
     * Builds the main menu bar with File, View, Acquisition, Tools,
     * and Help menus, populating them with the created actions.
     */
    void createMenus();

    /**
     * @brief Creates the central widget layout
     *
     * Sets up the main layout structure with header, central content area,
     * and footer, including splitters for resizable panels.
     */
    void createCentralLayout();

    /**
     * @brief Creates keyboard shortcuts
     *
     * Registers global keyboard shortcuts for common operations like
     * run/stop, channel toggle, and fullscreen mode.
     */
    void createShortcuts();

    /**
     * @brief Establishes signal-slot connections
     *
     * Connects all UI components, core engines, and external interfaces
     * through Qt's signal-slot mechanism for proper event handling.
     */
    void setupConnections();

    /**
     * @brief Loads user settings from persistent storage
     *
     * Restores window geometry, panel visibility, theme preferences,
     * and other user-configurable settings from QSettings.
     */
    void loadSettings();

    /**
     * @brief Saves current settings to persistent storage
     *
     * Stores current window state, user preferences, and configuration
     * to QSettings for restoration on next application launch.
     */
    void saveSettings();

    /**
     * @brief Updates UI elements based on current state
     *
     * Refreshes the enabled/disabled state of controls, menu items,
     * and other UI elements based on connection status and acquisition state.
     */
    void updateUIState();
    
    // Core components
    /// @brief Manages device connection and communication
    std::unique_ptr<DeviceConnection> m_connection;

    /// @brief Handles waveform data acquisition and processing
    std::unique_ptr<WaveformProvider> m_waveformProvider;

    /// @brief Stores global oscilloscope settings and configuration
    std::unique_ptr<ScopeSettings> m_scopeSettings;

    /// @brief Performs automated measurements on waveform data
    std::unique_ptr<MeasurementEngine> m_measurementEngine;

    /// @brief Manages cursor positioning and measurements
    std::unique_ptr<CursorManager> m_cursorManager;

    /// @brief Handles protocol decoding for digital signals
    std::unique_ptr<ProtocolDecoder> m_protocolDecoder;

    /// @brief Manages data export operations
    std::unique_ptr<DataExporter> m_dataExporter;
    
    // Channels
    /// @brief Map of channel names to ScopeChannel objects for all four channels
    QMap<QString, ScopeChannel*> m_channels;
    
    /// @brief Pointer to the FFT analysis window (nullptr when closed)
    FFTWindow *m_fftWindow = nullptr;
    
    // === Modern UI Components ===
    // Header & Footer
    /// @brief Header widget containing global controls and status
    HeaderWidget *m_headerWidget;

    /// @brief Footer widget displaying channel readings and status
    FooterWidget *m_footerWidget;
    
    // Control Panels
    /// @brief Left panel with analog channel controls
    AnalogControlPanel *m_analogPanel;

    /// @brief Right panel with digital controls and settings
    DigitalControlPanel *m_digitalPanel;
    
    // Waveform Displays
    /// @brief Main analog waveform display widget
    WaveformDisplay *m_analogDisplay;

    /// @brief Digital/FFT display widget (optional)
    WaveformDisplay *m_digitalDisplay;  // For FFT/digital signals
    
    // Layout management
    /// @brief Central container widget holding the main layout
    QWidget *m_centralContainer;

    /// @brief Main vertical layout for header, content, footer
    QVBoxLayout *m_mainLayout;

    /// @brief Horizontal layout for left panel, center, right panel
    QHBoxLayout *m_contentLayout;

    /// @brief Horizontal splitter between left panel and center
    QSplitter *m_horizontalSplitter;

    /// @brief Vertical splitter for analog/digital displays
    QSplitter *m_verticalSplitter;

    /// @brief Center widget containing the waveform displays
    QWidget *m_centerWidget;
    
    // Menus
    /// @brief File menu for connection and data operations
    QMenu *m_fileMenu;

    /// @brief View menu for layout and display options
    QMenu *m_viewMenu;

    /// @brief Acquisition menu for data capture controls
    QMenu *m_acquisitionMenu;

    /// @brief Measure menu for measurement tools
    QMenu *m_measureMenu;

    /// @brief Tools menu for analysis and export tools
    QMenu *m_toolsMenu;

    /// @brief Help menu for documentation and about
    QMenu *m_helpMenu;
    
    // Actions - File
    /// @brief Action for connecting to a device
    QAction *m_connectAction;

    /// @brief Action for disconnecting from device
    QAction *m_disconnectAction;

    /// @brief Action for exporting waveform data
    QAction *m_exportAction;

    /// @brief Action for taking screenshots
    QAction *m_screenshotAction;

    /// @brief Action for exiting the application
    QAction *m_exitAction;
    
    // Actions - View
    /// @brief Action for toggling fullscreen mode
    QAction *m_fullScreenAction;

    /// @brief Action for toggling dark/light theme
    QAction *m_darkModeAction;

    /// @brief Action for resetting window layout
    QAction *m_resetLayoutAction;

    /// @brief Action for toggling left panel visibility
    QAction *m_toggleLeftPanelAction;

    /// @brief Action for toggling right panel visibility
    QAction *m_toggleRightPanelAction;

    /// @brief Action for toggling FFT display
    QAction *m_toggleFFTDisplayAction;
    
    // Actions - Channels
    /// @brief Action for toggling channel 1
    QAction *m_channel1Action;

    /// @brief Action for toggling channel 2
    QAction *m_channel2Action;

    /// @brief Action for toggling channel 3
    QAction *m_channel3Action;

    /// @brief Action for toggling channel 4
    QAction *m_channel4Action;
    
    // Actions - Acquisition
    /// @brief Action for run/stop acquisition toggle
    QAction *m_runStopAction;

    /// @brief Action for single acquisition
    QAction *m_singleAction;

    /// @brief Action for auto-scaling channels
    QAction *m_autoScaleAction;

    /// @brief Action for forcing trigger
    QAction *m_forceTriggerAction;

    /// @brief Action for clearing display
    QAction *m_clearAction;
    
    // Actions - Tools
    /// @brief Action for opening FFT window
    QAction *m_fftAction;

    /// @brief Action for opening protocol decoder
    QAction *m_protocolDecoderAction;

    /// @brief Action for opening math channel config
    QAction *m_mathChannelAction;

    /// @brief Action for toggling cursors
    QAction *m_cursorAction;
    
    // Actions - Help
    /// @brief Action for showing about dialog
    QAction *m_aboutAction;

    /// @brief Action for showing Qt about dialog
    QAction *m_aboutQtAction;

    /// @brief Action for checking for updates
    QAction *m_checkUpdatesAction;
    
    // Timers
    /// @brief Timer for periodic status bar updates
    QTimer *m_statusUpdateTimer;

    /// @brief Timer for controlling acquisition intervals
    QTimer *m_acquisitionTimer;
    
    // State
    /// @brief Flag indicating if acquisition is currently running
    bool m_isRunning = false;

    /// @brief Flag indicating if dark theme is active
    bool m_isDarkMode = true;

    /// @brief Flag indicating if FFT display is shown
    bool m_showFFTDisplay = false;

    /// @brief Counter for number of acquisitions performed
    int m_acquisitionCount = 0;

    /// @brief Path to last used export directory
    QString m_lastExportPath;

    /// @brief Path to last used screenshot directory
    QString m_lastScreenshotPath;
    
    // Panel visibility
    /// @brief Flag for left panel visibility state
    bool m_leftPanelVisible = true;

    /// @brief Flag for right panel visibility state
    bool m_rightPanelVisible = true;
};

#endif // MAINWINDOW_H
