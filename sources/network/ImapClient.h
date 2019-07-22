#ifndef _IMAP_CLIENT
#define _IMAP_CLIENT

#include "Mail.h"
#include "SecurityType.h"
#include <string>

using namespace std;

class ImapBearer;

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
	bool getEmailsSince(std::string dirname, std::string &fromdate, std::string &uidlist);
	bool GetEmailsPrior(std::string dirname, std::string& fromdate, std::string& uidlist);
	bool GetMessageHeader(long uid);
	bool GetMessageBody(long uid);
	bool DeleteMessage(long msgno);
	bool FlagMessage(long msgno, std::string flag);
	bool Expunge(long msgno);
	bool MarkAsSeen(long msgno);
    std::string Error();
    std::string Account();
private:
	std::string host;
    std::string username;
    std::string password;
    uint16_t port;

	SecurityType securityType;
	std::string currentDirectory;
	std::string _Error;

	ImapBearer* bearerPtr;
};


#endif
