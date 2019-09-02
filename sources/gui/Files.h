#ifndef _FILES
#define _FILES

#include <QtCore>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QListWidget>
#include <QPushButton>

#include "../../data/Mail.h"
#include "../../uicommon/ThemeHandler.h"

class Files : public QWidget
{
    Q_OBJECT
public:
    Files(QWidget *parent = 0);
};

#endif
