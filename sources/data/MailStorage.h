#ifndef _MAIL_STORAGE
#define _MAIL_STORAGE

#include <string>
#include "../network/Mail.h"

using namespace std;

class MailStorage
{
public:
    MailStorage();
    ~MailStorage();
    bool StoreMail(std::string &path, Mail &email);
	bool RetrieveMail(std::string& path, Mail& email);
};

#endif
