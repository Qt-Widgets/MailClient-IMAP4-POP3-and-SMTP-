#include <memory.h>
#include "../utils/StringEx.h"
#include "SmtpClient.h"
#include "TcpClient.h"

class SmtpBearer
{
public:
    TcpClient cl;
	SmtpBearer()
	{
	}
};

SmtpClient::SmtpClient()
{
	_Host = "";
	_Username = "";
	_Password = "";
	_Port = 25;
	_SendHelo = true;
	_SecurityType = 'N';

	_BearerPtr = new SmtpBearer();
}

SmtpClient::SmtpClient(const std::string host, uint16_t port, std::string username, std::string password, char sectype, bool sendhelo)
{
	_Host = host;
	_Username = username;
	_Password = password;
	_Port = port;
	_SendHelo = sendhelo;
	_SecurityType = sectype;

	_BearerPtr = new SmtpBearer();
}

SmtpClient::~SmtpClient()
{
    if (_BearerPtr)
    {
        _BearerPtr->cl.closeSocket();
    }
}

void SmtpClient::setAccountInformation(const std::string host, uint16_t port, std::string username, std::string password, char sectype, bool sendhelo)
{
	_Host = host;
	_Username = username;
	_Password = password;
	_Port = port;
	_SendHelo = sendhelo;
	_SecurityType = sectype;

    return;
}

bool SmtpClient::connect()
{
    int retcode;
    if (_BearerPtr->cl.createSocket(_Host.c_str(), _Port, false))
    {
        if (_BearerPtr->cl.connectSocket(retcode))
        {
            return true;
        }
    }

    return false;
}

bool SmtpClient::sendHelo()
{
    std::string resp;
    std::string my_ip = "123.201.90.94";
    char buff[128] = { 0 };
    sprintf(buff, "EHLO %s\r\n", my_ip.c_str());

    std::string helo = buff;
    _BearerPtr->cl.sendString(helo);

    while (true)
    {
        if (!_BearerPtr->cl.receiveString(resp, "\r\n"))
        {
            return false;
        }

        if (resp.length() < 1)
        {
            return true;
        }

        if(_BearerPtr->cl.pendingPreFetchedBufferSize() < 1)
        {
            return true;
        }
    }

    return false;
}

std::string SmtpClient::account()
{
    return _Username;
}

std::string SmtpClient::error()
{
	return _Error;
}

bool SmtpClient::sendMail(MailHeader &ehdr, MailBody &ebdy)
{
    bool ret = false;

	return ret;
}
