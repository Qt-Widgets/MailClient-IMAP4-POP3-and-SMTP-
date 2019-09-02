#ifndef _ACTION_BUILDER
#define _ACTION_BUILDER

#include <QtCore>
#include <QIcon>
#include <QAction>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QPalette>
#include <QLabel>
#include <QLineEdit>
#include "../../uicommon/ThemeHandler.h"

class ActionBuilder
{
public:
    ActionBuilder();
    void CreateActions();
    void PopulateToolbar(QToolBar &tbar);

protected:
    QAction* fileExit;
    QAction* fileNewMail;
    QAction* fileAddressBook;
    QAction* fileProfiles;
	QAction* fileExpunge;

    QAction* editReply;
    QAction* editReplyAll;
    QAction* editForward;
    QAction* editDelete;
    QAction* editFlag;

    QAction* viewNext;
    QAction* viewPrevious;
    QAction* viewSearch;
    QAction* viewRefresh;
    QAction* viewFilter;

    QLineEdit searchInput;
};

#endif
