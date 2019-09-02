#ifndef MAIL_CLIENT
#define MAIL_CLIENT

#include <QtCore>
#include <QObject>
#include <QStringBuilder>
#include <QUrlQuery>
#include <QDebug>
#include <QList>
#include <QPair>
#include <QApplication>
#include <QMutex>
#include <QSplashScreen>
#include "MainWindow.h"
#include "MailClientOperations.h"
#include "../../data/Mail.h"
#include "../../utils/CommQueue.h"
#include "../../uicommon/ThemeHandler.h"

#include <vector>
#include <map>
#include <string>

using namespace std;

class MailClient : public QApplication, public MailClientOperations
{
    Q_OBJECT
public:
    explicit MailClient(int argc, char *argv[]);
    virtual ~MailClient();

	bool InitializeUI();
	void ShowSpalsh();
	void ShowSplashMessage(QString str);
	void ShowUI();

    bool SetupCommQueue();

	bool FetchConfiguration();

    bool FetchProfiles();
    bool FetchDirectories();

	bool GetAllContacts(std::vector<std::string>& ctlist);
	bool GetProfileList(std::vector<Profile>& ctlist);
	bool GetProfileInformation(std::string& str, Profile& prf);
	bool AddProfile(Profile& obj);
	bool UpdateProfile(Profile& obj);
	bool RemoveProfile(std::string& str);
	bool SendEmail(Mail& eml, MailStorageInformation &stg);
	bool GetAccountDirectories(std::string& profilename, std::vector<std::string>& resplines);
	bool GetEmails(std::string& profilename, std::string& dirname, std::vector<MailHeader>& mails, std::vector<MailStorageInformation>& stgl);
	bool GetEmailsByTerm(std::string& profilename, std::string& dirname, std::string& term, std::vector<MailHeader>& mails, std::vector<MailStorageInformation>& stgl);
	bool GetEmailHeader(std::string& profilename, std::string& dirname, std::string& uid);
	bool GetEmailBody(std::string& profilename, std::string& dirname, std::string& uid);
	bool RemoveEmail(std::string& profilename, std::string& dirname, std::string& uid, std::string& messageid);
	bool FlagEmail(std::string& profilename, std::string& dirname, std::string& uid, std::string& flag);
	bool MarkEmailSeen(std::string& profilename, std::string& dirname, std::string& uid);
	bool PurgeDeleted(std::string& profilename, std::string& dirname);

	std::vector<Profile>* ProfileList() { return  &profiles; }
	std::vector<std::string>* DirectoryList(std::string dir) { return &directories[dir]; }

    bool GetProfile(std::string &emailid, Profile &prof);

	std::string KeyValue(std::string key);

	ThemeSetting Theme();

private slots:
	void eventSwitchToLightTheme();
	void eventSwitchToDarkTheme();

 private:
	MainWindow applicationWindow;
	QSplashScreen applicationSpalsh;

    CommQueue mailRequestQueue;
    CommQueue contactRequestQueue;

    bool mailQueue, contactQueue;

	std::vector<Profile> profiles;
    std::map<std::string, std::vector<std::string>> directories;
	std::map<std::string, std::string> configuration;

	ThemeSetting theme;
};

extern MailClient* mailClientPtr;

#endif
