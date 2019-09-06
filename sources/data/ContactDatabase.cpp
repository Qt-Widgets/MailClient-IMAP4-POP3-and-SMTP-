#include "ContactDatabase.h"
#include "../utils/StringEx.h"
#include <memory.h>
#include <fstream>

std::string contact_sqlcreatestr = "create table Contacts \
            (	\
            EmailId varchar(32) not null, \
            AlternateEmailIds varchar(1024), \
            FirstName varchar(32) not null, \
            MiddleName varchar(32), \
            LastName varchar(32), \
            WebSite varchar(32), \
            AddressLine1 varchar(32), \
            AddressLine2 varchar(32), \
            City varchar(32), \
            State varchar(32), \
            Country varchar(32), \
            ZipCode varchar(32), \
            WorkPhone varchar(32), \
            HomePhone varchar(32), \
            Fax varchar(32), \
            MobileNo varchar(32), \
            NickName varchar(32), \
            Anniversary varchar(32), \
            Notes varchar(128), \
            Photo varchar(4096), \
            primary key(EmailId) \
            )";

///////////////////////////////////////

std::string contact_sqlinsertstr = "insert into Contacts (EmailId, AlternateEmailIds, FirstName, MiddleName, LastName, WebSite, AddressLine1, AddressLine2, City, State, Country, ZipCode, WorkPhone, HomePhone, Fax, MobileNo, NickName, Anniversary, Notes, Photo) values ('EmailId__', 'AlternateEmailIds__', 'FirstName__', 'MiddleName__', 'LastName__', 'WebSite__', 'AddressLine1__', 'AddressLine2__', 'City__', 'State__', 'Country__', 'ZipCode__', 'WorkPhone__', 'HomePhone__', 'Fax__', 'MobileNo__', 'NickName__', 'Anniversary__', 'Notes__', 'Photo__')";

///////////////////////////////

std::string contact_sqlupdatestr = "update Contacts set AlternateEmailIds = 'AlternateEmailIds__', FirstName = 'FirstName__',  MiddleName = 'MiddleName__', LastName = 'LastName__',  WebSite = 'WebSite__', AddressLine1 = 'AddressLine1__', AddressLine2 = 'AddressLine2__', City = 'City__', State = 'State__', Country = 'Country__', ZipCode = 'ZipCode__', WorkPhone = 'WorkPhone__', HomePhone = 'HomePhone__', Fax = 'Fax__', MobileNo = 'MobileNo__', NickName = 'NickName__', Anniversary = 'Anniversary__', Notes = 'Notes__', Photo = 'Photo__' where EmailId = 'EmailId__'";

///////////////////////////////////////

std::string contact_sqldeletestr = "delete from Contacts where EmailId = 'EmailId__'";

///////////////////////////////////////

std::string contact_sqlselectstr = "select * from Contacts";

///////////////////////////////////////

std::string contact_sqlsearchstr = "select EmailId, FirstName, MiddleName, LastName from Contacts where EmailId like '%_TERM_%' or FirstName like '%_TERM_%' or LastName like '%_TERM_%' or MiddleName like '%_TERM_%'";


DataSet ContactDatabase::set;

static int SQLCallback(void* NotUsed, int argc, char** argv, char** azColName)
{
	int i;

	Row datarow;
	Row header;

	if (ContactDatabase::set.size() < 1)
	{
		for (i = 0; i < argc; i++)
		{
			header.push_back(azColName[i]);
		}
	}

	if (header.size() > 0)
	{
		ContactDatabase::set.push_back(header);
	}

	for (i = 0; i < argc; i++)
	{
		if (argv[i] == NULL)
		{
			datarow.push_back(" ");
		}
		else
		{
			datarow.push_back(argv[i]);
		}
	}

	ContactDatabase::set.push_back(datarow);

	return 0;
}

ContactDatabase::ContactDatabase()
{
	dbPtr = nullptr;
	isOpen = false;
}

ContactDatabase::~ContactDatabase()
{
	if (isOpen)
	{
		CloseDatabase();
	}
}

bool ContactDatabase::OpenDatabase(const std::string& dbfilename, std::string& errmsg)
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

bool ContactDatabase::CloseDatabase()
{
	if (dbPtr != nullptr)
	{
		sqlite3_close(dbPtr);
	}

	isOpen = false;
	return true;
}

bool ContactDatabase::ExecuteDML(const char* sqlstr, std::string& errmsg)
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

bool ContactDatabase::ExecuteSQL(const char* sqlstr, std::string& errmsg)
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

bool ContactDatabase::IsOpen()
{
	return isOpen;
}

bool ContactDatabase::CreateDatabase(std::string &str)
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

    rc = sqlite3_exec(dbnew, contact_sqlcreatestr.c_str(), nullptr, nullptr, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		errmsg = zErrMsg;
		return false;
	}

	sqlite3_close(dbnew);

	return true;
}

void ContactDatabase::Serialize(std::string& data)
{
	data.clear();

	for (auto row : set)
	{
		int col = (int) row.size();
		int index = 1;

		for (auto fieldval : row)
		{
			if (index >= col)
			{
				data += fieldval + "\n";
			}
			else
			{
				data += fieldval + ",";
				index++;
			}
		}
	}
}

bool ContactDatabase::GetContactList(std::string& contactlist)
{
	mutexRef.lock();
	bool ret = false;
	set.clear();

	std::string errMesg;

	std::string sqlquery = "select EmailId, FirstName, MiddleName, LastName, Photo from Contacts";

	if (ExecuteSQL(sqlquery.c_str(), errMesg))
	{
		Serialize(contactlist);
		ret = true;
	}

	set.clear();

	mutexRef.unlock();

	return ret;
}

bool ContactDatabase::GetContacts(std::string &contactlist, const std::string &emailid)
{
	mutexRef.lock();
	bool ret = false;
	set.clear();

	std::string errMesg;

    std::string sqlquery = contact_sqlselectstr;

    if(emailid.length() > 0)
    {
        sqlquery += " where EmailId = '";
        sqlquery += emailid;
        sqlquery += "'";
    }

    if (ExecuteSQL(sqlquery.c_str(), errMesg))
	{
        Serialize(contactlist);
		ret = true;
	}

	set.clear();

	mutexRef.unlock();

	return ret;
}

bool ContactDatabase::CreateContact(std::vector <std::string> &fieldlist)
{
	if (fieldlist.size() < 20)
	{
		return false;
	}

	mutexRef.lock();

	bool ret = false;

	std::string errmsg;

    std::string sql = contact_sqlinsertstr;

	strreplace(sql, "EmailId__", fieldlist[0]);
	strreplace(sql, "AlternateEmailIds__", fieldlist[1]);
	strreplace(sql, "FirstName__", fieldlist[2]);
	strreplace(sql, "MiddleName__", fieldlist[3]);
	strreplace(sql, "LastName__", fieldlist[4]);
	strreplace(sql, "WebSite__", fieldlist[5]);
	strreplace(sql, "AddressLine1__", fieldlist[6]);
	strreplace(sql, "AddressLine2__", fieldlist[7]);
	strreplace(sql, "City__", fieldlist[8]);
	strreplace(sql, "State__", fieldlist[9]);
	strreplace(sql, "Country__", fieldlist[10]);
	strreplace(sql, "ZipCode__", fieldlist[11]);
	strreplace(sql, "WorkPhone__", fieldlist[12]);
	strreplace(sql, "HomePhone__", fieldlist[13]);
	strreplace(sql, "Fax__", fieldlist[14]);
	strreplace(sql, "MobileNo__", fieldlist[15]);
	strreplace(sql, "NickName__", fieldlist[16]);
	strreplace(sql, "Anniversary__", fieldlist[17]);
	strreplace(sql, "Notes__", fieldlist[18]);
	strreplace(sql, "Photo__", fieldlist[19]);

	ret = ExecuteDML(sql.c_str(), errmsg);		

	mutexRef.unlock();

	return ret;
}

bool ContactDatabase::UpdateContact(std::vector <std::string> &fieldlist, const std::string & emailid)
{
	if (fieldlist.size() < 20)
	{
		return false;
	}

	mutexRef.lock();

	bool ret = false;

	if (fieldlist[0] == emailid)
	{
		std::string errmsg;

        std::string sql = contact_sqlupdatestr;

		strreplace(sql, "EmailId__", fieldlist[0]);
		strreplace(sql, "AlternateEmailIds__", fieldlist[1]);
		strreplace(sql, "FirstName__", fieldlist[2]);
        strreplace(sql, "MiddleName__", fieldlist[3]);
        strreplace(sql, "LastName__", fieldlist[4]);
        strreplace(sql, "WebSite__", fieldlist[5]);
        strreplace(sql, "AddressLine1__", fieldlist[6]);
        strreplace(sql, "AddressLine2__", fieldlist[7]);
        strreplace(sql, "City__", fieldlist[8]);
        strreplace(sql, "State__", fieldlist[9]);
        strreplace(sql, "Country__", fieldlist[10]);
        strreplace(sql, "ZipCode__", fieldlist[11]);
        strreplace(sql, "WorkPhone__", fieldlist[12]);
        strreplace(sql, "HomePhone__", fieldlist[13]);
        strreplace(sql, "Fax__", fieldlist[14]);
        strreplace(sql, "MobileNo__", fieldlist[15]);
        strreplace(sql, "NickName__", fieldlist[16]);
        strreplace(sql, "Anniversary__", fieldlist[17]);
        strreplace(sql, "Notes__", fieldlist[18]);
        strreplace(sql, "Photo__", fieldlist[19]);

        ret = ExecuteDML(sql.c_str(), errmsg);
    }

	mutexRef.unlock();

	return ret;
}

bool ContactDatabase::RemoveContact(const std::string & emailid)
{
	mutexRef.lock();

	std::string errmsg;

	bool ret = false;

    std::string sql = contact_sqldeletestr;

    strreplace(sql, "EmailId__", emailid);

	ret = ExecuteDML(sql.c_str(), errmsg);

	mutexRef.unlock();

	return ret;
}

bool ContactDatabase::GetContacts(std::list<Contact> &contactlist, const std::string & emailid)
{
	mutexRef.lock();

	bool ret = false;
	set.clear();

    std::string sqlquery = contact_sqlselectstr;

    if(emailid.length() > 0)
    {
        sqlquery += " where EmailId = '";
        sqlquery += emailid;
        sqlquery += "'";
    }

	std::string errMesg;

	if (ExecuteSQL(sqlquery.c_str(), errMesg))
	{
		set.pop_front();

		for (auto row : set)
		{
			int col = (int) row.size();
			int index = 0;

            Contact con;

			for (auto fieldval : row)
			{
				switch (index)
				{
					case 0:
					{
                        con.EmailId = fieldval;
						break;
					}
					case 1:
					{
						con.AlternateEmailIds = fieldval;
						break;
					}
					case 2:
					{
                        con.FirstName = fieldval;
						break;
					}
					case 3:
					{
                        con.MiddleName = fieldval;
						break;
					}
					case 4:
					{
                        con.LastName = fieldval;
						break;
					}
					case 5:
					{
						con.WebSite = fieldval;
						break;
					}
					case 6:
					{
                        con.AddressLine1 = fieldval;
						break;
                    }
                    case 7:
                    {
                        con.AddressLine2 = fieldval;
                        break;
                    }
                    case 8:
                    {
                        con.City = fieldval;
                        break;
                    }
                    case 9:
                    {
                        con.State = fieldval;
                        break;
                    }
                    case 10:
                    {
                        con.Country = fieldval;
                        break;
                    }
                    case 11:
                    {
                        con.ZipCode = fieldval;
                        break;
                    }
                    case 12:
                    {
                        con.WorkPhone = fieldval;
                        break;
                    }
                    case 13:
                    {
                        con.HomePhone = fieldval;
                        break;
                    }
                    case 14:
                    {
                        con.Fax = fieldval;
                        break;
                    }
                    case 15:
                    {
                        con.MobileNo = fieldval;
                        break;
                    }
					case 16:
					{
						con.NickName = fieldval;
						break;
					}
                    case 17:
                    {
                        con.Anniversary = fieldval;
                        break;
                    }
                    case 18:
                    {
                        con.Notes = fieldval;
                        break;
                    }
                    case 19:
                    {
                        con.Photo = fieldval;
                        break;
                    }
                    default:
					{
						break;
					}
				}			
				
				index++;

			}

            contactlist.push_back(con);
		}
	}

	set.clear();
	mutexRef.unlock();

	return ret;
}

bool ContactDatabase::SearchContacts(std::string& contactlist, const std::string& term)
{
	mutexRef.lock();

	bool ret = false;
	set.clear();

	std::string sqlquery = contact_sqlsearchstr;

	strreplace(sqlquery, "_TERM", term);

	std::string errMesg;

	if (ExecuteSQL(sqlquery.c_str(), errMesg))
	{
		Serialize(contactlist);
		ret = true;
	}

	set.clear();
	mutexRef.unlock();

	return ret;
}
