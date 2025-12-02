#ifndef THEME_H
#define THEME_H

#include <QApplication>
#include <QColor>
#include <QFont>
#include <QPalette>
#include <QString>

class ThemeManager {
public:
  static void applyClassicTheme(QApplication &app);
  static QString getGlobalStyleSheet();
  static QFont getSystemFont();
  static QFont getMonospaceFont();

  // Colors
  static QColor windowColor() {
    return QColor(192, 192, 192);
  } // Standard Silver
  static QColor textColor() { return Qt::black; }
  static QColor baseColor() { return Qt::white; }
  static QColor highlightColor() { return QColor(0, 0, 128); } // Navy Blue
  static QColor highlightTextColor() { return Qt::white; }
  static QColor shadowColor() { return QColor(128, 128, 128); }

private:
  static QPalette createClassicPalette();
};

#endif // THEME_H
