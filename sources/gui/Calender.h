#ifndef _CALENDER
#define _CALENDER

#include <QtCore>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QListWidget>
#include <QPushButton>
#include <QCalendarWidget>

class Calender : public QWidget
{
    Q_OBJECT
public:
    Calender(QWidget *parent = 0);
	QGridLayout layout;
	QCalendarWidget calender;
};

#endif
