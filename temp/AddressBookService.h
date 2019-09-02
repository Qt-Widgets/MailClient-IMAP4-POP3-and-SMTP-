#ifndef _ADDRESS_BOOK_SERVICE
#define _ADDRESS_BOOK_SERVICE

#include "../../utils/CommQueue.h"
#include "../../utils/Logger.h"
#include "../../utils/Directory.h"
#include "../../utils/StringEx.h"
#include "../../data/ContactDatabase.h"

class AddressBookService
{
public:
    explicit AddressBookService(int argc, char *argv[]);
    virtual ~AddressBookService();
	bool Initialize();
    void StartMainLoop();
	void StartListOnlyLoop();
	ContactDatabase* DatabaseInstance();
private:
	void GetContactList();
	void GetContactDetails(std::string& contactId);
	void GetContactDetailsByTerm(std::string& contactId);
	void CreateContact(std::string& userinfo);
	void UpdateContact(std::string& userinfo);
	void RemoveContact(std::string& contactId);

	CommQueue uiCmdStream;
	std::string appName;
	ContactDatabase db;
};

#endif
