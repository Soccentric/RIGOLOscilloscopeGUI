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
#include <QDebug>
#include <QFile>
#include <QFontDatabase>
#include <QPainter>
#include <QSplashScreen>
#include <QStyleFactory>
#include <QTimer>

#include "ui/mainwindow.h"
#include "ui/theme.h"

/**
 * @brief Creates and displays a custom splash screen during application startup
 *
 * Generates a professional-looking splash screen with the application name,
 * version, and loading message. The splash screen uses a classic theme
 * consistent with the application's overall design.
 *
 * @return Pointer to the QSplashScreen object that was created and shown
 */
QSplashScreen *showSplashScreen() {
  QPixmap pixmap(500, 300);
  pixmap.fill(QColor(192, 192, 192)); // Classic Silver

  QPainter painter(&pixmap);
  painter.setPen(Qt::black);

  // Draw 3D Border
  painter.setPen(Qt::white);
  painter.drawLine(0, 0, 499, 0);
  painter.drawLine(0, 0, 0, 299);
  painter.setPen(QColor(128, 128, 128));
  painter.drawLine(499, 0, 499, 299);
  painter.drawLine(0, 299, 499, 299);

  // Title
  QFont titleFont("Tahoma", 24, QFont::Bold);
  painter.setFont(titleFont);
  painter.setPen(Qt::black);
  painter.drawText(pixmap.rect().adjusted(0, 60, 0, 0),
                   Qt::AlignHCenter | Qt::AlignTop, "RIGOL Oscilloscope GUI");

  // Version
  QFont versionFont("Tahoma", 12);
  painter.setFont(versionFont);
  painter.setPen(QColor(64, 64, 64));
  painter.drawText(pixmap.rect().adjusted(0, 100, 0, 0),
                   Qt::AlignHCenter | Qt::AlignTop,
                   "Version 2.0.0 (Classic Edition)");

  // Loading text
  painter.setPen(Qt::black);
  painter.drawText(pixmap.rect().adjusted(0, 0, 0, -30),
                   Qt::AlignHCenter | Qt::AlignBottom, "Loading...");

  // Blue accent bar (Classic Navy)
  painter.fillRect(2, 290, 496, 8, QColor(0, 0, 128));

  painter.end();

  QSplashScreen *splash = new QSplashScreen(pixmap);
  splash->show();

  return splash;
}

/**
 * @brief Main entry point for the RIGOL Oscilloscope GUI application
 *
 * Initializes the Qt application, applies the classic theme, shows a splash
 * screen, creates the main window, and starts the event loop. This function
 * handles the complete application lifecycle from startup to shutdown.
 *
 * @param argc Number of command-line arguments passed to the application
 * @param argv Array of command-line argument strings
 * @return Integer exit code (0 for success, non-zero for errors)
 */
int main(int argc, char *argv[]) {
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
  splash->showMessage("Applying classic theme...",
                      Qt::AlignBottom | Qt::AlignHCenter, Qt::black);
  app.processEvents();
  ThemeManager::applyClassicTheme(app);

  // Initialize main window
  splash->showMessage("Initializing...", Qt::AlignBottom | Qt::AlignHCenter,
                      Qt::black);
  app.processEvents();

  MainWindow mainWindow;

  // Close splash and show main window
  QTimer::singleShot(1500, [&]() {
    splash->finish(&mainWindow);
    mainWindow.show();
    delete splash;
  });

  qDebug() << "RIGOL Oscilloscope GUI started (Classic Mode)";
  qDebug() << "Qt version:" << qVersion();

  return app.exec();
}
