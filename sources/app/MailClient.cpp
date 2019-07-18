#include "MailClient.h"
#include "./utils/StringEx.h"
#include <iostream>
#include "../network/TcpClient.h"

MailClient* mailClientPtr = nullptr;

MailClient::MailClient(int argc, char *argv[])
{
    mailClientPtr = this;
	Logger::GetInstance()->SetModuleName(argv[0]);
	Logger::GetInstance()->StartLogging();
}

MailClient::~MailClient()
{
}

bool MailClient::Initialize()
{
	return true;
}

void MailClient::Start()
{
}


void MailClient::SendEmail(std::string& str)
{
}

void MailClient::GetAccountDirectories(std::string& str)
{
}

void MailClient::GetDirectoryInformation(std::string& str)
{

}

void MailClient::GetEmailHeader(std::string& str)
{
}

void MailClient::GetEmailBody(std::string& str)
{
}

void MailClient::RemoveEmail(std::string& str)
{
}

void MailClient::FlagEmail(std::string& str)
{
}

void MailClient::MarkEmail(std::string& str)
{
}

void MailClient::PurgeDeleted(std::string& str)
{
}

void MailClient::TestIn()
{
	ImapClient climap;

	//climap.setAccountInformation("outlook.office365.com", 993, "subrato.roy@collabera.com", "Spades@2019", 'S');
	climap.setAccountInformation("rushpriority.com", 993, "subrato1@rushpriority.com", "roy1", 'S');

	if (climap.connect())
	{
		if (climap.getCapabilities())
		{
			if (climap.login())
			{
				std::vector<std::string> dlist;

				if (climap.getDirectoryList(dlist))
				{
					unsigned long count, uidnext;

					if (climap.getDirectory("INBOX", count, uidnext))
					{
						std::string uidstr;
						std::string fromdate = "17-JUL-2019";

						climap.getDirectory("INBOX", fromdate, uidstr);

						std::vector<std::string> uidlist;

						strsplit(uidstr, uidlist, ' ', true);

						for (std::string str : uidlist)
						{
							if (climap.getMessageHeader(atoi(str.c_str())))
							{
								climap.getMessageBody(atoi(str.c_str()));
							}
						}
					}
				}
			}
			climap.logout();
		}
	}

	climap.disconnect();
}

void MailClient::TestOut()
{
	std::string publicIpAddress;

	TcpClient cl;

	std::string request = "GET / HTTP/1.1\r\nHost: api.ipify.org\r\nConnection: close\r\n\r\n";
	std::string response = "";

	if (cl.createSocket("api.ipify.org", 443, true))
	{
		int retcode = 0;
		if (cl.connectSocket(retcode))
		{
			cl.sendString(request);

			cl.receiveString(response);

			std::vector<std::string> tokens;

			strsplit(response, tokens, "\r\n", true);

			if (strcontains(tokens[0].c_str(), "200 OK"))
			{
				publicIpAddress = tokens[tokens.size() - 1];
			}
		}
	}

	MailHeader hdr;
	MailBody bdy;

	std::string serialized_header;
	std::string serialized_body;

	hdr.setFrom("subrato.roy@collabera.com");
	hdr.setAccount("subrato.roy@collabera.com");
	hdr.addtoToList("sriiixi@gmail.com");
	hdr.addtoCcList("subratoroy@hotmail.com");
	hdr.setSubject("Test Message from RP2");
	hdr.serialize(serialized_header, nullptr);

	bdy.setMessage("Text body as test message from Rush Priority 2 testing code.....");
	bdy.setMessageId(hdr.messageId());
	bdy.serialize(serialized_body);

	SmtpClient clsmtp;
	clsmtp.setAccountInformation("smtp.office365.com", 587, "subrato.roy@collabera.com", "Spades@2019", 'N');
	clsmtp.setPublicIp(publicIpAddress);

	if (clsmtp.connect())
	{
		if (clsmtp.sendHelo())
		{
			if (clsmtp.needTls())
			{
				if (clsmtp.startTls())
				{
					if (clsmtp.sendHelo())
					{
						if (clsmtp.login())
						{
							clsmtp.sendMail(hdr, bdy);
							clsmtp.logout();
						}
					}
				}
			}
			else
			{
				if (clsmtp.login())
				{
					clsmtp.sendMail(hdr, bdy);
					clsmtp.logout();
				}
			}
		}
	}
	clsmtp.disconnect();
}
