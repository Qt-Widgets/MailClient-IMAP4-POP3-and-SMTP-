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

void normalizeGmailDirList(std::string& imapData, std::vector<std::string>& dirlist);
void normalizeOutlookDirList(std::string& imapData, std::vector<std::string>& dirlist);
void normalizeDoveCotDirList(std::string& imapData, std::vector<std::string>& dirlist);
void normalizeYahooDirList(std::string& imapData, std::vector<std::string>& dirlist);

ImapClient::ImapClient()
{
	_Host = "";
	_Username = "";
	_Password = "";
	_Port = 0;
	_SecurityType = 'N';

	_CurrentDirectory = "";
	_Error = "";

	_BearerPtr = new ImapBearer();
}

ImapClient::ImapClient(const std::string &host, uint16_t port, std::string username, std::string password, char sectype)
{
    if(sectype == 'A')
    {
        sectype = 'S';
    }

    _Host = host;
    _Port = port;
    _Username = username;
    _Password = password;
    _SecurityType = sectype;

	_CurrentDirectory = "";
	_Error = "";

	_BearerPtr = new ImapBearer();
}

ImapClient::~ImapClient()
{
	if (_BearerPtr)
	{
		_BearerPtr->cl.closeSocket();
	}
}

void ImapClient::setAccountInformation(const std::string &host, uint16_t port, std::string username, std::string password, char sectype)
{
    if(sectype == 'A')
    {
        sectype = 'S';
    }

    _Host = host;
    _Port = port;
    _Username = username;
    _Password = password;
    _SecurityType = sectype;
	_CurrentDirectory = "";

    return;
}

bool ImapClient::connect()
{
	if (_BearerPtr->cl.createSocket(_Host.c_str(), _Port, true))
	{
		int retcode;
		if (_BearerPtr->cl.connectSocket(retcode))
		{
			return true;
		}
	}

	return false;
}

bool ImapClient::disconnect()
{
	if (_BearerPtr->cl.isConnected())
	{
		return _BearerPtr->cl.closeSocket();
	}

	return false;
}

bool ImapClient::getCapabilities()
{
	std::string resp;
	std::string capability = "CY CAPABILITY\r\n";
	_BearerPtr->cl.sendString(capability);

	while (true)
	{
		if (!_BearerPtr->cl.receiveString(resp, "\r\n"))
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

bool ImapClient::login()
{
	std::string resp;

	std::string login = "LG LOGIN ";
	login += _Username + " ";
	login += _Password + "\r\n";

	_BearerPtr->cl.sendString(login);

	while (true)
	{
		if (!_BearerPtr->cl.receiveString(resp, "\r\n"))
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

std::string ImapClient::error()
{
    return _Error;
}

std::string ImapClient::account()
{
    return _Username;
}

bool ImapClient::expunge(long msgno)
{
	std::string imapData;
	return false;
}

bool ImapClient::markAsSeen(long msgno)
{
	std::string imapData;
	return false;
}

bool ImapClient::deleteMessage(long msgno)
{
	std::string imapData;
	return false;
}

bool ImapClient::flagMessage(long msgno, std::string flag)
{
	std::string imapData;
	return false;
}

bool ImapClient::getDirectoryList(std::vector<std::string>& dirList)
{
	std::string resp;

	std::string temp = std::string("LS LIST ") + std::string("\"\"") + std::string(" \"*\"") + std::string("\r\n");
	char command[128] = { 0 };
	sprintf(command, "%s", temp.c_str());

	_BearerPtr->cl.sendString(command);

	bool result = false;

	while (true)
	{
		if (!_BearerPtr->cl.receiveString(resp, "\r\n"))
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

		if (strcontains(resp.c_str(), "LS OK") || strcontains(resp.c_str(), "LIST completed"))
		{
			result = true;
			break;
		}		
		
		dirList.push_back(resp);
	}

	return result;
}

bool ImapClient::getDirectory(std::string dirname, unsigned long& emailCount, unsigned long &uidNext)
{
	_CurrentDirectory = dirname;
	
	std::string resp;
	std::vector<std::string> buffer;
	char command[128] = { 0 };
	sprintf(command, "IN SELECT \"%s\"\r\n", _CurrentDirectory.c_str());

	_BearerPtr->cl.sendString(command);

	bool result = false;

	while (true)
	{
		if (!_BearerPtr->cl.receiveString(resp, "\r\n"))
		{
			return false;
		}

		if (strcontains(resp.c_str(), "IN OK"))
		{
			result = true;
			break;
		}		
		
		buffer.push_back(resp);
	}

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

	return result;
}

bool ImapClient::getMessageHeader(long msgno)
{
	char command[128] = { 0 };
	memset(command, 0, 128);
	sprintf(command, "HD FETCH %ld (BODY[HEADER.FIELDS (MESSAGE-ID DATE FROM SUBJECT TO CC BCC FLAGS)])\r\n", msgno);

	std::string resp;
	std::vector<std::string> buffer;
	_BearerPtr->cl.sendString(command);

	bool result = false;

	while (true)
	{
		if (!_BearerPtr->cl.receiveString(resp, "\r\n"))
		{
			return false;
		}

		if (strcontains(resp.c_str(), "HD OK"))
		{
			result = true;
			break;
		}		
		
		buffer.push_back(resp);
	}

	return result;
}

bool ImapClient::getMessageBody(long msgno)
{
	char command[128] = { 0 };
	memset(command, 0, 128);
	sprintf(command, "BD FETCH %ld (BODY[TEXT])\r\n", msgno);

	std::string resp;
	std::string buffer;
	_BearerPtr->cl.sendString(command);

	bool result = false;

	while (true)
	{
		if (!_BearerPtr->cl.receiveString(resp, "\r\n"))
		{
			return false;
		}

		if (strcontains(resp.c_str(), "BD OK"))
		{
			result = true;
			break;
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

void normalizeGmailDirList(std::string& imapData, std::vector<std::string>& dirlist)
{

}

void normalizeOutlookDirList(std::string& imapData, std::vector<std::string>& dirlist)
{
	std::vector<std::string> temp;

	strsplit(imapData, temp, '\n', true);

	for (auto str : temp)
	{
		if (strsubstringpos(str.c_str(), "HasChildren") > -1)
		{
			continue;
		}

		dirlist.push_back(str);
	}
}

void normalizeDoveCotDirList(std::string& imapData, std::vector<std::string>& dirlist)
{

}

void normalizeYahooDirList(std::string& imapData, std::vector<std::string>& dirlist)
{

}
