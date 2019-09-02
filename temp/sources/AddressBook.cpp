#include "AddressBook.h"
#include "../../uicommon/ThemeHandler.h"
#include "../../utils/StringEx.h"

AddressBook::AddressBook(int argc, char *argv[]) : QApplication (argc, argv)
{
    appWindow.SetOpsHandler(this);

	QMetaObject::Connection c1 = connect(&appWindow, &MainWindow::SwitchToDarkTheme, this, &AddressBook::eventSwitchToDarkTheme);
	QMetaObject::Connection c2 = connect(&appWindow, &MainWindow::SwitchToLightTheme, this, &AddressBook::eventSwitchToLightTheme);

	setAppThemeLight(this);
}

AddressBook::~AddressBook()
{
    requestQueue.Close();
}

void AddressBook::eventSwitchToLightTheme()
{
	setAppThemeLight(this);
}

void AddressBook::eventSwitchToDarkTheme()
{
	setAppThemeDark(this);
}

bool AddressBook::SetupCommQueue()
{
    if(!requestQueue.Open("addressbook", false))
    {
        return false;
    }

    return true;
}

void AddressBook::ShowWindow()
{
    appWindow.show();
}

bool AddressBook::SearchContacts(std::vector<std::string>& ctlist, std::string &term)
{
	std::string comm = "address_book_client|SEARCH|";
	comm += term;
	comm += "\n";

	requestQueue.WriteLine(comm);

	std::string line;

	while (requestQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		ctlist.push_back(line);
	}

	if (ctlist.size() < 2)
	{
		return false;
	}

	ctlist.erase(ctlist.begin(), ctlist.begin() + 1);

	return true;
}

bool AddressBook::GetAllContacts(std::vector<std::string> &ctlist)
{
    std::string comm = "address_book_client|LIST\n";

    requestQueue.WriteLine(comm);

    std::string line;

    while(requestQueue.ReadLine(line))
    {
        if(line == "<END>")
        {
            break;
        }

        ctlist.push_back(line.c_str());
    }

    if(ctlist.size() < 2)
    {
        return false;
    }    
	
	ctlist.erase(ctlist.begin(), ctlist.begin() + 1);

    return true;
}

bool AddressBook::GetContact(const std::string &contactId, Contact &obj)
{
    std::string comm = "address_book_client|INFO|";
    comm += contactId;
    comm += "\n";

    requestQueue.WriteLine(comm);

    std::string line;
	std::vector<std::string> ctlist;

    while(requestQueue.ReadLine(line))
    {
        if(line == "<END>")
        {
            break;
        }

        ctlist.push_back(line.c_str());
    }

    if(ctlist.size() < 2)
    {
        return false;
    }    
	
	ctlist.erase(ctlist.begin(), ctlist.begin() + 1);

	std::vector<std::string> fields;
	
	strsplit(ctlist[0], fields, ',');

    obj.EmailId = fields[0];
	obj.AlternateEmailIds = fields[1];

	obj.FirstName = fields[2];
	obj.MiddleName = fields[3];
	obj.LastName = fields[4];
	obj.WebSite = fields[5];

	obj.AddressLine1 = fields[6];
	obj.AddressLine2 = fields[7];
	obj.City = fields[8];
	obj.State = fields[9];
	obj.Country = fields[10];
	obj.ZipCode = fields[11];

	obj.WorkPhone = fields[12];
	obj.HomePhone = fields[13];
	obj.Fax = fields[14];
	obj.MobileNo = fields[15];
	obj.NickName = fields[16];

	obj.Anniversary = fields[17];
	obj.Notes = fields[18];
	obj.Photo = fields[19];

    return true;
}

bool AddressBook::AddContact(const Contact &obj)
{
    std::string comm = "address_book_client|ADD|";
    std::string temp;
    SerializeContact(obj, temp);
    comm += temp;
    comm += "\n";

    requestQueue.WriteLine(comm);

    std::string line;

	bool ret = false;

    while(requestQueue.ReadLine(line))
    {
        if(line == "<END>")
        {
            break;
        }

		if(line == "OK") 
		{       
			ret = true;
		}   
	}

    return ret;
}

bool AddressBook::UpdateContact(const Contact &obj)
{
    std::string comm = "address_book_client|UPDATE|";
    std::string temp;
    SerializeContact(obj, temp);
    comm += temp;
    comm += "\n";

    requestQueue.WriteLine(comm);

    std::string line;

	bool ret = false;

	while (requestQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		if (line == "OK")
		{
			ret = true;
		}
	}

	return ret;
}

bool AddressBook::RemoveContact(const std::string &contactId)
{
    std::string comm = "address_book_client|REMOVE|";
    comm += contactId;
    comm += "\n";

    requestQueue.WriteLine(comm);

    std::string line;

	bool ret = false;

	while (requestQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		if (line == "OK")
		{
			ret = true;
		}
	}

	return ret;
}

void AddressBook::SerializeContact(const Contact &obj, std::string &str)
{
    str.clear();

    str += obj.EmailId + ",";
	str += obj.AlternateEmailIds + ",";
	str += obj.FirstName + ",";
    str += obj.MiddleName + ",";
    str += obj.LastName + ",";
    str += obj.WebSite + ",";
    str += obj.AddressLine1 + ",";
    str += obj.AddressLine2 + ",";
    str += obj.City + ",";
    str += obj.State + ",";
    str += obj.Country + ",";
    str += obj.ZipCode + ",";
    str += obj.WorkPhone + ",";
    str += obj.HomePhone + ",";
    str += obj.Fax + ",";
    str += obj.MobileNo + ",";
    str += obj.NickName + ",";
    str += obj.Anniversary + ",";
    str += obj.Notes + ",";
    str += obj.Photo;
}
