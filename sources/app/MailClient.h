#ifndef MAIL_CLIENT
#define MAIL_CLIENT

#include "./network/ImapClient.h"
#include "./network/SmtpClient.h"
#include "./network/Mail.h"
#include "./utils/Logger.h"
#include "./utils/Directory.h"
#include "./utils/StringEx.h"

typedef struct MailTransport
{
	SmtpClient smtp;
	ImapClient imap;
}MailTransport;

class MailClient
{
public:
    explicit MailClient(int argc, char *argv[]);
    virtual ~MailClient();
    void Start();
	bool Initialize();
	void TestIn();
	void TestOut();

private:
	void GetProfileList();
	void GetProfileInformation(std::string &str);
	void AddProfile(std::string &str);
	void UpdateProfile(std::string& str);
	void RemoveProfile(std::string& str);
	void SendEmail(std::string& str);
	void GetAccountDirectories(std::string& str);
	void GetDirectoryInformation(std::string& str);
	void GetEmailHeader(std::string& str);
	void GetEmailBody(std::string& str);
	void RemoveEmail(std::string& str);
	void FlagEmail(std::string& str);
	void MarkEmail(std::string& str);
	void PurgeDeleted(std::string& str);
};

extern MailClient* mailClientPtr;

#endif
