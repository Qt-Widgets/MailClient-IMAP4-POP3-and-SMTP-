#ifndef PROFILE_H
#define PROFILE_H

#include <string>
#include "../utils/StringEx.h"

using namespace std;

class Profile
{
public:
    std::string ProfileName;
    std::string EMailId;
    std::string Password;

    std::string MailInServer;
    std::string MailInPort;
    std::string MailInSecurity;

    std::string MailOutServer;
    std::string MailOutPort;
    std::string MailOutSecurity;

    std::string LastSync;

	inline void DeSerialize(std::string& str)
	{
		std::vector<std::string> tokens;

		strsplit(str, tokens, ',');

		ProfileName = tokens[0];
		EMailId = tokens[1];
		Password = tokens[2];

		MailInServer = tokens[3];
		MailInPort = tokens[4];
		MailInSecurity = tokens[5];

		MailOutServer = tokens[6];
		MailOutPort = tokens[7];
		MailOutSecurity = tokens[8];

		LastSync = tokens[9];
	}

	inline void Serialize(std::string& str)
	{
		str.clear();

		str += ProfileName;
		str += ",";
		str += EMailId;
		str += ",";
		str += Password;
		str += ",";
		str += MailInServer;
		str += ",";
		str += MailInPort;
		str += ",";
		str += MailInSecurity;
		str += ",";
		str += MailOutServer;
		str += ",";
		str += MailOutPort;
		str += ",";
		str += MailOutSecurity;
		str += ",";
		str += LastSync;
	}

};

#endif
