#ifndef _SMTP_CLIENT
#define _SMTP_CLIENT

#include "SecurityType.h"
#include "Mail.h"
#include <string>

using namespace std;

class SmtpBearer;

class SmtpClient
{
public:
    SmtpClient();
    SmtpClient(const std::string host, uint16_t port, std::string username, std::string password, char sectype, bool sendhelo = false);
    ~SmtpClient();
    void setAccountInformation(const std::string host, uint16_t port, std::string username, std::string password, char sectype, bool sendhelo = false);
    bool sendMail(MailHeader &ehdr, MailBody &ebdy);
    std::string account();
	std::string error();

private:
    std::string _Host;
    std::string _Username;
    std::string _Password;
	uint16_t _Port;
    bool _SendHelo;
    char _SecurityType;    
	
	std::string _Error;
    MailHeader _EmlHdr;
    MailBody _EmlBdy;
	SmtpBearer* _BearerPtr;
};

#endif
