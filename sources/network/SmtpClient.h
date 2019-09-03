#ifndef _SMTP_CLIENT
#define _SMTP_CLIENT

#include "Mail.h"
#include "SecurityType.h"
#include "TcpClient.h"
#include <string>

using namespace std;

class SmtpClient
{
public:
    SmtpClient();
    SmtpClient(const std::string hoststr, uint16_t portstr, std::string usernamestr, std::string passwordstr, SecurityType sectype);
    ~SmtpClient();
    void SetAccountInformation(const std::string hoststr, uint16_t portstr, std::string usernamestr, std::string passwordstr, SecurityType sectype);
	void SetPublicIp(std::string& ip);
	bool Disconnect();
	bool Connect();
    bool SendHelo();
	bool StartTls();
	bool NeedTls();
	bool Login();
	bool Logout();
	bool SendMail(Mail& mail);
    std::string Account();
	std::string Error();

private:
    std::string host;
    std::string username;
    std::string password;
	uint16_t port;
	SecurityType securityType;
	bool startTls;
	std::string publicIp;
	
	std::string errorStr;
    MailHeader emailHdr;
    MailBody emailBdy;
	TcpClient bearer;
};

#endif
