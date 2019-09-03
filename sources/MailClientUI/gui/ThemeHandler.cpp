#include "ThemeHandler.h"
#include <QPalette>
#include <QStyleFactory>
#include <QApplication>

ThemeSetting application_theme = Dark;
QPalette application_palette;
//= QApplication::palette();
QString application_font = "Arial";

void adjustBackground(QWidget* ptr)
{
    QString str = ptr->metaObject()->className();

    QPalette pal = ptr->palette();
    pal.setColor(QPalette::Window, Qt::white);
    ptr->setAutoFillBackground(true);
    ptr->setPalette(pal);
    ptr->setStyleSheet(str + "{background: white;}");
}

void setAppThemeDark(QApplication* ptr)
{
#define FUSION_THEME
#ifdef FUSION_THEME
    ptr->setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    ptr->setPalette(darkPalette);
    ptr->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
#endif
    application_theme = Dark;
    application_palette = darkPalette;
}

void setAppThemeLight(QApplication* ptr)
{
#define FUSION_THEME
#ifdef FUSION_THEME
	ptr->setStyle(QStyleFactory::create("Fusion"));
	QPalette lightPalette;
	lightPalette.setColor(QPalette::Window, QColor(255, 255, 255));
	lightPalette.setColor(QPalette::WindowText, Qt::black);
	lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));
	lightPalette.setColor(QPalette::AlternateBase, QColor(235, 235, 235));
	lightPalette.setColor(QPalette::ToolTipBase, Qt::black);
	lightPalette.setColor(QPalette::ToolTipText, Qt::black);
	lightPalette.setColor(QPalette::Text, Qt::black);
	lightPalette.setColor(QPalette::Button, QColor(255, 255, 255));
	lightPalette.setColor(QPalette::ButtonText, Qt::black);
	lightPalette.setColor(QPalette::BrightText, Qt::red);
	lightPalette.setColor(QPalette::Link, QColor(42, 130, 218));
	lightPalette.setColor(QPalette::Highlight, QColor(15, 122, 253));
	lightPalette.setColor(QPalette::HighlightedText, Qt::white);
	ptr->setPalette(lightPalette);
	ptr->setStyleSheet("QToolTip { color: #000000; background-color: #2a82da; border: 1px solid black; }");
#endif
    application_theme = Light;
    application_palette = lightPalette;
}

QPalette currentThemePalette()
{
    return application_palette;
}

QString currentFont()
{
    return application_font;
}

void initThemes()
{
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    application_theme = Dark;
    application_palette = darkPalette;

    QFontDatabase fontdb;

    QStringList fontlist = fontdb.families();

    foreach(QString str, fontlist)
    {
        if(str.toLower().contains("tahoma"))
        {
            application_font = str;
            break;
        }

        if(str.toLower().contains("liberation") && str.toLower().contains("sans"))
        {
            application_font = str;
            break;
        }

        if(str.toLower().contains("dejavu") && str.toLower().contains("sans"))
        {
            application_font = str;
            break;
        }
    }

    if(application_font.length() < 1)
    {
        application_font = fontlist.at(0);
    }
}

