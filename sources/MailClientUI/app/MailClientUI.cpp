#include "MailClientUI.h"
#include "../../utils/StringEx.h"
#include "../../data/MailStorage.h"
#include "../../network/TcpClient.h"

MailClientUI* mailClientPtr = nullptr;

void DeSerializeProfile(Profile& prf, std::string& str);
void SerializeProfile(Profile& prf, std::string& str);

void mailBoxPoller(MailClientUI* appPtr);

MailClientUI::MailClientUI(int argc, char *argv[]) : QApplication (argc, argv)
{
    mailClientPtr = this;
    QPixmap pix(":images/splashscreen.png");
	applicationSpalsh.setPixmap(pix);
}

MailClientUI::~MailClientUI()
{
}

bool MailClientUI::InitializeDB()
{
	std::string curr_dir, parent_dir;

	dircurrentdirectory(curr_dir);
	dirgetparentdirectory(curr_dir, parent_dir);

	std::string dbcdir = parent_dir + "/db/";

	std::string mail_db_file = dbcdir + "Mails.db";
	std::string contact_db_file = dbcdir + "Contacts.db";

	std::string errmsg;

	if (!dirisdirectory(dbcdir))
	{
		dircreatedirectory(dbcdir);
		MailDatabase::CreateDatabase(mail_db_file);
		ContactDatabase::CreateDatabase(contact_db_file);
	}
	else
	{
		MailDatabase temp_mail_db;
		if (!temp_mail_db.OpenDatabase(mail_db_file, errmsg))
		{
			remove(mail_db_file.c_str());
			MailDatabase::CreateDatabase(mail_db_file);
		}
		else
		{
			temp_mail_db.CloseDatabase();
		}

		MailDatabase temp_contact_db;
		if (!temp_contact_db.OpenDatabase(contact_db_file, errmsg))
		{
			remove(contact_db_file.c_str());
			MailDatabase::CreateDatabase(contact_db_file);
		}
		else
		{
			temp_contact_db.CloseDatabase();
		}
	}

	return (mailDb.OpenDatabase(mail_db_file, errmsg) && contactDb.OpenDatabase(contact_db_file, errmsg));
}

bool MailClientUI::InitializeNetwork()
{
	TcpClient cl;

	std::string request = "GET / HTTP/1.1\r\nHost: api.ipify.org\r\nConnection: close\r\n\r\n";
	std::string response = "";

	if (cl.CreateSocket("api.ipify.org", 443, true))
	{
		int retcode = 0;
		if (cl.ConnectSocket(retcode))
		{
			cl.SendString(request);

			cl.ReceiveString(response);

			std::vector<std::string> tokens;

			strsplit(response, tokens, "\r\n", true);

			if (strcontains(tokens[0].c_str(), "200 OK"))
			{
				publicIpAddress = tokens[tokens.size() - 1];
			}
		}
	}

	std::thread pollerthread(mailBoxPoller, this);
	pollerthread.detach();
}

bool MailClientUI::InitializeUI()
{
	SettingsView* ptr = applicationWindow.Settings();

	auto a1 = connect(ptr, &SettingsView::SwitchToDarkTheme, this, &MailClientUI::eventSwitchToDarkTheme);
	auto a2 = connect(ptr, &SettingsView::SwitchToLightTheme, this, &MailClientUI::eventSwitchToLightTheme);

	return applicationWindow.Initialize();
}

void MailClientUI::ShowSpalsh()
{
	applicationSpalsh.show();
}

void MailClientUI::ShowSplashMessage(QString str)
{
	applicationSpalsh.showMessage(str);
}

void MailClientUI::ShowUI()
{
	applicationWindow.show();
}

void MailClientUI::eventSwitchToLightTheme()
{
	theme = Light;
	setAppThemeLight(this);
}

void MailClientUI::eventSwitchToDarkTheme()
{
	theme = Dark;
	setAppThemeDark(this);
}

ThemeSetting MailClientUI::Theme()
{
	return theme;
}

bool MailClientUI::FetchProfiles()
{
    return GetProfileList(profiles);
}

bool MailClientUI::FetchDirectories()
{
    for(Profile prf : profiles)
    {
		std::vector<std::string> dirs;

        if(!GetAccountDirectories(prf.ProfileName, dirs))
        {
            return false;
        }

        directories[prf.ProfileName] = dirs;
    }

    return true;
}

bool MailClientUI::FetchConfiguration()
{
	//std::string comm = "rushpriority_ui|GET CONFIGURATION\n";

	//mailClientQueue.WriteLine(comm);

	//std::string line;

	//while (mailClientQueue.ReadLine(line))
	//{
	//	if (line == "<END>")
	//	{
	//		break;
	//	}

	//	std::vector<std::string> tokens;

	//	strsplit(line, tokens, "::", true);

	//	if (tokens.size() == 2)
	//	{
	//		configuration[tokens[0]] = tokens[1];
	//	}
	//}

	return true;
}

bool MailClientUI::GetProfileList(std::vector<Profile> &ctlist)
{
	std::string response;

	if (mailDb.GetProfiles(response, ""))
	{
		std::vector<std::string> resplines;
		strsplit(response, resplines, '\n', true);

		for (auto pr : resplines)
		{
			Profile prf;
			DeSerializeProfile(prf, pr);

			ctlist.push_back(prf);
		}

		return true;
	}

    return false;
}

bool MailClientUI::GetProfileInformation(std::string &str, Profile &prf)
{
    std::string comm = "rushpriority_ui|PROFILE INFO|";
    comm += str;
    comm += "\n";

    mailClientQueue.WriteLine(comm);

    std::string line;

	std::vector<std::string> resplines;

    while(mailClientQueue.ReadLine(line))
    {
        if(line == "<END>")
        {
            break;
        }

        resplines.push_back(line.c_str());
    }

    if(resplines.size() < 2)
    {
        return false;
    }

    //Omit the header
	resplines.erase(resplines.begin(), resplines.begin() + 1);

	std::string data = resplines[0];
    DeSerializeProfile(prf, data);

    return true;}

bool MailClientUI::AddProfile(Profile &obj)
{
    std::string comm = "rushpriority_ui|PROFILE ADD|";
	std::string temp;
    SerializeProfile(obj, temp);
    comm += temp;
    comm += "\n";

    mailClientQueue.WriteLine(comm);

    std::string line;
	bool success = false;

	while (mailClientQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		if (line == "OK")
		{
			success = true;
		}
	}

	return success;
}

bool MailClientUI::UpdateProfile(Profile &obj)
{
    std::string comm = "rushpriority_ui|PROFILE UPDATE|";
    std::string temp;
    SerializeProfile(obj, temp);
    comm += temp;
    comm += "\n";

    mailClientQueue.WriteLine(comm);

    std::string line;
	bool success = false;

	while (mailClientQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		if (line == "OK")
		{
			success = true;
		}
	}

	return success;
}

bool MailClientUI::RemoveProfile(std::string &str)
{
    std::string comm = "rushpriority_ui|PROFILE REMOVE|";
    std::string temp;
    comm += str;
    comm += "\n";

    mailClientQueue.WriteLine(comm);

    std::string line;
	bool success = false;

	while (mailClientQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		if (line == "OK")
		{
			success = true;
		}
	}

	return success;
}

bool MailClientUI::SendEmail(Mail &eml, MailStorageInformation &stg)
{
	std::string eml_dir;

	eml_dir = mailClientPtr->KeyValue("emaildirectory");

	eml_dir += "/";
	eml_dir += stg.GetAccount();
	eml_dir += "/";

	if (!dirisdirectory(eml_dir))
	{
		dircreatedirectory(eml_dir);
	}

	std::string eml_out_dir;
	eml_out_dir += eml_dir;
	eml_out_dir += "OUTBOX/";

	if (!dirisdirectory(eml_out_dir))
	{
		dircreatedirectory(eml_out_dir);
	}

	MailStorage ms;

	eml.Serialize();

	ms.StoreMail(eml_out_dir, eml);

	std::string comm = "rushpriority_ui|SEND EMAIL|";
	comm += stg.GetAccount() + "|";
	comm += eml.Header.GetMessageId() + "|";
	comm += stg.GetUid() + "|";
	comm += eml.Header.GetSubject() + "|";
	comm += eml.Header.GetFrom() + "|";
	comm += eml.Header.GetTo() + "|";
	comm += eml.Header.GetCc() + "|";
	comm += eml.Header.GetBcc() + "|";
	comm += stg.GetDirectory() + "|";
	comm += stg.GetStatus() + "|";
	comm += eml.Header.GetTimeStamp() + "|";
	comm += "\n";

	mailClientQueue.WriteLine(comm);

	std::string line;

	bool sent = false;

	while (mailClientQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		if (line == "OK")
		{
			sent = true;
		}

		if (line == "ERROR")
		{
			sent = false;
		}
	}

	return sent;
}

bool MailClientUI::GetAccountDirectories(std::string &profilename, std::vector<std::string> &resplines)
{
    std::string comm = "rushpriority_ui|LIST DIRECTORIES|";
    comm += profilename;
    comm += "\n";

    mailClientQueue.WriteLine(comm);

    std::string line;

    resplines.clear();

    while(mailClientQueue.ReadLine(line))
    {
        if(line == "<END>")
        {
            break;
        }

        if(line == "OK")
        {
            continue;
        }

        resplines.push_back(line);
    }

    if(resplines.size() < 1)
    {
        return false;
    }

    return true;
}

bool MailClientUI::GetEmails(std::string &profilename, std::string &dirname, std::vector<MailHeader> &mails, std::vector<MailStorageInformation>& stgl)
{
    std::string comm = "rushpriority_ui|LIST EMAILS|";
    comm += profilename;
    comm += "|";
    comm += dirname;
    comm += "\n";

    mailClientQueue.WriteLine(comm);

    std::string line;

	bool header_read = false;

    while(mailClientQueue.ReadLine(line))
    {
        if(line == "<END>")
        {
            break;
        }

        if(line == "OK" || line == "ERROR")
        {
            continue;
        }

		if (!header_read)
		{
			header_read = true;
			continue;
		}

		MailHeader hdr;
		MailStorageInformation stg;

		std::vector<std::string> tokens;

		strsplit(line, tokens, '|');

		stg.SetAccount(tokens[0]);
		hdr.SetMessageId(tokens[1]);
		stg.SetUid(tokens[2]);
		hdr.SetSubject(tokens[3]);
		hdr.SetFrom(tokens[4]);
		hdr.AddtoToList(tokens[5]);
		hdr.AddtoCcList(tokens[6]);
		hdr.AddtoBccList(tokens[7]);
		stg.SetDirectory(tokens[8]);
		stg.SetStatus(tokens[9]);
		hdr.SetTimeStamp(tokens[10]);

		mails.push_back(hdr);
		stgl.push_back(stg);
	}

	if (mails.size() < 1)
	{
		return false;
	}

    return true;
}

bool MailClientUI::GetEmailsByTerm(std::string& profilename, std::string& dirname, std::string& term, std::vector<MailHeader>& mails, std::vector<MailStorageInformation>& stgl)
{
	std::string comm = "rushpriority_ui|SEARCH EMAILS|";
	comm += profilename;
	comm += "|";
	comm += dirname;
	comm += "|";
	comm += term;
	comm += "\n";

	mailClientQueue.WriteLine(comm);

	std::string line;

	bool header_read = false;

	while (mailClientQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		if (line == "OK" || line == "ERROR")
		{
			continue;
		}

		if (!header_read)
		{
			header_read = true;
			continue;
		}

		MailHeader hdr;
		MailStorageInformation stg;

		std::vector<std::string> tokens;

		strsplit(line, tokens, '|');

		stg.SetAccount(tokens[0]);
		hdr.SetMessageId(tokens[1]);
		stg.SetUid(tokens[2]);
		hdr.SetSubject(tokens[3]);
		hdr.SetFrom(tokens[4]);
		hdr.AddtoToList(tokens[5]);
		hdr.AddtoCcList(tokens[6]);
		hdr.AddtoBccList(tokens[7]);
		stg.SetDirectory(tokens[8]);
		stg.SetStatus(tokens[9]);
		hdr.SetTimeStamp(tokens[10]);

		mails.push_back(hdr);
		stgl.push_back(stg);
	}

	if (mails.size() < 1)
	{
		return false;
	}

	return true;
}

bool MailClientUI::GetEmailHeader(std::string &profilename, std::string &dirname, std::string &uid)
{
	std::string comm = "rushpriority_ui|EMAIL HEADER|";
	comm += profilename;
	comm += "|";
	comm += dirname;
	comm += "|";
	comm += uid;
	comm += "\n";

	mailClientQueue.WriteLine(comm);

	std::string line;
	bool success = false;

	while (mailClientQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		if (line == "OK")
		{
			success = true;
		}
	}

	return success;
}

bool MailClientUI::GetEmailBody(std::string &profilename, std::string &dirname, std::string & uid)
{
	std::string comm = "rushpriority_ui|EMAIL BODY|";
	comm += profilename;
	comm += "|";
	comm += dirname;
	comm += "|";
	comm += uid;
	comm += "\n";

	mailClientQueue.WriteLine(comm);

	std::string line;
	bool success = false;

	while (mailClientQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		if (line == "OK")
		{
			success = true;
		}
	}

	return success;
}

bool MailClientUI::RemoveEmail(std::string &profilename, std::string &dirname, std::string & uid, std::string& messageid)
{
	std::string comm = "rushpriority_ui|EMAIL REMOVE|";
	comm += profilename;
	comm += "|";
	comm += dirname;
	comm += "|";
	comm += uid;
	comm += "|";
	comm += messageid;
	comm += "\n";

	mailClientQueue.WriteLine(comm);

	std::string line;
	bool success = false;

	while (mailClientQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		if (line == "OK")
		{
			success = true;
		}
	}

	return success;
}

bool MailClientUI::FlagEmail(std::string &profilename, std::string &dirname, std::string &uid, std::string &flag)
{
	std::string comm = "rushpriority_ui|EMAIL FLAG|";
	comm += profilename;
	comm += "|";
	comm += dirname;
	comm += "|";
	comm += uid;
	comm += "|";
	comm += flag;
	comm += "\n";

	mailClientQueue.WriteLine(comm);

	std::string line;
	bool success = false;

	while (mailClientQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		if (line == "OK")
		{
			success = true;
		}
	}

	return success;
}

bool MailClientUI::MarkEmailSeen(std::string &profilename, std::string &dirname, std::string &uid)
{
	std::string comm = "rushpriority_ui|EMAIL MARK|";
	comm += profilename;
	comm += "|";
	comm += dirname;
	comm += "|";
	comm += uid;
	comm += "\n";

	mailClientQueue.WriteLine(comm);

	std::string line;
	bool success = false;

	while (mailClientQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		if (line == "OK")
		{
			success = true;
		}
	}

	return success;
}

bool MailClientUI::PurgeDeleted(std::string &profilename, std::string &dirname)
{
	std::string comm = "rushpriority_ui|DIRECTORY PURGE|";
	comm += profilename;
	comm += "|";
	comm += dirname;
	comm +=	"\n";

	mailClientQueue.WriteLine(comm);

	std::string line;
	bool success = false;

	while (mailClientQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		if (line == "OK")
		{
			success = true;
		}
	}

	return success;
}

bool MailClientUI::GetProfile(std::string &emailid, Profile &prof)
{
    for(auto prf : profiles)
    {
        if(prf.EMailId == emailid)
        {
            prof = prf;
            return true;
        }
    }

    return false;
}

std::string MailClientUI::KeyValue(std::string key)
{
	return configuration[key];
}

void MailClientUI::DeSerializeProfile(Profile &prf, std::string& str)
{
	std::vector<std::string> tokens;

	strsplit(str, tokens, '|');

	prf.ProfileName = tokens[0];
	prf.EMailId = tokens[1];
	prf.Password = tokens[2];

	prf.MailInServer = tokens[3];
	prf.MailInPort = tokens[4];
	prf.MailInSecurity = tokens[5];

	prf.MailOutServer = tokens[6];
	prf.MailOutPort = tokens[7];
	prf.MailOutSecurity = tokens[8];

	prf.LastSync = tokens[9];
}

void MailClientUI::SerializeProfile(Profile& prf, std::string& str)
{
	str.clear();

	str += prf.ProfileName;
	str += "|";
	str += prf.EMailId;
	str += "|";
	str += prf.Password;
	str += "|";
	str += prf.MailInServer;
	str += "|";
	str += prf.MailInPort;
	str += "|";
	str += prf.MailInSecurity;
	str += "|";
	str += prf.MailOutServer;
	str += "|";
	str += prf.MailOutPort;
	str += "|";
	str += prf.MailOutSecurity;
	str += "|";
	str += prf.LastSync;
}

bool MailClientUI::SearchContacts(std::vector<std::string>& ctlist, std::string& term)
{
	std::string comm = "address_book_client|SEARCH|";
	comm += term;
	comm += "\n";

	mailClientQueue.WriteLine(comm);

	std::string line;

	while (mailClientQueue.ReadLine(line))
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

bool MailClientUI::GetAllContacts(std::vector<std::string>& ctlist)
{
	std::string comm = "address_book_client|LIST\n";

	mailClientQueue.WriteLine(comm);

	std::string line;

	while (mailClientQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		ctlist.push_back(line.c_str());
	}

	if (ctlist.size() < 2)
	{
		return false;
	}

	ctlist.erase(ctlist.begin(), ctlist.begin() + 1);

	return true;
}

bool MailClientUI::GetContact(const std::string& contactId, Contact& obj)
{
	std::string comm = "address_book_client|INFO|";
	comm += contactId;
	comm += "\n";

	mailClientQueue.WriteLine(comm);

	std::string line;
	std::vector<std::string> ctlist;

	while (mailClientQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		ctlist.push_back(line.c_str());
	}

	if (ctlist.size() < 2)
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

bool MailClientUI::AddContact(const Contact& obj)
{
	std::string comm = "address_book_client|ADD|";
	std::string temp;
	SerializeContact(obj, temp);
	comm += temp;
	comm += "\n";

	mailClientQueue.WriteLine(comm);

	std::string line;

	bool ret = false;

	while (mailClientQueue.ReadLine(line))
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

bool MailClientUI::UpdateContact(const Contact& obj)
{
	std::string comm = "address_book_client|UPDATE|";
	std::string temp;
	SerializeContact(obj, temp);
	comm += temp;
	comm += "\n";

	mailClientQueue.WriteLine(comm);

	std::string line;

	bool ret = false;

	while (mailClientQueue.ReadLine(line))
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

bool MailClientUI::RemoveContact(const std::string& contactId)
{
	std::string comm = "address_book_client|REMOVE|";
	comm += contactId;
	comm += "\n";

	mailClientQueue.WriteLine(comm);

	std::string line;

	bool ret = false;

	while (mailClientQueue.ReadLine(line))
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

void MailClientUI::SerializeContact(const Contact& obj, std::string& str)
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

void mailBoxPoller(MailClientUI* appPtr)
{
	std::string current_dir, parent_dir, email_dir, profile_dir;

	dircurrentdirectory(current_dir);
	dirgetparentdirectory(current_dir, parent_dir);

	email_dir = parent_dir + "/emails/";

	if (!dirisdirectory(email_dir))
	{
		dircreatedirectory(email_dir);
	}

	while (true)
	{
		std::this_thread::sleep_for(5s);

		std::vector<Profile> plist;

		appPtr->MailDb()->GetProfiles(plist, "");

		for (auto p : plist)
		{
			profile_dir = email_dir + p.ProfileName;

			if (!dirisdirectory(profile_dir))
			{
				dircreatedirectory(profile_dir);
			}

			ImapClient imap;
			imap.SetAccountInformation(p.MailInServer, atoi(p.MailInPort.c_str()), p.EMailId, p.Password, (SecurityType)p.MailInSecurity[0]);

			if (!imap.Connect())
			{
				continue;
			}

			if (!imap.Login())
			{
				continue;
			}

			std::vector<std::string> dirlist;

			if (!imap.GetDirectoryList(dirlist))
			{
				continue;
			}

			for (auto dir : dirlist)
			{
				strremove(dir, '"');
				strremove(dir, '\\');

				std::string storage_dir = profile_dir + "/" + dir;

				if (!dirisdirectory(storage_dir))
				{
					dircreatedirectory(storage_dir);
				}

				imap.SelectDirectory(dir);

				std::string uidstr;
				std::vector<std::string> uidlist;

				long mail_count = appPtr->MailDb()->GetEmailCount(p.ProfileName, dir);

				if (mail_count > 0)
				{
					imap.GetEmailsRecent(dir, uidstr);
				}
				else
				{
					imap.GetEmailsAll(dir, uidstr);
				}

				strsplit(uidstr, uidlist, ' ', true);

				for (std::string str : uidlist)
				{
					Mail mail;
					MailStorageInformation inf;

					if (imap.GetMessageHeader(str, mail))
					{
						if (mail.Header.GetMessageId().length() < 1)
						{
							mail.Header.GenerateMessageId();
						}

						if (mail.Header.GetTimeStamp().length() < 1)
						{
							mail.Header.GenerateTimeStamp();
						}

						imap.GetMessageBody(str, mail);
						mail.Body.SetMessageId(mail.Header.GetMessageId());
						inf.SetAccount(p.ProfileName);
						inf.SetDirectory(dir);
						inf.SetUid(str);

						if (appPtr->MailDb()->CreateEmail(mail.Header, inf))
						{
							MailStorage stg;
							stg.StoreMail(storage_dir, mail);
						}
					}
				}
			}
		}
	}
}
