/**
 * RIGOL Oscilloscope GUI - Main Entry Point
 * 
 * A professional-grade oscilloscope control application for RIGOL
 * MSO/DS series oscilloscopes. Features include:
 * - Real-time waveform display with OpenGL rendering
 * - Four-channel support with independent controls
 * - Advanced triggering modes
 * - Automated measurements and statistics
 * - FFT spectrum analysis
 * - Protocol decoding (I2C, SPI, UART)
 * - Data export (CSV, MATLAB, WAV, etc.)
 * 
 * @author RIGOL Oscilloscope GUI Team
 * @version 2.0.0
 */

#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QDebug>
#include <QSplashScreen>
#include <QTimer>
#include <QFontDatabase>
#include <QPainter>

#include "ui/mainwindow.h"

/**
 * @brief Applies a comprehensive dark theme to the Qt application
 *
 * This function configures the Fusion style and sets up a complete dark color palette
 * for all Qt widgets, including custom stylesheets for enhanced visual consistency.
 * The theme is designed to provide excellent readability and a professional appearance
 * suitable for technical applications like oscilloscope control software.
 *
 * @param app Reference to the QApplication instance that will use the dark theme
 */
void applyDarkTheme(QApplication &app)
{
    // Fusion style works well with dark themes
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Dark palette
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(45, 45, 48));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(30, 30, 30));
    darkPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 48));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(45, 45, 48));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
    
    app.setPalette(darkPalette);
    
    // Additional stylesheet for fine-tuning
    QString styleSheet = R"(
        QMainWindow {
            background-color: #2d2d30;
        }
        
        QDockWidget {
            titlebar-close-icon: url(:/icons/close.png);
            titlebar-normal-icon: url(:/icons/float.png);
        }
        
        QDockWidget::title {
            background: #3e3e42;
            padding: 6px;
            border: 1px solid #555;
        }
        
        QMenuBar {
            background-color: #2d2d30;
            border-bottom: 1px solid #3e3e42;
        }
        
        QMenuBar::item:selected {
            background-color: #3e3e42;
        }
        
        QMenu {
            background-color: #2d2d30;
            border: 1px solid #3e3e42;
        }
        
        QMenu::item:selected {
            background-color: #3e3e42;
        }
        
        QToolBar {
            background-color: #2d2d30;
            border: none;
            spacing: 3px;
        }
        
        QToolButton {
            background-color: transparent;
            border: 1px solid transparent;
            border-radius: 3px;
            padding: 3px;
        }
        
        QToolButton:hover {
            background-color: #3e3e42;
            border: 1px solid #555;
        }
        
        QToolButton:pressed {
            background-color: #505050;
        }
        
        QPushButton {
            background-color: #3e3e42;
            border: 1px solid #555;
            border-radius: 3px;
            padding: 5px 15px;
            min-width: 80px;
        }
        
        QPushButton:hover {
            background-color: #505050;
            border: 1px solid #666;
        }
        
        QPushButton:pressed {
            background-color: #404040;
        }
        
        QPushButton:disabled {
            background-color: #2d2d30;
            color: #666;
        }
        
        QComboBox {
            background-color: #3e3e42;
            border: 1px solid #555;
            border-radius: 3px;
            padding: 3px 8px;
            min-width: 100px;
        }
        
        QComboBox:hover {
            border: 1px solid #666;
        }
        
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        
        QComboBox QAbstractItemView {
            background-color: #2d2d30;
            border: 1px solid #555;
            selection-background-color: #3e3e42;
        }
        
        QSpinBox, QDoubleSpinBox {
            background-color: #3e3e42;
            border: 1px solid #555;
            border-radius: 3px;
            padding: 3px;
        }
        
        QLineEdit {
            background-color: #3e3e42;
            border: 1px solid #555;
            border-radius: 3px;
            padding: 5px;
        }
        
        QLineEdit:focus {
            border: 1px solid #2a82da;
        }
        
        QSlider::groove:horizontal {
            height: 4px;
            background: #3e3e42;
            border-radius: 2px;
        }
        
        QSlider::handle:horizontal {
            background: #2a82da;
            border: 1px solid #2a82da;
            width: 14px;
            margin: -5px 0;
            border-radius: 7px;
        }
        
        QSlider::handle:horizontal:hover {
            background: #3a92ea;
        }
        
        QTabWidget::pane {
            border: 1px solid #3e3e42;
            background-color: #2d2d30;
        }
        
        QTabBar::tab {
            background-color: #252526;
            border: 1px solid #3e3e42;
            padding: 8px 16px;
        }
        
        QTabBar::tab:selected {
            background-color: #2d2d30;
            border-bottom-color: #2d2d30;
        }
        
        QTabBar::tab:hover {
            background-color: #3e3e42;
        }
        
        QGroupBox {
            border: 1px solid #3e3e42;
            border-radius: 5px;
            margin-top: 10px;
            padding-top: 10px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 5px;
            color: #aaa;
        }
        
        QScrollBar:vertical {
            background: #2d2d30;
            width: 12px;
            margin: 0;
        }
        
        QScrollBar::handle:vertical {
            background: #555;
            min-height: 20px;
            border-radius: 6px;
        }
        
        QScrollBar::handle:vertical:hover {
            background: #666;
        }
        
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0;
        }
        
        QScrollBar:horizontal {
            background: #2d2d30;
            height: 12px;
            margin: 0;
        }
        
        QScrollBar::handle:horizontal {
            background: #555;
            min-width: 20px;
            border-radius: 6px;
        }
        
        QStatusBar {
            background-color: #007acc;
            color: white;
        }
        
        QStatusBar::item {
            border: none;
        }
        
        QTableWidget {
            background-color: #1e1e1e;
            gridline-color: #3e3e42;
            border: 1px solid #3e3e42;
        }
        
        QTableWidget::item {
            padding: 5px;
        }
        
        QTableWidget::item:selected {
            background-color: #3e3e42;
        }
        
        QHeaderView::section {
            background-color: #252526;
            color: white;
            padding: 5px;
            border: 1px solid #3e3e42;
        }
        
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid #555;
            border-radius: 3px;
            background-color: #3e3e42;
        }
        
        QCheckBox::indicator:checked {
            background-color: #2a82da;
            border-color: #2a82da;
        }
        
        QRadioButton::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid #555;
            border-radius: 8px;
            background-color: #3e3e42;
        }
        
        QRadioButton::indicator:checked {
            background-color: #2a82da;
            border: 4px solid #3e3e42;
        }
        
        QProgressBar {
            border: 1px solid #555;
            border-radius: 3px;
            background-color: #3e3e42;
            text-align: center;
        }
        
        QProgressBar::chunk {
            background-color: #2a82da;
            border-radius: 2px;
        }
        
        QToolTip {
            background-color: #1e1e1e;
            color: white;
            border: 1px solid #555;
            padding: 5px;
        }
    )";
    
    app.setStyleSheet(styleSheet);
}

/**
 * @brief Creates and displays a custom splash screen during application startup
 *
 * Generates a professional-looking splash screen with the application name, version,
 * and loading message. The splash screen uses a dark theme consistent with the
 * application's overall design.
 *
 * @return Pointer to the QSplashScreen object that was created and shown
 */
QSplashScreen* showSplashScreen()
{
    QPixmap pixmap(500, 300);
    pixmap.fill(QColor(45, 45, 48));
    
    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    
    // Title
    QFont titleFont("Segoe UI", 24, QFont::Bold);
    painter.setFont(titleFont);
    painter.drawText(pixmap.rect().adjusted(0, 60, 0, 0), 
                     Qt::AlignHCenter | Qt::AlignTop,
                     "RIGOL Oscilloscope GUI");
    
    // Version
    QFont versionFont("Segoe UI", 12);
    painter.setFont(versionFont);
    painter.setPen(QColor(128, 128, 128));
    painter.drawText(pixmap.rect().adjusted(0, 100, 0, 0),
                     Qt::AlignHCenter | Qt::AlignTop,
                     "Version 2.0.0");
    
    // Loading text
    painter.setPen(Qt::white);
    painter.drawText(pixmap.rect().adjusted(0, 0, 0, -30),
                     Qt::AlignHCenter | Qt::AlignBottom,
                     "Loading...");
    
    // Blue accent bar
    painter.fillRect(0, 290, 500, 10, QColor(42, 130, 218));
    
    painter.end();
    
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();
    
    return splash;
}

/**
 * @brief Main entry point for the RIGOL Oscilloscope GUI application
 *
 * Initializes the Qt application, applies the dark theme, shows a splash screen,
 * creates the main window, and starts the event loop. This function handles the
 * complete application lifecycle from startup to shutdown.
 *
 * @param argc Number of command-line arguments passed to the application
 * @param argv Array of command-line argument strings
 * @return Integer exit code (0 for success, non-zero for errors)
 */
int main(int argc, char *argv[])
{
    // Enable high DPI scaling
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    
    QApplication app(argc, argv);
    
    // Application metadata
    app.setOrganizationName("RIGOL");
    app.setOrganizationDomain("rigol.com");
    app.setApplicationName("RIGOL Oscilloscope GUI");
    app.setApplicationVersion("2.0.0");
    
    // Show splash screen
    QSplashScreen *splash = showSplashScreen();
    app.processEvents();
    
    // Apply theme
    splash->showMessage("Applying theme...", Qt::AlignBottom | Qt::AlignHCenter, Qt::white);
    app.processEvents();
    applyDarkTheme(app);
    
    // Initialize main window
    splash->showMessage("Initializing...", Qt::AlignBottom | Qt::AlignHCenter, Qt::white);
    app.processEvents();
    
    MainWindow mainWindow;
    
    // Close splash and show main window
    QTimer::singleShot(1500, [&]() {
        splash->finish(&mainWindow);
        mainWindow.show();
        delete splash;
    });
    
    qDebug() << "RIGOL Oscilloscope GUI started";
    qDebug() << "Qt version:" << qVersion();
    
    return app.exec();
}
