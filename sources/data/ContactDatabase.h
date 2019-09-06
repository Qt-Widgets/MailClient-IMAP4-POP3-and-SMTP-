#ifndef _CONTACT_DATABASE
#define _CONTACT_DATABASE

#include <string>
#include <list>
#include <vector>
#include <thread>
#include <mutex>
#include "Contact.h"
#include "../../3rdparty/sqlite3.h"

typedef std::list<std::string> Row;
typedef std::list<Row> DataSet;

class ContactDatabase
{
public:
	ContactDatabase();
    ~ContactDatabase();

    bool CreateContact(std::vector<std::string> &fieldlist);
    bool UpdateContact(std::vector <std::string> &fieldlist, const std::string & emailid);
    bool RemoveContact(const std::string & emailid);
    bool GetContacts(std::string &contactlist, const std::string & emailid);
    bool GetContacts(std::list<Contact> &contactlist, const std::string &emailid);
	bool GetContactList(std::string& contactlist);
	bool SearchContacts(std::string& contactlist, const std::string& term);

	bool OpenDatabase(const std::string& dbfilename, std::string& errmsg);
	bool CloseDatabase();
	bool IsOpen();
	static bool CreateDatabase(std::string &str);
	static DataSet set;
private:
	bool ExecuteDML(const char* sqlstr, std::string& errmsg);
	bool ExecuteSQL(const char* sqlstr, std::string& errmsg);
	void Serialize(std::string &data);
	bool isOpen;
	std::string dbName;
	std::string username;
	std::string password;
	std::string dbFilename;
	sqlite3* dbPtr;
	std::mutex mutexRef;
};

#endif // DBCONNECTION_H
