#include <memory.h>
#include "../utils/StringEx.h"
#include "../utils/Base64.h"
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
	_StartTls = false;

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
	_StartTls = false;

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
	_StartTls = false;

    return;
}

bool SmtpClient::connect()
{
    int retcode;
    if (_BearerPtr->cl.createSocket(_Host.c_str(), _Port, false))
    {
        if (_BearerPtr->cl.connectSocket(retcode))
        {
			std::string resp;
			_BearerPtr->cl.receiveString(resp);
            return true;
        }
    }

    return false;
}

bool SmtpClient::disconnect()
{
	if (_BearerPtr->cl.isConnected())
	{
		return _BearerPtr->cl.closeSocket();
	}

	return false;
}

bool SmtpClient::sendHelo()
{
    std::string resp;
    char buff[128] = { 0 };
    sprintf(buff, "EHLO %s\r\n", _PublicIp.c_str());

    std::string helo = buff;
    _BearerPtr->cl.sendString(helo);

    while (true)
    {
		if (!_BearerPtr->cl.receiveString(resp))
		{
		    return false;
		}

		if (strcontains(resp.c_str(), "STARTTLS"))
		{
			_StartTls = true;
		}

        if(_BearerPtr->cl.pendingPreFetchedBufferSize() < 1)
        {
            return true;
        }
    }

    return false;
}

void SmtpClient::setPublicIp(std::string& ip)
{
	_PublicIp = ip;
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
	std::vector<std::string> all_rcpt;

	for (auto s : ehdr.toList())
	{
		all_rcpt.push_back(s);
	}

	for (auto s : ehdr.ccList())
	{
		all_rcpt.push_back(s);
	}

	for (auto s : ehdr.bccList())
	{
		all_rcpt.push_back(s);
	}

	long pending_rcpt = all_rcpt.size();
	long sent = 0;

    bool ret = false;
	std::string resp;
	char buff[128] = { 0 };

	memset(buff, 0, 128);
	sprintf(buff, "MAIL FROM: <%s>\r\n", _Username.c_str());

	_BearerPtr->cl.sendString(buff);

	while (true)
	{
		if (!_BearerPtr->cl.receiveString(resp))
		{
			return false;
		}

		// FROM is verified
		if (strcontains(resp.c_str(), "250 2.1.0"))
		{
			memset(buff, 0, 128);
			sprintf(buff, "RCPT TO: <%s>\r\n", all_rcpt[sent].c_str());
			_BearerPtr->cl.sendString(buff);
			sent++;
			continue;
		}

		// RCPT TO is verified
		if (strcontains(resp.c_str(), "250 2.1.5"))
		{
			if (pending_rcpt == sent)
			{
				memset(buff, 0, 128);
				sprintf(buff, "DATA\r\n");
				_BearerPtr->cl.sendString(buff);
			}
			else
			{
				memset(buff, 0, 128);
				sprintf(buff, "RCPT TO: <%s>\r\n", all_rcpt[sent].c_str());
				_BearerPtr->cl.sendString(buff);
				sent++;
			}
			continue;
		}

		if (strcontains(resp.c_str(), "354"))
		{
			std::string data;
			ehdr.serialize(data, &ebdy);
			_BearerPtr->cl.sendString(data);
			continue;
		}

		if (strcontains(resp.c_str(), "250 2.0.0"))
		{
			ret = true;
			break;
		}
	}

	return ret;
}

bool SmtpClient::startTls()
{
	std::string resp;
	char buff[128] = { 0 };
	sprintf(buff, "STARTTLS\r\n");

	_BearerPtr->cl.sendString(buff);

	bool handshake_ok = false;

	while (true)
	{
		if (!_BearerPtr->cl.receiveString(resp))
		{
			return false;
		}

		if (strcontains(resp.c_str(), "220"))
		{
			handshake_ok = true;
		}

		if (_BearerPtr->cl.pendingPreFetchedBufferSize() < 1)
		{
			break;
		}
	}

	return _BearerPtr->cl.switchToSecureMode();
}

bool SmtpClient::needTls()
{
	return _StartTls;
}

bool SmtpClient::login()
{
	std::string resp;
	std::vector<std::string> tokens;

	char buff[128] = { 0 };

	sprintf(buff, "AUTH LOGIN\r\n");
	int len = (int)strlen(buff);

	_BearerPtr->cl.sendString(buff);

	while (true)
	{
		if (!_BearerPtr->cl.receiveString(resp))
		{
			return false;
		}

		//Authentication challenge
		if (strcontains(resp.c_str(), "334"))
		{
			strremove(resp, '\n');
			strremove(resp, '\r');
			tokens.clear();
			strsplit(resp, tokens, ' ');

			class Base64 codec_b64;
			std::string decoded_str;
			unsigned long olen;
			decoded_str = (char*)codec_b64.DecodeBase64(tokens[1].c_str(), tokens[1].length(), olen);
			decoded_str[olen] = 0;

			if (strcontains(decoded_str.c_str(), "Username"))
			{
				std::string encoded_uname;
				codec_b64.EncodeBase64((unsigned char*)_Username.c_str(), _Username.length(), olen, encoded_uname);
				encoded_uname += "\r\n";
				_BearerPtr->cl.sendString(encoded_uname);
				continue;
			}

			if (strcontains(decoded_str.c_str(), "Password"))
			{
				std::string encoded_password;
				codec_b64.EncodeBase64((unsigned char*)_Password.c_str(), _Password.length(), olen, encoded_password);
				encoded_password += "\r\n";
				_BearerPtr->cl.sendString(encoded_password);
				continue;
			}
		}

		if (_BearerPtr->cl.pendingPreFetchedBufferSize() < 1)
		{
			tokens.clear();
			strsplit(resp, tokens, ' ');
			
			if (tokens[0] == "235")
			{
				return true;
			}

			break;
		}
	}

	return false;
}
