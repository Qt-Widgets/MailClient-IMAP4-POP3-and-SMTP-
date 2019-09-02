#include "MailDatabase.h"
#include "../../utils/StringEx.h"
#include <memory.h>
#include <fstream>

DataSet MailDatabase::set;

std::string profile_sqlcreatestr = "create table Profiles \
            (	\
            ProfileName varchar(32) not null, \
            EMailId varchar(64) not null, \
            Password varchar(32) not null, \
            MailInServer varchar(32) not null, \
            MailInPort integer not null, \
            MailInSecurity char(1) not null, \
            MailOutServer varchar(32) not null, \
            MailOutPort integer not null, \
            MailOutSecurity char(1) not null, \
            LastSync varchar(12) not null, \
            primary key(EMailId) \
            )";

std::string mailbox_sqlcreatestr = "create table MailBox \
            (	\
            ProfileName varchar(32) not null, \
            MessageId varchar(256) not null, \
            UID varchar(256) not null, \
            Subject varchar(128), \
            FromUser varchar(64), \
            ToList varchar(1024), \
            CCList varchar(1024), \
            BCCList varchar(1024), \
            MailDirectory char(64) not null, \
            MailStatus char(1) not null, \
            MessageTimeStamp varchar(32), \
            primary key(ProfileName, MessageId) \
            )";

///////////////////////////////////////

std::string mailbox_sqlinsertstr = "insert into MailBox (ProfileName, MessageId, UID, Subject, FromUser, ToList, CCList, BCCList, MailDirectory, MailStatus, MessageTimeStamp) values ('ProfileName__', 'MessageId__', 'UID__', 'Subject__', 'FromUser__', 'ToList__', 'CCList__', 'BccList__', 'MailDirectory__', 'MailStatus__', 'MessageTimeStamp__')";

std::string profile_sqlinsertstr = "insert into Profiles (ProfileName, EMailId, Password, MailInServer, MailInPort, MailInSecurity, MailOutServer, MailOutPort, MailOutSecurity, LastSync) values ('ProfileName__', 'EMailId__', 'Password__', 'MailInServer__', MailInPort__, 'MailInSecurity__', 'MailOutServer__', MailOutPort__, 'MailOutSecurity__', 'LastSync__')";

///////////////////////////////

std::string profile_sqlupdatestr = "update Profiles set  ProfileName = 'ProfileName__', Password = 'Password__',  MailInServer = 'MailInServer__', MailInPort = MailInPort__, MailInSecurity = 'MailInSecurity__', MailOutServer = 'MailOutServer__', MailOutPort = MailOutPort__, MailOutSecurity = 'MailOutSecurity__' where EMailId = 'EMailId__'";

std::string mailbox_sqlupdatestr = "update MailBox set UID = 'UID__', Subject = 'Subject__', FromUser = 'FromUser__', ToList = 'ToList__', CCList = 'CCList__', BCCList = 'BccList__', MailDirectory = 'MailDirectory__', MailStatus = 'MailStatus__', MessageTimeStamp = 'MessageTimeStamp__' where ProfileName = 'ProfileName__' and MessageId = 'MessageId__'";

///////////////////////////////////////

std::string profile_sqldeletestr = "delete from Profiles where ProfileName = 'ProfileName__'";

std::string mailbox_sqldeletestr = "delete from MailBox where  ProfileName = 'ProfileName__' and MailDirectory = 'MailDirectory__' and UID = 'UID__'";

///////////////////////////////////////

std::string profile_sqlselectstr = "select * from Profiles";

std::string mailbox_sqlselectstr = "select * from MailBox";

static int SQLCallback(void* NotUsed, int argc, char** argv, char** azColName)
{
	int i;

	Row datarow;
	Row header;

	if (MailDatabase::set.size() < 1)
	{
		for (i = 0; i < argc; i++)
		{
			header.push_back(azColName[i]);
		}
	}

	MailDatabase::set.push_back(header);

	for (i = 0; i < argc; i++)
	{
		datarow.push_back(argv[i]);
	}

	MailDatabase::set.push_back(datarow);

	return 0;
}

MailDatabase::MailDatabase()
{
	dbPtr = nullptr;
	isOpen = false;
}

MailDatabase::~MailDatabase()
{
	if (isOpen)
	{
		CloseDatabase();
	}
}

bool MailDatabase::OpenDatabase(const std::string& dbfilename, std::string& errmsg)
{
	int rc;

	dbFilename = dbfilename;

	rc = sqlite3_open(dbfilename.c_str(), &dbPtr);

	if (rc)
	{
		return false;
	}

	isOpen = true;
	return true;
}

bool MailDatabase::CloseDatabase()
{
	if (dbPtr != nullptr)
	{
		sqlite3_close(dbPtr);
	}

	isOpen = false;
	return true;
}

bool MailDatabase::ExecuteDML(const char* sqlstr, std::string& errmsg)
{
	char* zErrMsg = nullptr;
	int rc;

	rc = sqlite3_exec(dbPtr, sqlstr, nullptr, nullptr, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		if (zErrMsg != nullptr)
		{
			errmsg = zErrMsg;
		}
		return false;
	}

	return true;
}

bool MailDatabase::ExecuteSQL(const char* sqlstr, std::string& errmsg)
{
	char* zErrMsg = nullptr;
	int rc;

	rc = sqlite3_exec(dbPtr, sqlstr, SQLCallback, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		if (zErrMsg != nullptr)
		{
			errmsg = zErrMsg;
		}
		return false;
	}

	return true;
}

bool MailDatabase::IsOpen()
{
	return isOpen;
}

bool MailDatabase::CreateDatabase(std::string str)
{
	std::string errmsg;
	int rc;
	char* zErrMsg = nullptr;

	sqlite3* dbnew;

	std::fstream file;
	file.open(str, fstream::out | fstream::trunc);
	file.close();

	rc = sqlite3_open(str.c_str(), &dbnew);

	if (rc)
	{
		return false;
	}

    rc = sqlite3_exec(dbnew, profile_sqlcreatestr.c_str(), nullptr, nullptr, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = zErrMsg;
		return false;
	}

    rc = sqlite3_exec(dbnew, mailbox_sqlcreatestr.c_str(), nullptr, nullptr, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = zErrMsg;
		return false;
	}

	sqlite3_close(dbnew);

	return true;
}

void MailDatabase::Serialize(std::string& data)
{
	data.clear();

	for (auto row : set)
	{
		int col = row.size();
		int index = 1;

		for (auto fieldval : row)
		{
			if (index >= col)
			{
				data += fieldval + "\n";
			}
			else
			{
				data += fieldval + "|";
				index++;
			}
		}
	}
}

bool MailDatabase::GetProfiles(std::string &profilelist, std::string profilename)
{
	mutexRef.lock();
	bool ret = false;
	set.clear();

    std::string sqlquery = profile_sqlselectstr;

    if (profilename.length() > 0)
	{
        sqlquery += " where ProfileName = '";
        sqlquery += profilename;
        sqlquery += "'";
	}

	std::string errMesg;

	if (ExecuteSQL(sqlquery.c_str(), errMesg))
	{
        Serialize(profilelist);
		ret = true;
	}

	set.clear();

	mutexRef.unlock();

	return ret;
}

bool MailDatabase::GetProfiles(std::vector<Profile>& profilelist, std::string profilename)
{
	mutexRef.lock();

	bool ret = false;
	set.clear();

	std::string sqlquery = profile_sqlselectstr;

	if (profilename.length() > 0)
	{
		sqlquery += " where ProfileName = '";
		sqlquery += profilename;
		sqlquery += "'";
	}

	std::string errMesg;

	if (ExecuteSQL(sqlquery.c_str(), errMesg))
	{
		if (set.size() < 2)
		{
			mutexRef.unlock();
			return false;
		}

		set.pop_front();

		for (auto row : set)
		{
			int col = row.size();

			if (col < 10)
			{
				continue;
			}

			int index = 0;

			Profile prf;

			for (auto fieldval : row)
			{
				switch (index)
				{
				case 0:
				{
					prf.ProfileName = fieldval;
					break;
				}
				case 1:
				{
					prf.EMailId = fieldval;
					break;
				}
				case 2:
				{
					prf.Password = fieldval;
					break;
				}
				case 3:
				{
					prf.MailInServer = fieldval;
					break;
				}
				case 4:
				{
					prf.MailInPort = fieldval;
					break;
				}
				case 5:
				{
					prf.MailInSecurity = fieldval;
					break;
				}
				case 6:
				{
					prf.MailOutServer = fieldval;
					break;
				}
				case 7:
				{
					prf.MailOutPort = fieldval;
					break;
				}
				case 8:
				{
					prf.MailOutSecurity = fieldval;
					break;
				}
				case 9:
				{
					prf.LastSync = fieldval;
					break;
				}
				default:
				{
					break;
				}
				}

				index++;
			}

			profilelist.push_back(prf);
		}
	}

	set.clear();

	mutexRef.unlock();

	return ret;
}

bool MailDatabase::CreateProfile(std::vector <std::string>& fieldlist)
{
	if (fieldlist.size() < 10)
	{
		return false;
	}

	mutexRef.lock();

	bool ret = false;

	std::string errmsg;

	std::string sql = profile_sqlinsertstr;

	strreplace(sql, "ProfileName__", fieldlist[0]);
	strreplace(sql, "EMailId__", fieldlist[1]);
	strreplace(sql, "Password__", fieldlist[2]);
	strreplace(sql, "MailInServer__", fieldlist[3]);
	strreplace(sql, "MailInPort__", fieldlist[4]);
	strreplace(sql, "MailInSecurity__", fieldlist[5]);
	strreplace(sql, "MailOutServer__", fieldlist[6]);
	strreplace(sql, "MailOutPort__", fieldlist[7]);
	strreplace(sql, "MailOutSecurity__", fieldlist[8]);
	strreplace(sql, "LastSync__", fieldlist[9]);

	ret = ExecuteDML(sql.c_str(), errmsg);

	mutexRef.unlock();

	return ret;
}

bool MailDatabase::UpdateProfile(std::vector <std::string>& fieldlist, std::string objectid)
{
	if (fieldlist.size() < 10)
	{
		return false;
	}

	mutexRef.lock();

	bool ret = false;

	if (fieldlist[0] == objectid)
	{
		std::string errmsg;

		std::string sql = profile_sqlupdatestr;

		strreplace(sql, "ProfileName__", fieldlist[0]);
		strreplace(sql, "EMailId__", fieldlist[1]);
		strreplace(sql, "Password__", fieldlist[2]);
		strreplace(sql, "MailInServer__", fieldlist[3]);
		strreplace(sql, "MailInPort__", fieldlist[4]);
		strreplace(sql, "MailInSecurity__", fieldlist[5]);
		strreplace(sql, "MailOutServer__", fieldlist[6]);
		strreplace(sql, "MailOutPort__", fieldlist[7]);
		strreplace(sql, "MailOutSecurity__", fieldlist[8]);
		strreplace(sql, "LastSync__", fieldlist[9]);

		ret = ExecuteDML(sql.c_str(), errmsg);
	}

	mutexRef.unlock();

	return ret;

}

bool MailDatabase::DeleteProfile(std::string profilename)
{
	mutexRef.lock();

	std::string errmsg;

	bool ret = false;

	std::string sql = profile_sqldeletestr;

	strreplace(sql, "ProfileName__", profilename);

	ret = ExecuteDML(sql.c_str(), errmsg);

	mutexRef.unlock();

	return ret;
}

bool MailDatabase::GetEmail(std::string &emailist, std::string &messageid)
{
	mutexRef.lock();

	bool ret = false;
	set.clear();
    std::string sqlquery;
    sqlquery = mailbox_sqlselectstr;

    if (messageid.length() > 0)
	{
        sqlquery += " where MessageId = '";
        sqlquery += messageid;
        sqlquery += "'";
	}

	std::string errMesg;

	if (ExecuteSQL(sqlquery.c_str(), errMesg))
	{
        Serialize(emailist);
		ret = true;
	}

	set.clear();

	mutexRef.unlock();

	return ret;
}

bool MailDatabase::GetEmailsByTerm(std::string& emailist, std::string& profilename, std::string& folder, std::string& term)
{
	mutexRef.lock();

	bool ret = false;
	set.clear();
	std::string sqlquery;
	sqlquery = mailbox_sqlselectstr;

	sqlquery += " where ";
	sqlquery += "ProfileName = '";
	sqlquery += profilename;
	sqlquery += "'";
	sqlquery += " and ";
	sqlquery += "MailDirectory = '";
	sqlquery += folder;
	sqlquery += "'";
	sqlquery += " and ";
	sqlquery += "Subject like '%";
	sqlquery += term;
	sqlquery += "%'";

	std::string errMesg;

	if (ExecuteSQL(sqlquery.c_str(), errMesg))
	{
		Serialize(emailist);
		ret = true;
	}

	set.clear();

	mutexRef.unlock();

	return ret;
}

bool MailDatabase::GetEmails(std::string &emailist, std::string &profilename, std::string &folder)
{
	mutexRef.lock();

	bool ret = false;
	set.clear();
	std::string sqlquery;
	sqlquery = mailbox_sqlselectstr;

	sqlquery += " where ";
	sqlquery += "ProfileName = '";
	sqlquery += profilename;
	sqlquery += "'";	
	sqlquery += " and ";
	sqlquery += "MailDirectory = '";
	sqlquery += folder;
	sqlquery += "'";

	std::string errMesg;

	if (ExecuteSQL(sqlquery.c_str(), errMesg))
	{
		Serialize(emailist);
		ret = true;
	}

	set.clear();

	mutexRef.unlock();

	return ret;
}

bool MailDatabase::CreateEmail(MailHeader& hdr, MailStorageInformation& stg)
{
	std::vector <std::string> fieldlist;

	fieldlist.push_back(stg.GetAccount());
	fieldlist.push_back(hdr.GetMessageId());
	fieldlist.push_back(stg.GetUid());
	fieldlist.push_back(hdr.GetSubject());
	fieldlist.push_back(hdr.GetFrom());
	fieldlist.push_back(hdr.GetTo());
	fieldlist.push_back(hdr.GetCc());
	fieldlist.push_back(hdr.GetBcc());
	fieldlist.push_back(stg.GetDirectory());
	fieldlist.push_back(stg.GetStatus());
	fieldlist.push_back(hdr.GetTimeStamp());

	return CreateEmail(fieldlist);
}

bool MailDatabase::CreateEmail(std::vector <std::string> &fieldlist)
{
	if (fieldlist.size() < 11)
	{
		return false;
	}

	mutexRef.lock();

	bool ret = false;

	std::string errmsg;

    std::string sql = mailbox_sqlinsertstr;

	strreplace(sql, "ProfileName__", fieldlist[0]);
	strreplace(sql, "MessageId__", fieldlist[1]);
	strreplace(sql, "UID__", fieldlist[2]);
	strreplace(sql, "Subject__", fieldlist[3]);
	strreplace(sql, "FromUser__", fieldlist[4]);
	strreplace(sql, "ToList__", fieldlist[5]);
	strreplace(sql, "CCList__", fieldlist[6]);
	strreplace(sql, "BccList__", fieldlist[7]);
	strreplace(sql, "MailDirectory__", fieldlist[8]);
	strreplace(sql, "MailStatus__", fieldlist[9]);
	strreplace(sql, "MessageTimeStamp__", fieldlist[10]);

	ret = ExecuteDML(sql.c_str(), errmsg);

	mutexRef.unlock();

	return ret;
}

bool MailDatabase::UpdateEmail(std::vector <std::string> &fieldlist, std::string objectid)
{
	if (fieldlist.size() < 11)
	{
		return false;
	}

	mutexRef.lock();

	bool ret = false;

	if (fieldlist[0] == objectid)
	{
		std::string errmsg;

        std::string sql = mailbox_sqlupdatestr;

        strreplace(sql, "ProfileName__", fieldlist[0]);
        strreplace(sql, "MessageId__", fieldlist[1]);
		strreplace(sql, "UID__", fieldlist[2]);
		strreplace(sql, "Subject__", fieldlist[3]);
        strreplace(sql, "FromUser__", fieldlist[4]);
        strreplace(sql, "ToList__", fieldlist[5]);
        strreplace(sql, "CCList__", fieldlist[6]);
        strreplace(sql, "BccList__", fieldlist[7]);
        strreplace(sql, "MailDirectory__", fieldlist[8]);
        strreplace(sql, "MailStatus__", fieldlist[9]);
        strreplace(sql, "MessageTimeStamp__", fieldlist[10]);

		ret = ExecuteDML(sql.c_str(), errmsg);
	}

	mutexRef.unlock();

	return ret;
}

bool MailDatabase::DeleteEmail(std::string& profilename, std::string& directory, std::string& uid)
{
	mutexRef.lock();

	std::string errmsg;

	bool ret = false;

	std::string sql = mailbox_sqldeletestr;

	strreplace(sql, "ProfileName__", profilename);
	strreplace(sql, "MailDirectory__", directory);
	strreplace(sql, "UID__", uid);

	ret = ExecuteDML(sql.c_str(), errmsg);

	mutexRef.unlock();

	return ret;
}

long MailDatabase::GetEmailCount(std::string& profilename, std::string& folder)
{
	mutexRef.lock();

	long ret = -1;
	set.clear();
	std::string sqlquery;

	sqlquery += "select count(1) from MailBox where ";
	sqlquery += "ProfileName = '";
	sqlquery += profilename;
	sqlquery += "'";
	sqlquery += " and ";
	sqlquery += "MailDirectory = '";
	sqlquery += folder;
	sqlquery += "'";

	std::string errMesg;

	if (ExecuteSQL(sqlquery.c_str(), errMesg))
	{
		std::string data;
		Serialize(data);

		if (set.size() > 1)
		{
			set.pop_front();
		}

		for (auto r : set)
		{
			for (auto c : r)
			{
				ret = atol(c.c_str());
				break;
			}
		}
	}

	set.clear();

	mutexRef.unlock();

	return ret;
}
