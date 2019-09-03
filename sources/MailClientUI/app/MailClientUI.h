#ifndef MAIL_CLIENT_UI
#define MAIL_CLIENT_UI

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
#include "MailClientOperations.h"
#include "ContactOperations.h"
#include "../gui/MainWindow.h"
#include "../../network/Mail.h"
#include "../../utils/CommQueue.h"
#include "../gui/ThemeHandler.h"

#include <vector>
#include <map>
#include <string>

using namespace std;

class MailClientUI : public QApplication, public MailClientOperations, public ContactOperations
{
    Q_OBJECT
public:
    explicit MailClientUI(int argc, char *argv[]);
    virtual ~MailClientUI();

	bool InitializeUI();
	void ShowSpalsh();
	void ShowSplashMessage(QString str);
	void ShowUI();

    bool SetupCommQueue();

	bool FetchConfiguration();

    bool FetchProfiles();
    bool FetchDirectories();

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

	bool SearchContacts(std::vector<std::string>& ctlist, std::string& term);
	bool GetAllContacts(std::vector<std::string>& ctlist);
	bool GetContact(const std::string& contactId, Contact& obj);
	bool AddContact(const Contact& obj);
	bool UpdateContact(const Contact& obj);
	bool RemoveContact(const std::string& contactId);

	std::vector<Profile>* ProfileList() { return  &profiles; }
	std::vector<std::string>* DirectoryList(std::string dir) { return &directories[dir]; }

    bool GetProfile(std::string &emailid, Profile &prof);

	std::string KeyValue(std::string key);

	ThemeSetting Theme();

private slots:
	void eventSwitchToLightTheme();
	void eventSwitchToDarkTheme();
	void SerializeContact(const Contact& obj, string& str);
	void DeSerializeProfile(Profile& prf, std::string& str);
	void SerializeProfile(Profile& prf, std::string& str);

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

extern MailClientUI* mailClientPtr;

#endif