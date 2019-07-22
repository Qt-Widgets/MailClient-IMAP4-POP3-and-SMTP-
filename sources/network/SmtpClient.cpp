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
	host = "";
	username = "";
	password = "";
	port = 25;
	securityType = None;
	startTls = false;

	bearerPtr = new SmtpBearer();
}

SmtpClient::SmtpClient(const std::string hoststr, uint16_t portstr, std::string usernamestr, std::string passwordstr, SecurityType sectype)
{
	host = hoststr;
	username = usernamestr;
	password = passwordstr;
	port = portstr;
	securityType = sectype;
	startTls = false;

	bearerPtr = new SmtpBearer();
}

SmtpClient::~SmtpClient()
{
    if (bearerPtr)
    {
        bearerPtr->cl.CloseSocket();
    }
}

void SmtpClient::SetAccountInformation(const std::string hoststr, uint16_t portstr, std::string usernamestr, std::string passwordstr, SecurityType sectype)
{
	host = hoststr;
	username = usernamestr;
	password = passwordstr;
	port = portstr;
	securityType = sectype;
	startTls = false;

    return;
}

bool SmtpClient::Connect()
{
	bool need_ssl = false;

	// Note: For SMTP TLS sessions starts on plain sockets and switches to SSL sockets after STARTTLS negotiation
	if (securityType == None || securityType == Tls)
	{
		need_ssl = false;
	}
	else
	{
		need_ssl = true;
	}

    int retcode;

    if (bearerPtr->cl.CreateSocket(host.c_str(), port, need_ssl))
    {
        if (bearerPtr->cl.ConnectSocket(retcode))
        {
			std::string resp;
			bearerPtr->cl.ReceiveString(resp);
            return true;
        }
    }

    return false;
}

bool SmtpClient::Disconnect()
{
	if (bearerPtr->cl.IsConnected())
	{
		return bearerPtr->cl.CloseSocket();
	}

	return false;
}

bool SmtpClient::SendHelo()
{
    std::string resp;
    char buff[128] = { 0 };
    sprintf(buff, "EHLO %s\r\n", publicIp.c_str());

    std::string helo = buff;
    bearerPtr->cl.SendString(helo);

    while (true)
    {
		if (!bearerPtr->cl.ReceiveString(resp))
		{
		    return false;
		}

		if (strcontains(resp.c_str(), "STARTTLS"))
		{
			startTls = true;
		}

        if(bearerPtr->cl.PendingPreFetchedBufferSize() < 1)
        {
            return true;
        }
    }

    return false;
}

void SmtpClient::SetPublicIp(std::string& ip)
{
	publicIp = ip;
}

std::string SmtpClient::Account()
{
    return username;
}

std::string SmtpClient::Error()
{
	return error;
}

bool SmtpClient::SendMail(MailHeader &ehdr, MailBody &ebdy)
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
	sprintf(buff, "MAIL FROM: <%s>\r\n", username.c_str());

	bearerPtr->cl.SendString(buff);

	while (true)
	{
		if (!bearerPtr->cl.ReceiveString(resp))
		{
			return false;
		}

		// FROM is verified
		if (strcontains(resp.c_str(), "250 2.1.0"))
		{
			memset(buff, 0, 128);
			sprintf(buff, "RCPT TO: <%s>\r\n", all_rcpt[sent].c_str());
			bearerPtr->cl.SendString(buff);
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
				bearerPtr->cl.SendString(buff);
			}
			else
			{
				memset(buff, 0, 128);
				sprintf(buff, "RCPT TO: <%s>\r\n", all_rcpt[sent].c_str());
				bearerPtr->cl.SendString(buff);
				sent++;
			}
			continue;
		}

		if (strcontains(resp.c_str(), "354"))
		{
			std::string data;
			ehdr.serialize(data, &ebdy);
			bearerPtr->cl.SendString(data);
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

bool SmtpClient::StartTls()
{
	std::string resp;
	char buff[128] = { 0 };
	sprintf(buff, "STARTTLS\r\n");

	bearerPtr->cl.SendString(buff);

	bool handshake_ok = false;

	while (true)
	{
		if (!bearerPtr->cl.ReceiveString(resp))
		{
			return false;
		}

		if (strcontains(resp.c_str(), "220"))
		{
			handshake_ok = true;
		}

		if (bearerPtr->cl.PendingPreFetchedBufferSize() < 1)
		{
			break;
		}
	}

	return bearerPtr->cl.SwitchToSecureMode();
}

bool SmtpClient::NeedTls()
{
	return startTls;
}

bool SmtpClient::Login()
{
	std::string resp;
	std::vector<std::string> tokens;

	char buff[128] = { 0 };

	sprintf(buff, "AUTH LOGIN\r\n");
	int len = (int)strlen(buff);

	bearerPtr->cl.SendString(buff);

	while (true)
	{
		if (!bearerPtr->cl.ReceiveString(resp))
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
				codec_b64.EncodeBase64((unsigned char*)username.c_str(), username.length(), olen, encoded_uname);
				encoded_uname += "\r\n";
				bearerPtr->cl.SendString(encoded_uname);
				continue;
			}

			if (strcontains(decoded_str.c_str(), "Password"))
			{
				std::string encoded_password;
				codec_b64.EncodeBase64((unsigned char*)password.c_str(), password.length(), olen, encoded_password);
				encoded_password += "\r\n";
				bearerPtr->cl.SendString(encoded_password);
				continue;
			}
		}

		if (bearerPtr->cl.PendingPreFetchedBufferSize() < 1)
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

bool SmtpClient::Logout()
{
	std::string resp;
	std::vector<std::string> tokens;

	char buff[128] = { 0 };

	sprintf(buff, "QUIT\r\n");
	int len = (int)strlen(buff);

	bearerPtr->cl.SendString(buff);

	while (true)
	{
		if (!bearerPtr->cl.ReceiveString(resp))
		{
			return false;
		}

		if (strcontains(resp.c_str(), "221"))
		{
			break;
		}
	}

	return true;
}
