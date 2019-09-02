#ifndef PROFILE_H
#define PROFILE_H

#include <string>

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
};

#endif
