#ifndef _IMAP_CLIENT
#define _IMAP_CLIENT

#include "../data/Mail.h"
#include "../data/SecurityType.h"
#include "TcpClient.h"
#include <string>

using namespace std;

class ImapClient
{
public:
    ImapClient();
    ImapClient(const std::string &hoststr, uint16_t portstr, std::string usernamestr, std::string passwordstr, SecurityType sectype);
    ~ImapClient();

    void SetAccountInformation(const std::string &hoststr, uint16_t portstr, std::string usernamestr, std::string passwordstr, SecurityType sectype);
	bool Disconnect();
	bool Connect();
    bool GetDirectoryList(std::vector<std::string> &dirList);
	bool GetCapabilities();
	bool Login();
	bool Logout();
	bool GetDirectory(std::string dirname, unsigned long &emailCount, unsigned long& uidNext);
	bool SelectDirectory(std::string dirname);
	bool GetEmailsSince(std::string dirname, std::string &fromdate, std::string &uidlist);
	bool GetEmailsPrior(std::string dirname, std::string& fromdate, std::string& uidlist);
	bool GetEmailsRecent(std::string dirname, std::string& uidlist);
	bool GetEmailsAll(std::string dirname, std::string& uidlist);
	bool GetMessageHeader(std::string& uid, Mail& mail);
	bool GetMessageBody(std::string& uid, Mail& mail);
	bool DeleteMessage(std::string& uid);
	bool FlagMessage(std::string& uid, std::string flag);
	bool MarkAsSeen(std::string& uid);
	bool Expunge(std::string& dir);
    std::string Error();
    std::string Account();
private:
	std::string host;
    std::string username;
    std::string password;
    uint16_t port;

	SecurityType securityType;
	std::string currentDirectory;
	std::string errorStr;

	TcpClient bearer;
};


#endif
