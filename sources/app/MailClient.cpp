#include "MailClient.h"
#include "./utils/StringEx.h"
#include <iostream>

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
}
