#ifndef _RUSH_PRIORITY
#define _RUSH_PRIORITY

#include "../../network/ImapClient.h"
#include "../../network/SmtpClient.h"
#include "../../data/Mail.h"
#include "../../data/MailStorage.h"
#include "../../utils/Logger.h"
#include "../../utils/Directory.h"
#include "../../utils/StringEx.h"
#include "../../utils/CommQueue.h"
#include "../../utils/Configuration.h"
#include "../../data/MailDatabase.h"
#include "../../data/ContactDatabase.h"

typedef struct MailTransport
{
	SmtpClient smtp;
	ImapClient imap;
}MailTransport;

class RushPriority
{
public:
    explicit RushPriority(int argc, char *argv[]);
    virtual ~RushPriority();
	bool Initialize();
	void StartPollerLoop();
    void StartMessageLoop();
	MailDatabase* MailDb();

private:
	void GetProfileList();
	void GetProfileInformation(std::string &str);
	void AddProfile(std::string &str);
	void UpdateProfile(std::string& str);
	void RemoveProfile(std::string& str);

	void SendEmail(std::vector<std::string>& strlist);

	void GetAccountDirectories(std::string& str);
	void GetEmails(std::string& str1, std::string& str2);
	void GetEmailsByTerm(std::string& str1, std::string& str2, std::string& str3);
	void GetEmailHeader(std::string& profilename, std::string& directory, std::string& uid);
	void GetEmailBody(std::string& profilename, std::string& directory, std::string& uid);
	void RemoveEmail(std::string& profilename, std::string& directory, std::string& uid, std::string &messageid);
	void FlagEmail(std::string& profilename, std::string& directory, std::string& uid, std::string& flag);
	void MarkEmail(std::string& profilename, std::string& directory, std::string& uid);
	void PurgeDeleted(std::string& profilename, std::string& directory);

	void GetContactList();
	void GetContactDetails(std::string& contactId);
	void CreateContact(std::string& userinfo);
	void UpdateContact(std::string& userinfo);
	void RemoveContact(std::string& contactId);

	void LoadConfiguration();
	void SaveConfiguration(std::string &str);
	void GetConfiguration();

	void LoadProfiles();

	CommQueue cmdStream;
	std::string appName, publicIpAddress;
	MailDatabase mailDb;
	ContactDatabase contactDb;
	Configuration appConfig;
	std::map<std::string, Profile> profileList;
};

extern RushPriority* mailClientPtr;

#endif
