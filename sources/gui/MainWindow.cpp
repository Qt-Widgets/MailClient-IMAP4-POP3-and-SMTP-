#include "MainWindow.h"
#include "HorizontalLine.h"
#include "VerticalLine.h"
#include "../data/MailStorage.h"
#include "../app/MailClient.h"

#include <string>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{  
	initThemes();
	setMinimumHeight(768);
    setMinimumWidth(1024);
    setWindowIcon(QIcon(":images/mailclient.png"));
}

MainWindow::~MainWindow()
{
}

bool MainWindow::Initialize()
{
    //Intialize UI elements
    setStatusBar(&appStatusBar);
    appStatusBar.insertWidget(0, &lblStatusMessage);

    CreateActions();
    PopulateToolbar(appToolBar);
    addToolBar(&appToolBar);

    appCentralWidget.setOrientation(Qt::Horizontal);
	appCentralWidget.setHandleWidth(2);
    appCentralWidget.setFrameStyle(QFrame::Plain);   	
	
    directoryView.setFrameStyle(QFrame::Plain);
    mailBoxView.setFrameStyle(QFrame::Plain);

	setCentralWidget(&appTabs);

    appTabs.addTab(&appCentralWidget, "Mail");
    appTabs.addTab(&calendar, "Calender");
    appTabs.addTab(&pendingTasks, "Pending");
	appTabs.addTab(&profileView, "Profiles");
	appTabs.addTab(&settingsView, "Settings");
	appTabs.addTab(&about, "About");

    //Wire up event handlers
    auto c1 = connect(fileExit, &QAction::triggered, this, &MainWindow::eventExit);
	auto c2 = connect(fileNewMail, &QAction::triggered, this, &MainWindow::eventNewMail);
	auto c3 = connect(fileAddressBook, &QAction::triggered, this, &MainWindow::eventAddressBook);
	auto c4 = connect(editReply, &QAction::triggered, this, &MainWindow::eventReply);
	auto c5 = connect(editReplyAll, &QAction::triggered, this, &MainWindow::eventReplyAll);
	auto c6 = connect(editForward, &QAction::triggered, this, &MainWindow::eventForward);
	auto c7 = connect(editDelete, &QAction::triggered, this, &MainWindow::eventDelete);
	auto c8 = connect(editFlag, &QAction::triggered, this, &MainWindow::eventFlag);
	auto c9 = connect(viewNext, &QAction::triggered, this, &MainWindow::eventNext);
	auto c10 = connect(viewPrevious, &QAction::triggered, this, &MainWindow::eventPrevious);
	auto c11 = connect(viewSearch, &QAction::triggered, this, &MainWindow::eventSearch);
	auto c12 = connect(viewRefresh, &QAction::triggered, this, &MainWindow::eventRefresh);
	auto c13 = connect(fileExpunge, &QAction::triggered, this, &MainWindow::eventExpunge);

	auto c14 = connect(&profileView, &ProfileView::ProfileAdded, this, &MainWindow::eventProfileAdded);
	auto c15 = connect(&profileView, &ProfileView::ProfileUpdated, this, &MainWindow::eventProfileUpdated);
	auto c16 = connect(&profileView, &ProfileView::ProfileRemoved, this, &MainWindow::eventProfileRemoved);
	auto c17 = connect(&mailCompostion, &MailComposer::Finished, this, &MainWindow::eventMailFinished);
	auto c18 = connect(&directoryView, &DirectoryView::MailListRecieved, &mailBoxView, &MailBoxView::eventMailListReceived);
	auto c19 = connect(&mailBoxView, &MailBoxView::MailSelected, this, &MainWindow::eventMailSelected);
	auto c20 = connect(&directoryView, &DirectoryView::ClearMailView, &mailBoxView, &MailBoxView::eventClearMailView);

	addressBook.SetOpsHandler(mailClientPtr);

	SetupViews();

    return true;
}

void MainWindow::SetupViews()
{
    directoryView.Initialize();
	profileView.Initialize();
    mailBoxView.Initialize(&mailView);

	appCentralWidget.addWidget(&directoryView);
	appCentralWidget.addWidget(new VerticalLine());
	appCentralWidget.addWidget(&mailBoxView);
	appCentralWidget.addWidget(new VerticalLine());
	appCentralWidget.addWidget(&clientAreaWidget);

	clientAreaWidget.addWidget(&mailCompostion);	
	clientAreaWidget.addWidget(&mailView);
	clientAreaWidget.setCurrentWidget(&mailView);
}

void MainWindow::eventExit()
{
    close();
}

void MainWindow::eventNewMail()
{
	mailCompostion.Reset();
	mailCompostion.LoadProfiles();
	clientAreaWidget.setCurrentWidget(&mailCompostion);
}

void MainWindow::eventAddressBook()
{
	addressBook.show();
}

void MainWindow::eventMailSelected(MailInfo minfo)
{
	MailStorage ms;

	Mail eml;

	std::string eml_dir;

	eml_dir = mailClientPtr->KeyValue("emaildirectory");

	eml_dir += "/";
	eml_dir += minfo.Storage.GetAccount();
	eml_dir += "/";
	eml_dir += minfo.Storage.GetDirectory();
	eml_dir += "/";

	eml.Header = minfo.Header;

	ms.RetrieveMail(eml_dir, eml);
	eml.DeSerialize();
	mailView.SetEMail(eml);
	clientAreaWidget.setCurrentWidget(&mailView);
}

void MainWindow::eventReply()
{
	extendEmail(Reply);
}

void MainWindow::eventReplyAll()
{
	extendEmail(ReplyAll);
}

void MainWindow::eventForward()
{
	extendEmail(Forward);
}

void MainWindow::extendEmail(CompositionMode mode)
{
	MailInfo token = mailBoxView.GetCurrentToken();
	std::string str;
	Mail eml;
	MailStorage ms;
	std::string eml_dir;

	eml.Header = token.Header;

	if (mode == Forward)
	{
		str += "FW: ";
	}

	if (mode == Reply)
	{
		str += "RE: ";
	}

	if (mode == ReplyAll)
	{
		str += "RE: ";
	}

	str += token.Header.GetSubject();

	eml_dir = mailClientPtr->KeyValue("emaildirectory");
	eml_dir += "/";
	eml_dir += token.Storage.GetAccount();
	eml_dir += "/";
	eml_dir += token.Storage.GetDirectory();
	eml_dir += "/";

	ms.RetrieveMail(eml_dir, eml);
	eml.DeSerialize();

	eml.Header.SetSubject(str);

	mailCompostion.LoadProfiles();
	mailCompostion.SetEMail(eml, mode);
	clientAreaWidget.setCurrentWidget(&mailCompostion);
}

void MainWindow::eventDelete()
{
    mailBoxView.DeleteCurrent();
}

void MainWindow::eventFlag()
{
    mailBoxView.FlagCurrent();
}

void MainWindow::eventNext()
{
    mailBoxView.MoveToNext();
}

void MainWindow::eventPrevious()
{
    mailBoxView.MoveToPrevious();
}

void MainWindow::eventSearch()
{
	std::string searchterm = searchInput.text().toStdString();
	directoryView.Search(searchterm);
}

void MainWindow::eventRefresh()
{
	directoryView.Refresh();
}

void MainWindow::eventExpunge()
{
	directoryView.Expunge();
}

void MainWindow::eventProfileAdded(std::string profile_name)
{
    lblStatusMessage.setText("");
    directoryView.AddProfile(profile_name);
}

void MainWindow::eventProfileUpdated(std::string profile_name)
{
    directoryView.UpdateProfile(profile_name);
}

void MainWindow::eventProfileRemoved(std::string profile_name)
{
    directoryView.RemoveProfile(profile_name);
    mailBoxView.clear();
    mailView.Reset();
}

void MainWindow::eventMailFinished()
{
	clientAreaWidget.setCurrentWidget(&mailView);
}
