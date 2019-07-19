#ifndef _IMAP_CLIENT
#define _IMAP_CLIENT

#include "../data/Mail.h"
#include "../data/SecurityType.h"
#include <string>

using namespace std;

class ImapBearer;

class ImapClient
{
public:
    ImapClient();
    ImapClient(const std::string &host, uint16_t port, std::string username, std::string password, char sectype);
    ~ImapClient();

    void setAccountInformation(const std::string &host, uint16_t port, std::string username, std::string password, char sectype);
	bool disconnect();
	bool connect();
    bool getDirectoryList(std::vector<std::string> &dirList);
	bool getCapabilities();
	bool login();
	bool logout();
	bool getDirectory(std::string dirname, unsigned long &emailCount, unsigned long& uidNext);
	bool getEmailsSince(std::string dirname, std::string &fromdate, std::string &uidlist);
	bool getEmailsPrior(std::string dirname, std::string& fromdate, std::string& uidlist);
	bool getMessageHeader(long uid);
	bool getMessageBody(long uid);
	bool deleteMessage(long msgno);
	bool flagMessage(long msgno, std::string flag);
	bool expunge(long msgno);
	bool markAsSeen(long msgno);
    std::string error();
    std::string account();
private:
	std::string _Host;
    std::string _Username;
    std::string _Password;
    uint16_t _Port;

    char _SecurityType;
	std::string _CurrentDirectory;
	std::string _Error;

	ImapBearer* _BearerPtr;
};


#endif
