#ifndef MAILCLIENT_OPERATIONS
#define MAILCLIENT_OPERATIONS

#include <QObject>
#include "../data/Profile.h"
#include "../network/Mail.h"

#include <vector>
#include <string>

using namespace std;

class MailClientOperations
{
public:
    MailClientOperations() {}
    virtual ~MailClientOperations() {}

	virtual bool LoadConfiguration() = 0;
	virtual bool SaveConfiguration() = 0;
	virtual bool LoadEmail(std::string &profile, std::string &directory, std::string &uid, Mail &eml) = 0;
	virtual bool GetAllContacts(std::vector<std::string>& ctlist) = 0;
	virtual bool GetProfileList(std::vector<Profile>& ctlist) = 0;
	virtual bool GetProfileInformation(std::string& str, Profile& prf) = 0;
	virtual bool AddProfile(Profile& obj) = 0;
	virtual bool UpdateProfile(Profile& obj) = 0;
	virtual bool RemoveProfile(std::string& str) = 0;
	virtual bool SendEmail(Mail& eml) = 0;
	virtual bool GetAccountDirectories(std::string& profilename, std::vector<std::string>& resplines) = 0;
	virtual bool GetEmails(std::string& profilename, std::string& dirname, std::vector<MailHeader>& mails) = 0;
	virtual bool GetEmailsByTerm(std::string& profilename, std::string& dirname, std::string &term, std::vector<MailHeader>& mails) = 0;
	virtual bool GetEmailHeader(std::string& profilename, std::string& dirname, std::string& uid) = 0;
	virtual bool GetEmailBody(std::string& profilename, std::string& dirname, std::string& uid) = 0;
	virtual bool RemoveEmail(std::string& profilename, std::string& dirname, std::string& uid, std::string &messageid) = 0;
	virtual bool MarkEmailSeen(std::string& profilename, std::string& dirname, std::string& uid) = 0;
	virtual bool FlagEmail(std::string& profilename, std::string& dirname, std::string& uid, std::string& flag) = 0;
	virtual bool PurgeDeleted(std::string& profilename, std::string& dirname) = 0;
};

#endif
