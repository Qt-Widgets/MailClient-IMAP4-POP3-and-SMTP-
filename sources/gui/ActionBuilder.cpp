#include "ActionBuilder.h"

ActionBuilder::ActionBuilder()
{
}

void ActionBuilder::CreateActions()
{
    fileExit = new QAction(QIcon(":images/exit.png"), ("Exit"), nullptr);
    fileNewMail = new QAction(QIcon(":images/emailcompose.png"), ("Compose ..."), nullptr);
    fileAddressBook = new QAction(QIcon(":images/addressbook.png"), ("Contacts ..."), nullptr);
    fileProfiles = new QAction(QIcon(":images/profiles.png"), ("Profiles ..."), nullptr);
	fileExpunge = new QAction(QIcon(":images/expunge.png"), ("Expunge ..."), nullptr);

    editReply = new QAction(QIcon(":images/emailreply.png"), ("Reply"), nullptr);
    editReplyAll = new QAction(QIcon(":images/emailreplyall.png"), ("Reply All"), nullptr);
    editForward = new QAction(QIcon(":images/emailforward.png"), ("Forward"), nullptr);
    editDelete = new QAction(QIcon(":images/emaildelete.png"), ("Delete"), nullptr);
    editFlag = new QAction(QIcon(":images/emailflag.png"), ("Flag"), nullptr);

    viewNext = new QAction(QIcon(":images/emailnext.png"), ("Next"), nullptr);
    viewPrevious = new QAction(QIcon(":images/emailprevious.png"), ("Previous"), nullptr);
    viewSearch = new QAction(QIcon(":images/emailsearch.png"), ("Search ..."), nullptr);
    viewRefresh = new QAction(QIcon(":images/refresh.png"), ("Refresh"), nullptr);
    viewFilter = new QAction(QIcon(":images/emailfilter.png"), ("Filter"), nullptr);
}

void ActionBuilder::PopulateToolbar(QToolBar &tbar)
{
    searchInput.setMaximumWidth(250);

    tbar.addAction(fileNewMail);

    tbar.addSeparator();
    tbar.addAction(editReply);
    tbar.addAction(editReplyAll);
    tbar.addAction(editForward);
	tbar.addAction(editDelete);
	tbar.addAction(editFlag);

	tbar.addSeparator();
	tbar.addAction(viewNext);
	tbar.addAction(viewPrevious);

	tbar.addSeparator();
    tbar.addWidget(&searchInput);
	tbar.addAction(viewSearch);

	tbar.addSeparator();
    tbar.addAction(viewFilter);
	tbar.addAction(viewRefresh);
	tbar.addAction(fileExpunge);

	tbar.addSeparator();
	tbar.addAction(fileAddressBook);
	tbar.addSeparator();
	tbar.addAction(fileExit);

    tbar.setMovable(false);
    tbar.setToolButtonStyle(Qt::ToolButtonIconOnly);

    QPalette pal = tbar.palette();
    pal.setColor(QPalette::Window, Qt::white);
    tbar.setAutoFillBackground(true);
    tbar.setPalette(pal);
    tbar.setIconSize(QSize(32,32));
}

