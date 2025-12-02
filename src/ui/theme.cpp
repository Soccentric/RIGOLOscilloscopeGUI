#include "theme.h"
#include <QDebug>
#include <QStyleFactory>

void ThemeManager::applyClassicTheme(QApplication &app) {
  // Try to set Windows style if available, otherwise Fusion
  if (QStyleFactory::keys().contains("Windows")) {
    app.setStyle(QStyleFactory::create("Windows"));
  } else {
    app.setStyle(QStyleFactory::create("Fusion"));
  }

  app.setPalette(createClassicPalette());
  app.setStyleSheet(getGlobalStyleSheet());
  app.setFont(getSystemFont());
}

QPalette ThemeManager::createClassicPalette() {
  QPalette palette;

  // Standard Windows 95/98/2000 Gray Scheme
  QColor silver(192, 192, 192);
  QColor white(255, 255, 255);
  QColor black(0, 0, 0);
  QColor gray(128, 128, 128);
  QColor navy(0, 0, 128);

  palette.setColor(QPalette::Window, silver);
  palette.setColor(QPalette::WindowText, black);
  palette.setColor(QPalette::Base, white);
  palette.setColor(QPalette::AlternateBase, silver);
  palette.setColor(QPalette::Text, black);
  palette.setColor(QPalette::Button, silver);
  palette.setColor(QPalette::ButtonText, black);
  palette.setColor(QPalette::BrightText, white);

  // Classic Highlight
  palette.setColor(QPalette::Highlight, navy);
  palette.setColor(QPalette::HighlightedText, white);

  // 3D Effects
  palette.setColor(QPalette::Light, white);
  palette.setColor(QPalette::Midlight, QColor(223, 223, 223)); // 3D Light
  palette.setColor(QPalette::Dark, gray);
  palette.setColor(QPalette::Mid, QColor(160, 160, 160));
  palette.setColor(QPalette::Shadow, black);

  // Disabled state
  palette.setColor(QPalette::Disabled, QPalette::Text, gray);
  palette.setColor(QPalette::Disabled, QPalette::ButtonText, gray);
  palette.setColor(QPalette::Disabled, QPalette::WindowText, gray);

  return palette;
}

QString ThemeManager::getGlobalStyleSheet() {
  return R"(
        /* Global Reset */
        * {
            selection-background-color: #000080;
            selection-color: #ffffff;
        }

        /* Main Window & Dialogs */
        QMainWindow, QDialog {
            background-color: #c0c0c0;
        }

        /* GroupBox - Classic Etched Border */
        QGroupBox {
            border: 2px solid gray;
            border-radius: 0px;
            margin-top: 10px;
            background-color: #c0c0c0;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 3px;
            background-color: #c0c0c0;
            color: black;
        }

        /* Buttons - Classic 3D Look */
        QPushButton {
            background-color: #c0c0c0;
            border: 2px solid #808080;
            border-top-color: #ffffff;
            border-left-color: #ffffff;
            border-right-color: #404040;
            border-bottom-color: #404040;
            padding: 4px 12px;
            min-height: 20px;
        }

        QPushButton:pressed {
            border: 2px solid #808080;
            border-top-color: #404040;
            border-left-color: #404040;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            padding: 5px 11px 3px 13px; /* Shift text for press effect */
        }
        
        QPushButton:checked {
            background-color: #e0e0e0;
            border: 2px inset #808080;
        }

        /* Inputs - Sunken 3D Look */
        QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {
            background-color: white;
            border: 2px solid #808080;
            border-top-color: #404040;
            border-left-color: #404040;
            border-right-color: #ffffff;
            border-bottom-color: #ffffff;
            padding: 2px;
            selection-background-color: #000080;
        }

        /* Combo Box Dropdown */
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 16px;
            border-left-width: 1px;
            border-left-color: darkgray;
            border-left-style: solid;
            border-top-right-radius: 3px;
            border-bottom-right-radius: 3px;
            background: #c0c0c0;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 5px solid black;
            margin-top: 2px;
        }

        /* Menu Bar */
        QMenuBar {
            background-color: #c0c0c0;
            border-bottom: 1px solid #808080;
        }
        
        QMenuBar::item {
            spacing: 3px;
            padding: 2px 6px;
            background: transparent;
        }
        
        QMenuBar::item:selected {
            background: #000080;
            color: white;
        }
        
        QMenu {
            background-color: #c0c0c0;
            border: 2px outset #c0c0c0;
        }
        
        QMenu::item {
            padding: 4px 20px 4px 20px;
        }
        
        QMenu::item:selected {
            background: #000080;
            color: white;
        }

        /* Splitter - Classic Grip */
        QSplitter::handle {
            background-color: #c0c0c0;
            border: 1px solid #808080;
        }
        
        /* Scrollbars */
        QScrollBar:vertical {
            border: 1px solid #808080;
            background: #e0e0e0; /* Dotted pattern simulation */
            width: 16px;
            margin: 16px 0 16px 0;
        }
        
        QScrollBar::handle:vertical {
            background: #c0c0c0;
            border: 2px outset #c0c0c0;
            min-height: 20px;
        }
        
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            border: 2px outset #c0c0c0;
            background: #c0c0c0;
            height: 16px;
            subcontrol-origin: margin;
        }

        QScrollBar::add-line:vertical {
            subcontrol-position: bottom;
        }

        QScrollBar::sub-line:vertical {
            subcontrol-position: top;
        }
        
        /* Tab Widget */
        QTabWidget::pane {
            border: 2px outset #c0c0c0;
            background-color: #c0c0c0;
        }
        
        QTabBar::tab {
            background: #c0c0c0;
            border: 2px outset #c0c0c0;
            padding: 4px 12px;
            margin-right: 2px;
        }
        
        QTabBar::tab:selected {
            background: #c0c0c0;
            border-bottom-color: #c0c0c0; /* Blend with pane */
            font-weight: bold;
        }
    )";
}

QFont ThemeManager::getSystemFont() {
  // Classic Tahoma or MS Sans Serif look
  return QFont("Tahoma", 9);
}

QFont ThemeManager::getMonospaceFont() { return QFont("Courier New", 10); }
