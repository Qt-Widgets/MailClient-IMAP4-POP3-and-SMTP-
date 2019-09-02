#include "AddressBookService.h"
#include "../../utils/StringEx.h"
#include <iostream>

std::string error_str = "ERROR\n";
std::string success_str = "OK\n";
std::string resp_term = "<END>\n";

void ListRequestStream(AddressBookService *appPtr);

AddressBookService::AddressBookService(int argc, char *argv[])
{
	appName = argv[0];
	Logger::GetInstance()->SetModuleName(argv[0]);
	Logger::GetInstance()->StartLogging();
}

AddressBookService::~AddressBookService()
{
}

bool AddressBookService::Initialize()
{
	std::string curr_dir, parent_dir;

	dircurrentdirectory(curr_dir);
	dirgetparentdirectory(curr_dir, parent_dir);

	std::string dbcdir = parent_dir + "/db/";

	std::string db_file = dbcdir + "AddressBook.db";

	std::string errmsg;

	if (!dirisdirectory(dbcdir))
	{
		dircreatedirectory(dbcdir);
		ContactDatabase::CreateDatabase(db_file);
	}
	else
	{
		ContactDatabase tempdb;
		if (!tempdb.OpenDatabase(db_file, errmsg))
		{
			remove(db_file.c_str());
			ContactDatabase::CreateDatabase(db_file);
		}
		else
		{
			tempdb.CloseDatabase();
		}
	}

	return db.OpenDatabase(db_file, errmsg);
}

ContactDatabase* AddressBookService::DatabaseInstance()
{
	return &db;
}

void AddressBookService::StartListOnlyLoop()
{
	std::thread listprocthread(ListRequestStream, this);
	listprocthread.detach();
}

void AddressBookService::StartMainLoop()
{
	while (true)
	{
		if (!uiCmdStream.Open("addressbook", true))
		{
			return;
		}

		while (true)
		{
			std::string request;

			std::vector<std::string> params;

			if (!uiCmdStream.ReadLine(request))
			{
				break;
			}

			strsplit(request, params, '|', true);

			if (params.size() < 2)
			{
				continue;
			}

			std::string client_name = params[0];

			if (strcontains(request.c_str(), "QUIT"))
			{
				uiCmdStream.Close();
				return;
			}

			if (strcontains(request.c_str(), "LIST"))
			{
				GetContactList();
			}

			if (strcontains(request.c_str(), "INFO"))
			{
				if (params.size() == 3)
				{
					GetContactDetails(params[2]);
				}
			}

			if (strcontains(request.c_str(), "SEARCH"))
			{
				if (params.size() == 3)
				{
					GetContactDetailsByTerm(params[2]);
				}
			}

			if (strcontains(request.c_str(), "ADD"))
			{
				if (params.size() == 3)
				{
					CreateContact(params[2]);
				}
			}

			if (strcontains(request.c_str(), "UPDATE"))
			{
				if (params.size() == 3)
				{
					UpdateContact(params[2]);
				}
			}

			if (strcontains(request.c_str(), "REMOVE"))
			{
				if (params.size() == 3)
				{
					RemoveContact(params[2]);
				}
			}
		}

		uiCmdStream.Close();
	}
}

void AddressBookService::GetContactList()
{
	std::string response = "";

	if(db.GetContactList(response))
	{
		std::vector<std::string> resplines;
		strsplit(response, resplines, '\n', true);

		for (auto resp : resplines)
		{
			std::string temp = resp + "\n";
			uiCmdStream.WriteLine(temp);
		}
	}
	else
	{
		uiCmdStream.WriteLine(error_str);
	}
	uiCmdStream.WriteLine(resp_term);
}

void AddressBookService::GetContactDetails(std::string& contactId)
{
	std::string response = "";

	if (db.GetContacts(response, contactId))
	{	
		std::vector<std::string> resplines;
		strsplit(response, resplines, '\n', true);

		for (auto resp : resplines)
		{
			std::string temp = resp + "\n";
			uiCmdStream.WriteLine(temp);
		}
	}
	else
	{
		uiCmdStream.WriteLine(error_str);
	}
	uiCmdStream.WriteLine(resp_term);
}

void AddressBookService::GetContactDetailsByTerm(std::string& contactId)
{
	std::string response = "";

	if (db.SearchContacts(response, contactId))
	{
		std::vector<std::string> resplines;
		strsplit(response, resplines, '\n', true);

		for (auto resp : resplines)
		{
			std::string temp = resp + "\n";
			uiCmdStream.WriteLine(temp);
		}
	}
	else
	{
		uiCmdStream.WriteLine(error_str);
	}
	uiCmdStream.WriteLine(resp_term);
}


void AddressBookService::CreateContact(std::string& userinfo)
{
	std::vector<std::string> fieldlist;

	strsplit(userinfo, fieldlist, ',', false);

	if (db.CreateContact(fieldlist))
	{
		uiCmdStream.WriteLine(success_str);
	}
	else
	{
		uiCmdStream.WriteLine(error_str);
	}
	uiCmdStream.WriteLine(resp_term);
}

void AddressBookService::UpdateContact(std::string& userinfo)
{
	std::vector<std::string> fieldlist;

	strsplit(userinfo, fieldlist, ',', false);

	if (db.UpdateContact(fieldlist, fieldlist[0]))
	{
		uiCmdStream.WriteLine(success_str);
	}
	else
	{
		uiCmdStream.WriteLine(error_str);
	}
	uiCmdStream.WriteLine(resp_term);
}

void AddressBookService::RemoveContact(std::string& contactId)
{
	if (db.RemoveContact(contactId))
	{
		uiCmdStream.WriteLine(success_str);
	}
	else
	{
		uiCmdStream.WriteLine(error_str);
	}
	uiCmdStream.WriteLine(resp_term);
}

void ListRequestStream(AddressBookService* appPtr)
{
	CommQueue mailClientCmdStream;

	while (true)
	{
		if (!mailClientCmdStream.Open("addressbooklist", true))
		{
			return;
		}

		while (true)
		{
			std::string request;

			std::vector<std::string> params;

			if (!mailClientCmdStream.ReadLine(request))
			{
				break;
			}

			strsplit(request, params, '|', true);

			if (params.size() < 2)
			{
				continue;
			}

			std::string client_name = params[0];

			if (strcontains(request.c_str(), "QUIT"))
			{
				mailClientCmdStream.Close();
				return;
			}

			if (strcontains(request.c_str(), "LIST"))
			{
				std::string response = "";

				if (appPtr->DatabaseInstance()->GetContactList(response))
				{
					std::vector<std::string> resplines;
					strsplit(response, resplines, '\n', true);

					for (auto resp : resplines)
					{
						std::string temp = resp + "\n";
						mailClientCmdStream.WriteLine(temp);
					}
				}
				else
				{
					mailClientCmdStream.WriteLine(error_str);
				}
				mailClientCmdStream.WriteLine(resp_term);
			}
		}

		mailClientCmdStream.Close();
	}
}
