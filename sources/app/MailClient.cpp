#include "MailClient.h"
#include "./utils/StringEx.h"
#include "./network/TcpClient.h"

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

void MailClient::Test()
{
	//ImapClient climap;

	//climap.setAccountInformation("imap-mail.outlook.com", 993, "subratoroy@hotmail.com", "Silveroak@1974", 'S');

	//if (climap.connect())
	//{
	//	if (climap.getCapabilities())
	//	{
	//		if (climap.login())
	//		{
	//			std::vector<std::string> dlist;

	//			if (climap.getDirectoryList(dlist))
	//			{
	//				unsigned long count, uidnext;

	//				if (climap.getDirectory("INBOX", count, uidnext))
	//				{
	//					if (climap.getMessageHeader(count - 1))
	//					{
	//						climap.getMessageBody(count - 1);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

    TcpClient cl;
    std::string my_ip;

	char buff[128] = { 0 };

    if (cl.createSocket("smtp-mail.outlook.com", 587))
	{
		int retcode = 0;
		if (cl.connectSocket(retcode))
		{
			sprintf(buff, "EHLO %s\r\n", my_ip.c_str());

		}
	}

}