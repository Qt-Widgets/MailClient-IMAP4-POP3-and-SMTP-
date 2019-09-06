#ifndef _THEME_HANDLER
#define _THEME_HANDLER

#include <QWidget>
#include <QLabel>
#include <QMovie>
#include <QPalette>
#include <QFont>
#include <QFontDatabase>

typedef enum ThemeSetting
{
    Light,
    Dark
}ThemeSetting;

void initThemes();
void adjustBackground(QWidget* ptr);
void setAppThemeDark(QApplication *ptr);
void setAppThemeLight(QApplication* ptr);
QPalette currentThemePalette();
QString currentFont();

#endif
