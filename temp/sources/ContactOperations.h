#ifndef CONTACT_OPERATIONS
#define CONTACT_OPERATIONS

#include <QObject>
#include "../../data/Contact.h"

#include <vector>
#include <string>

using namespace std;

class ContactOperations
{
public:
    ContactOperations() {}
    virtual ~ContactOperations() {}
	virtual bool SearchContacts(std::vector<std::string> &ctlist, std::string &term) = 0;
    virtual bool GetAllContacts(std::vector<std::string> &ctlist) = 0;
    virtual bool GetContact(const std::string &contactId, Contact &obj) = 0;
    virtual bool AddContact(const Contact &obj) = 0;
    virtual bool UpdateContact(const Contact &obj) = 0;
    virtual bool RemoveContact(const std::string &contactId) = 0;
};

#endif
