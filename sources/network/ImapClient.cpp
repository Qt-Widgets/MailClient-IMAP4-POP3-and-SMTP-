#include <memory.h>
#include "../utils/StringEx.h"
#include "ImapClient.h"
#include "TcpClient.h"

class ImapBearer
{
public:
	TcpClient cl;
	ImapBearer()
	{
	}
};

unsigned long getNumber(const std::string& str);

ImapClient::ImapClient()
{
	host = "";
	username = "";
	password = "";
	port = 0;
	securityType = None;

	currentDirectory = "";
	_Error = "";

	bearerPtr = new ImapBearer();
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
	_Error = "";

	bearerPtr = new ImapBearer();
}

ImapClient::~ImapClient()
{
	if (bearerPtr)
	{
		bearerPtr->cl.CloseSocket();
	}
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

	if (bearerPtr->cl.CreateSocket(host.c_str(), port, need_ssl))
	{
		if (bearerPtr->cl.ConnectSocket(retcode))
		{
			return true;
		}
	}

	return false;
}

bool ImapClient::Disconnect()
{
	if (bearerPtr->cl.IsConnected())
	{
		return bearerPtr->cl.CloseSocket();
	}

	return false;
}

bool ImapClient::GetCapabilities()
{
	std::string resp;
	std::string capability = "CY CAPABILITY\r\n";
	bearerPtr->cl.SendString(capability);

	while (true)
	{
		if (!bearerPtr->cl.ReceiveString(resp, "\r\n"))
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

	bearerPtr->cl.SendString(login);

	while (true)
	{
		if (!bearerPtr->cl.ReceiveString(resp, "\r\n"))
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

	bearerPtr->cl.SendString(login);

	while (true)
	{
		if (!bearerPtr->cl.ReceiveString(resp, "\r\n"))
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
    return _Error;
}

std::string ImapClient::Account()
{
    return username;
}

bool ImapClient::Expunge(long msgno)
{
	std::string imapData;
	return false;
}

bool ImapClient::MarkAsSeen(long msgno)
{
	std::string imapData;
	return false;
}

bool ImapClient::DeleteMessage(long msgno)
{
	std::string imapData;
	return false;
}

bool ImapClient::FlagMessage(long msgno, std::string flag)
{
	std::string imapData;
	return false;
}

bool ImapClient::GetDirectoryList(std::vector<std::string>& dirList)
{
	std::string resp;

	std::string temp = std::string("LS LIST ") + std::string("\"\"") + std::string(" \"*\"") + std::string("\r\n");
	char command[128] = { 0 };
	sprintf(command, "%s", temp.c_str());

	bearerPtr->cl.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearerPtr->cl.ReceiveString(resp, "\r\n"))
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

bool ImapClient::GetDirectory(std::string dirname, unsigned long& emailCount, unsigned long &uidNext)
{
	currentDirectory = dirname;
	
	std::string resp;
	std::vector<std::string> buffer;
	char command[128] = { 0 };
	sprintf(command, "IN SELECT \"%s\"\r\n", currentDirectory.c_str());

	bearerPtr->cl.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearerPtr->cl.ReceiveString(resp, "\r\n"))
		{
			result = false;
			break;
		}

		if (strcontains(resp.c_str(), "IN OK"))
		{
			result = true;
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

bool ImapClient::getEmailsSince(std::string dirname, std::string& fromdate, std::string& uidlist)
{
	std::string resp;
	std::vector<std::string> buffer;
	char command[128] = { 0 };
	memset(command, 0, 128);
	sprintf(command, "UID SEARCH SINCE \"%s\"\r\n", fromdate.c_str());
	bearerPtr->cl.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearerPtr->cl.ReceiveString(resp, "\r\n"))
		{
			result = false;
			break;
		}

		if (strcontains(resp.c_str(), "UID OK"))
		{
			result = true;
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
	bearerPtr->cl.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearerPtr->cl.ReceiveString(resp, "\r\n"))
		{
			result = false;
			break;
		}

		if (strcontains(resp.c_str(), "UID OK"))
		{
			result = true;
			break;
		}

		uidlist = resp;
		strreplace(uidlist, "SEARCH", "*");
		strremove(uidlist, '*');
		stralltrim(uidlist);
	}

	return result;
}


bool ImapClient::GetMessageHeader(long uid)
{
	char command[128] = { 0 };
	memset(command, 0, 128);
	sprintf(command, "UID FETCH %d (BODY[HEADER.FIELDS (DATE FROM SUBJECT TO CC BCC MESSAGE-ID)])\r\n", uid);

	std::string resp;
	std::vector<std::string> buffer;
	bearerPtr->cl.SendString(command);

	bool result = false;

	while (true)
	{
		if (!bearerPtr->cl.ReceiveString(resp, "\r\n"))
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

	buffer.pop_back();
	buffer.pop_back();
	buffer.erase(buffer.begin());

	return result;
}

bool ImapClient::GetMessageBody(long uid)
{
	char command[128] = { 0 };
	memset(command, 0, 128);
	sprintf(command, "UID FETCH %d (BODY[TEXT])\r\n", uid);

	std::string resp;
	std::string buffer;
	bearerPtr->cl.SendString(command);

	bool result = false;

	long linectr = 0;

	while (true)
	{
		if (!bearerPtr->cl.ReceiveString(resp, "\r\n"))
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

		buffer += resp;
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
