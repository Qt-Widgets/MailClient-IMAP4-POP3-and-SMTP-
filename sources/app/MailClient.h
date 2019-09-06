#ifndef MAIL_CLIENT
#define MAIL_CLIENT

#include "../network/TcpClient.h"

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
#include "../gui/ThemeHandler.h"

#include "../network/ImapClient.h"
#include "../network/SmtpClient.h"
#include "../network/Mail.h"

#include "../data/MailStorage.h"
#include "../data/MailDatabase.h"
#include "../data/ContactDatabase.h"

#include "../utils/Logger.h"
#include "../utils/Directory.h"
#include "../utils/StringEx.h"
#include "../utils/Configuration.h"

#include <vector>
#include <map>
#include <string>

typedef struct MailTransport
{
	SmtpClient smtp;
	ImapClient imap;
}MailTransport;

class MailClient : public QApplication, public MailClientOperations, public ContactOperations
{
    Q_OBJECT
public:
    explicit MailClient(int argc, char *argv[]);
    virtual ~MailClient();

	bool InitializeDB();
	bool InitializeNetwork();
	bool InitializeUI();

	void ShowSpalsh();
	void ShowSplashMessage(QString str);
	void ShowUI();

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
	bool AddContact(Contact& obj);
	bool UpdateContact(Contact& obj);
	bool RemoveContact(const std::string& contactId);

	std::vector<Profile>* ProfileList() { return  &profiles; }
	std::vector<std::string>* DirectoryList(std::string dir) { return &directories[dir]; }

    bool GetProfile(std::string &emailid, Profile &prof);

	std::string KeyValue(std::string key);

	ThemeSetting Theme();

	MailDatabase* MailDb();

private slots:
	void eventSwitchToLightTheme();
	void eventSwitchToDarkTheme();
	void LoadProfiles();

 private:
	MainWindow applicationWindow;
	QSplashScreen applicationSpalsh;

	std::vector<Profile> profiles;
    std::map<std::string, std::vector<std::string>> directories;
	std::map<std::string, std::string> configuration;

	ThemeSetting theme;

	std::string appName, publicIpAddress;
	MailDatabase mailDb;
	ContactDatabase contactDb;
	Configuration appConfig;
	std::map<std::string, Profile> profileList;
};

extern MailClient* mailClientPtr;

#endif
