#ifndef _TASKS
#define _TASKS

#include <QtCore>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QListWidget>
#include <QPushButton>

#include "../../network/Mail.h"
#include "ThemeHandler.h"

class Tasks : public QWidget
{
    Q_OBJECT
public:
    Tasks(QWidget *parent = 0);
};

#endif
