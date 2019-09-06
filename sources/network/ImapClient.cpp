#include <memory.h>
#include "../utils/StringEx.h"
#include "ImapClient.h"

unsigned long getNumber(const std::string& str);

ImapClient::ImapClient()
{
	host = "";
	username = "";
	password = "";
	port = 0;
	securityType = None;

	currentDirectory = "";
	errorStr = "";
}

ImapClient::ImapClient(const std::string &hoststr, uint16_t portstr, std::string usernamestr, std::string passwordstr, SecurityType sectype)
{
	if (sectype == Tls)
	{
		sectype = Ssl;
	}

    host = hoststr;
    port = portstr;
    username = usernamestr;
    password = passwordstr;
    securityType = sectype;

	currentDirectory = "";
	errorStr = "";
}

ImapClient::~ImapClient()
{
	bearer.CloseSocket();
}

void ImapClient::SetAccountInformation(const std::string &hoststr, uint16_t portstr, std::string usernamestr, std::string passwordstr, SecurityType sectype)
{
	if (sectype == Tls)
	{
		sectype = Ssl;
	}

    host = hoststr;
    port = portstr;
    username = usernamestr;
    password = passwordstr;
    securityType = sectype;
	currentDirectory = "";

    return;
}

bool ImapClient::Connect()
{
	bool need_ssl = false;

	if (securityType == None)
	{
		need_ssl = false;
	}
	else
	{
		need_ssl = true;
	}		
	
	int retcode;

	if (bearer.CreateSocket(host.c_str(), port, need_ssl))
	{
		if (bearer.ConnectSocket(retcode))
		{
			return true;
		}
	}

	return false;
}

bool ImapClient::Disconnect()
{
	if (bearer.IsConnected())
	{
		return bearer.CloseSocket();
	}

	return false;
}

bool ImapClient::GetCapabilities()
{
	std::string resp;
	std::string capability = "CY CAPABILITY\r\n";
	bearer.SendString(capability);

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			return false;
		}

		if (strcontains(resp.c_str(), "CY OK"))
		{
			return true;
		}
	}

	return false;
}

bool ImapClient::Login()
{
	std::string resp;

	std::string login = "LG LOGIN ";
	login += username + " ";
	login += password + "\r\n";

	bearer.SendString(login);

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			return false;
		}

		if (strcontains(resp.c_str(), "LG OK"))
		{
			return true;
		}
	}

	return false;
}

bool ImapClient::Logout()
{
	std::string resp;

	std::string login = "LG LOGOUT\r\n ";

	bearer.SendString(login);

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			return false;
		}

		if (strcontains(resp.c_str(), "LG OK"))
		{
			return true;
		}
	}

	return false;
}

std::string ImapClient::Error()
{
    return errorStr;
}

std::string ImapClient::Account()
{
    return username;
}

bool ImapClient::Expunge(std::string& dir)
{
	std::string resp;
	std::string capability = "EX EXPUNGE\r\n";
	bearer.SendString(capability);

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			return false;
		}

		if (strcontains(resp.c_str(), "EX OK"))
		{
			return true;
		}
	}

	return false;
}

bool ImapClient::MarkAsSeen(std::string& uid)
{
	std::string resp;
	char command[128] = { 0 };
	memset(command, 0, 128);
	sprintf(command, "UID STORE %s +FLAGS \\Seen\r\n", uid.c_str());
	bearer.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			result = false;
			break;
		}

		if (strcontains(resp.c_str(), "OK STORE") || strcontains(resp.c_str(), "OK Store"))
		{
			result = true;
			break;
		}
	}

	return result;
}

bool ImapClient::DeleteMessage(std::string& uid)
{
	std::string resp;
	char command[128] = { 0 };
	memset(command, 0, 128);
	sprintf(command, "UID STORE %s +FLAGS \\Deleted\r\n", uid.c_str());
	bearer.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			result = false;
			break;
		}

		if (strcontains(resp.c_str(), "OK STORE") || strcontains(resp.c_str(), "OK Store"))
		{
			result = true;
			break;
		}
	}

	return result;
}

bool ImapClient::FlagMessage(std::string& uid, std::string flag)
{
	std::string resp;
	char command[128] = { 0 };
	memset(command, 0, 128);
	sprintf(command, "UID STORE %s +FLAGS \\%s\r\n", uid.c_str(), flag.c_str());
	bearer.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			result = false;
			break;
		}

		if (strcontains(resp.c_str(), "OK STORE") || strcontains(resp.c_str(), "OK Store"))
		{
			result = true;
			break;
		}
	}

	return result;
}

bool ImapClient::GetDirectoryList(std::vector<std::string>& dirList)
{
	std::string resp;

	std::string temp = std::string("LS LIST ") + std::string("\"\"") + std::string(" \"*\"") + std::string("\r\n");
	char command[128] = { 0 };
	sprintf(command, "%s", temp.c_str());

	bearer.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			return false;
		}

		if (strcontains(resp.c_str(), "HasChildren") || strcontains(resp.c_str(), "Sync Issue"))
		{
			continue;
		}

		std::vector<std::string> templist;

		if (strcontains(resp.c_str(), "\".\""))
		{
			strsplit(resp, templist, "\".\"", true);
			resp = templist[templist.size() -1];
		}

		if (strcontains(resp.c_str(), "\"/\""))
		{
			strsplit(resp, templist, "\"/\"", true);
			resp = templist[templist.size() - 1];
		}

		if (strcontains(resp.c_str(), "LS OK"))
		{
			result = true;
			break;
		}		
		
		dirList.push_back(resp);
	}

	return result;
}

bool ImapClient::SelectDirectory(std::string dirname)
{
	currentDirectory = dirname;

	std::string resp;
	std::vector<std::string> buffer;
	char command[128] = { 0 };
	sprintf(command, "IN SELECT \"%s\"\r\n", currentDirectory.c_str());

	bearer.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			result = false;
			break;
		}

		if (strcontains(resp.c_str(), "IN OK"))
		{
			result = true;
			break;
		}

		if (strcontains(resp.c_str(), "IN BAD"))
		{
			result = false;
			break;
		}

		buffer.push_back(resp);
	}

	return result;
}

bool ImapClient::GetDirectory(std::string dirname, unsigned long& emailCount, unsigned long &uidNext)
{
	currentDirectory = dirname;
	
	std::string resp;
	std::vector<std::string> buffer;
	char command[128] = { 0 };
	sprintf(command, "IN SELECT \"%s\"\r\n", currentDirectory.c_str());

	bearer.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			result = false;
			break;
		}

		if (strcontains(resp.c_str(), "IN OK"))
		{
			result = true;
			break;
		}		

		if (strcontains(resp.c_str(), "IN BAD"))
		{
			result = false;
			break;
		}
		
		buffer.push_back(resp);
	}

	if (result)
	{
		for (auto str : buffer)
		{
			if (strcontains(str.c_str(), "EXISTS") || (strcontains(str.c_str(), "exists")))
			{
				emailCount = getNumber(str);
			}

			if (strcontains(str.c_str(), "UIDNEXT") || (strcontains(str.c_str(), "uidnext")))
			{
				uidNext = getNumber(str);
			}
		}
	}

	return result;
}

bool ImapClient::GetEmailsAll(std::string dirname, std::string& uidlist)
{
	std::string fromdate = "01-JAN-1980";
	std::string resp;
	std::vector<std::string> buffer;
	char command[128] = { 0 };
	memset(command, 0, 128);
	sprintf(command, "UID SEARCH SINCE \"%s\"\r\n", fromdate.c_str());
	bearer.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			result = false;
			break;
		}

		if (strcontains(resp.c_str(), "UID OK"))
		{
			result = true;
			break;
		}

		if (strcontains(resp.c_str(), "UID NO") || strcontains(resp.c_str(), "UID BAD"))
		{
			result = false;
			break;
		}

		uidlist = resp;
		strreplace(uidlist, "SEARCH", "*");
		strremove(uidlist, '*');
		stralltrim(uidlist);
	}

	return result;
}

bool ImapClient::GetEmailsSince(std::string dirname, std::string& fromdate, std::string& uidlist)
{
	std::string resp;
	std::vector<std::string> buffer;
	char command[128] = { 0 };
	memset(command, 0, 128);
	sprintf(command, "UID SEARCH SINCE \"%s\"\r\n", fromdate.c_str());
	bearer.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			result = false;
			break;
		}

		if (strcontains(resp.c_str(), "UID OK"))
		{
			result = true;
			break;
		}

		if (strcontains(resp.c_str(), "UID NO") || strcontains(resp.c_str(), "UID BAD"))
		{
			result = false;
			break;
		}

		uidlist = resp;
		strreplace(uidlist, "SEARCH", "*");
		strremove(uidlist, '*');
		stralltrim(uidlist);
	}

	return result;
}

bool ImapClient::GetEmailsPrior(std::string dirname, std::string& fromdate, std::string& uidlist)
{
	std::string resp;
	std::vector<std::string> buffer;
	char command[128] = { 0 };
	memset(command, 0, 128);
	sprintf(command, "UID SEARCH BEFORE \"%s\"\r\n", fromdate.c_str());
	bearer.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			result = false;
			break;
		}

		if (strcontains(resp.c_str(), "UID OK"))
		{
			result = true;
			break;
		}

		if (strcontains(resp.c_str(), "UID NO") || strcontains(resp.c_str(), "UID BAD"))
		{
			result = false;
			break;
		}

		uidlist = resp;
		strreplace(uidlist, "SEARCH", "*");
		strremove(uidlist, '*');
		stralltrim(uidlist);
	}

	return result;
}

bool ImapClient::GetEmailsRecent(std::string dirname, std::string& uidlist)
{
	std::string resp;
	std::vector<std::string> buffer;
	char command[128] = { 0 };
	memset(command, 0, 128);
	sprintf(command, "UID SEARCH RECENT\r\n");
	bearer.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			result = false;
			break;
		}

		if (strcontains(resp.c_str(), "UID OK"))
		{
			result = true;
			break;
		}

		if (strcontains(resp.c_str(), "UID NO") || strcontains(resp.c_str(), "UID BAD"))
		{
			result = false;
			break;
		}

		uidlist = resp;
		strreplace(uidlist, "SEARCH", "*");
		strremove(uidlist, '*');
		stralltrim(uidlist);
	}

	return result;
}

bool ImapClient::GetMessageHeader(std::string& uid, Mail& mail)
{
	char command[128] = { 0 };
	memset(command, 0, 128);
	sprintf(command, "UID FETCH %s (BODY[HEADER.FIELDS (DATE FROM SUBJECT TO CC BCC MESSAGE-ID)])\r\n", uid.c_str());

	std::string resp;
	std::vector<std::string> buffer;
	bearer.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			return false;
		}

		if (strcontains(resp.c_str(), "UID OK"))
		{
			result = true;
			break;
		}

		if (strcontains(resp.c_str(), "UID NO") || strcontains(resp.c_str(), "UID BAD"))
		{
			result = false;
			break;
		}

		buffer.push_back(resp);
	}

	if (result)
	{
		buffer.pop_back();
		buffer.pop_back();
		buffer.erase(buffer.begin());

		for (auto str : buffer)
		{
			std::string field_name, field_value;
			strsplit(str, ':', field_name, field_value, true);

			if (strcontains(field_name.c_str(), "Message-ID") || strcontains(field_name.c_str(), "Message-Id"))
			{
				stralltrim(field_value);
				mail.Header.SetMessageId(field_value);
			}

			if (strcontains(field_name.c_str(), "Subject"))
			{
				stralltrim(field_value);
				strreplace(field_value, '|', ',');
				mail.Header.SetSubject(field_value);
			}

			if (strcontains(field_name.c_str(), "Date"))
			{
				stralltrim(field_value);
				mail.Header.SetTimeStamp(field_value);
			}

			if (strcontains(field_name.c_str(), "From"))
			{
				stralltrim(field_value);
				mail.Header.SetFrom(field_value);
			}

			if (strcontains(field_name.c_str(), "To"))
			{
				stralltrim(field_value);
				mail.Header.AddtoToList(field_value);
			}

			if (strcontains(field_name.c_str(), "CC"))
			{
				stralltrim(field_value);
				mail.Header.AddtoCcList(field_value);
			}

			if (strcontains(field_name.c_str(), "BCC"))
			{
				stralltrim(field_value);
				mail.Header.AddtoBccList(field_value);
			}
		}
	}

	return result;
}

bool ImapClient::GetMessageBody(std::string& uid, Mail& mail)
{
	char command[128] = { 0 };
	memset(command, 0, 128);
	sprintf(command, "UID FETCH %s (BODY[TEXT])\r\n", uid.c_str());

	std::string resp;
	std::string buffer;
	bearer.SendString(command);

	bool result = false;

	long linectr = 0;

	while (true)
	{
		if (!bearer.ReceiveString(resp, "\r\n"))
		{
			return false;
		}

		if (strcontains(resp.c_str(), "UID OK"))
		{
			result = true;
			break;
		}

		if (strcontains(resp.c_str(), "UID NO") || strcontains(resp.c_str(), "UID BAD"))
		{
			result = false;
			break;
		}

		if (linectr == 0)
		{
			linectr++;
			continue;
		}

		buffer += resp + "\r\n";
	}

	if (result)
	{
		mail.Serialize();
		mail.SerializedData += buffer;
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////

unsigned long getNumber(const std::string& str)
{
	std::string str_count;
	unsigned long num = -1;

	for (auto c : str)
	{
		if (isdigit(c))
		{
			str_count.push_back(c);
		}
	}	
	
	num = atol(str_count.c_str());
	return num;
}
