#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QVBoxLayout>
#include <QLabel>
#include <QtCore>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QTabBar>
#include <QStackedWidget>

#include "DirectoryView.h"
#include "MailBoxView.h"
#include "ActionBuilder.h"
#include "MailView.h"
#include "Calender.h"
#include "Tasks.h"
#include "About.h"
#include "SettingsView.h"
#include "ProfileView.h"
#include "MailComposer.h"
#include "AddressBookWindow.h"

class MainWindow : public QMainWindow, ActionBuilder
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool Initialize();
	SettingsView* Settings() { return &settingsView; }

protected:
    void SetupViews();

protected slots:
    void eventExit();
    void eventNewMail();
    void eventAddressBook();

    void eventReply();
    void eventReplyAll();
    void eventForward();
    void eventDelete();
    void eventFlag();

    void eventNext();
    void eventPrevious();
    void eventSearch();
    void eventRefresh();
	void eventExpunge();

    void eventProfileAdded(std::string profile_name, std::string& emailId);
    void eventProfileUpdated(std::string profile_name, std::string& emailId);
    void eventProfileRemoved(std::string profile_name, std::string& emailId);

	void eventMailSelected(MailHeader emlhdr);
	void eventMailFinished();

private:

	void extendEmail(CompositionMode mode);

    QToolBar        appToolBar;
    QStatusBar      appStatusBar;
    QLabel          lblStatusMessage;

    QSplitter       appCentralWidget;
	QStackedWidget  clientAreaWidget;

    DirectoryView   directoryView;
    MailBoxView     mailBoxView;
    MailView		mailView;

    QTabWidget		appTabs;

    Calender		calendar;
    Tasks			pendingTasks;
	SettingsView	settingsView;
    ProfileView		profileView;
	About			about;

    MailComposer	mailCompostion;
	AddressBookWindow addressBook;
};

#endif // MAINWINDOW_H
