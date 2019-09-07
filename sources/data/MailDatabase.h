#ifndef _MAIL_DATABASE
#define _MAIL_DATABASE

#include <string>
#include <list>
#include <vector>
#include <thread>
#include <mutex>
#include "Profile.h"
#include "Contact.h"
#include "../network/Mail.h"
#include "../3rdparty/sqlite3.h"

typedef std::list<std::string> Row;
typedef std::list<Row> DataSet;

class MailDatabase
{
public:
	MailDatabase();
    ~MailDatabase();

    bool CreateProfile(std::vector <std::string> &fieldlist);
    bool UpdateProfile(std::vector <std::string> &fieldlist, std::string profilename);
    bool DeleteProfile(std::string profilename);
    bool GetProfiles(std::string &profilelist, std::string profilename);
    bool GetProfiles(std::vector<Profile> &profilelist, std::string profilename);

	bool CreateEmail(MailHeader &hdr);
	bool CreateEmail(std::vector <std::string> &fieldlist);
    bool UpdateEmail(std::vector <std::string> &fieldlist, std::string objectid);
	bool DeleteEmail(std::string& profilename, std::string& directory, std::string& uid);
	bool GetEmail(std::string &emailist, std::string &messageid);
    bool GetEmails(std::string &emailist, std::string &profileid, std::string &folder);
	bool GetEmailsByTerm(std::string& emailist, std::string& profileid, std::string& folder, std::string& term);
	long GetEmailCount(std::string& profileid, std::string& folder);

	bool OpenDatabase(const std::string& dbfilename, std::string& errmsg);
	bool CloseDatabase();
	bool IsOpen();
	static bool CreateDatabase(std::string str);
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
