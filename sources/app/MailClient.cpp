#include "MailClient.h"

MailClient* mailClientPtr = nullptr;

void mailBoxPoller(MailClient* appPtr);

MailClient::MailClient(int argc, char *argv[]) : QApplication (argc, argv)
{
    mailClientPtr = this;
    QPixmap pix(":images/splashscreen.png");
	applicationSpalsh.setPixmap(pix);
}

MailClient::~MailClient()
{
}

MailDatabase* MailClient::MailDb()
{
	return &mailDb;
}

bool MailClient::InitializeDB()
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

bool MailClient::InitializeNetwork()
{
	bool ret = false;

	TcpClient cl;

	std::string request = "GET / HTTP/1.1\r\nHost: api.ipify.org\r\nConnection: close\r\n\r\n";
	std::string response = "";

	if (/*cl.CreateSocket("api.ipify.org", 443, true)*/ cl.CreateSocket("54.225.92.64", 443, true))
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
				ret = true;
			}
		}
	}

	std::thread pollerthread(mailBoxPoller, this);
	pollerthread.detach();

	return ret;
}

bool MailClient::InitializeUI()
{
	SettingsView* ptr = applicationWindow.Settings();

	auto a1 = connect(ptr, &SettingsView::SwitchToDarkTheme, this, &MailClient::eventSwitchToDarkTheme);
	auto a2 = connect(ptr, &SettingsView::SwitchToLightTheme, this, &MailClient::eventSwitchToLightTheme);

	return applicationWindow.Initialize();
}

void MailClient::ShowSpalsh()
{
	applicationSpalsh.show();
}

void MailClient::ShowSplashMessage(QString str)
{
	applicationSpalsh.showMessage(str);
}

void MailClient::ShowUI()
{
	applicationWindow.show();
}

void MailClient::eventSwitchToLightTheme()
{
	theme = Light;
	setAppThemeLight(this);
}

void MailClient::eventSwitchToDarkTheme()
{
	theme = Dark;
	setAppThemeDark(this);
}

ThemeSetting MailClient::Theme()
{
	return theme;
}

bool MailClient::FetchProfiles()
{
    return GetProfileList(profiles);
}

bool MailClient::FetchDirectories()
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

bool MailClient::FetchConfiguration()
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

bool MailClient::GetProfileList(std::vector<Profile> &ctlist)
{
	std::string response = "";

	if (mailDb.GetProfiles(response, ""))
	{
		std::vector<std::string> resplines;
		strsplit(response, resplines, '\n', true);

		if (resplines.size() > 1)
		{
			for (auto resp : resplines)
			{
				Profile prf;
				prf.DeSerialize(resp);
				ctlist.push_back(prf);
			}

			ctlist.erase(ctlist.begin(), ctlist.begin() + 1);
		}

		return true;
	}

    return false;
}

bool MailClient::GetProfileInformation(std::string &str, Profile &prf)
{
	std::string response = "";

	if (mailDb.GetProfiles(response, str))
	{
		std::vector<std::string> resplines;
		strsplit(response, resplines, '\n', true);

		for (auto resp : resplines)
		{
			Profile prf;
			prf.DeSerialize(resp);
			return true;
		}
	}

    return false;
}

bool MailClient::AddProfile(Profile &obj)
{
	std::string str;
	obj.Serialize(str);
	std::vector<std::string> fields;
	strsplit(str, fields, ',', true);

	if (mailDb.CreateProfile(fields))
	{
		LoadProfiles();
		return true;
	}

	return false;
}

bool MailClient::UpdateProfile(Profile &obj)
{
	std::string str;
	obj.Serialize(str);
	std::vector<std::string> fields;
	strsplit(str, fields, ',', true);

	if (mailDb.UpdateProfile(fields, fields[0]))
	{
		LoadProfiles();
		return true;
	}

	return false;
}

bool MailClient::RemoveProfile(std::string &str)
{
	if (mailDb.DeleteProfile(str))
	{
		LoadProfiles();
		return true;
	}

	return false;
}

bool MailClient::SendEmail(Mail &eml, MailStorageInformation &stg)
{
	bool send_success = false;

	Profile pr = profileList[stg.GetAccount()];

	std::string current_dir, parent_dir, email_dir, profile_dir;

	dircurrentdirectory(current_dir);
	dirgetparentdirectory(current_dir, parent_dir);

	email_dir = parent_dir + "/emails/";
	profile_dir += email_dir + stg.GetAccount();

	std::string eml_out_dir;
	eml_out_dir += profile_dir;
	eml_out_dir += "/OUTBOX/";

	MailStorage ms;

	eml.Body.SetMessageId(eml.Header.GetMessageId());
	ms.RetrieveMail(eml_out_dir, eml);

	SmtpClient smtp;

	smtp.SetAccountInformation(pr.MailOutServer, atoi(pr.MailOutPort.c_str()), pr.EMailId, pr.Password, (SecurityType)pr.MailOutSecurity[0]);

	smtp.SetPublicIp(publicIpAddress);

	if (smtp.Connect())
	{
		if (smtp.SendHelo())
		{
			if (smtp.NeedTls())
			{
				if (smtp.StartTls())
				{
					if (smtp.SendHelo())
					{
						if (smtp.Login())
						{
							if (smtp.SendMail(eml))
							{
								send_success = true;
							}
							smtp.Logout();
						}
					}
				}
			}
			else
			{
				if (smtp.Login())
				{
					if (smtp.SendMail(eml))
					{
						send_success = true;
					}
					smtp.Logout();
				}
			}
		}
	}

	smtp.Disconnect();

	if (send_success)
	{
		return true;
	}

	return false;
}

bool MailClient::GetAccountDirectories(std::string &profilename, std::vector<std::string> &resplines)
{
	Profile pr = profileList[profilename];

	ImapClient imap;
	imap.SetAccountInformation(pr.MailInServer, atoi(pr.MailInPort.c_str()), pr.EMailId, pr.Password, (SecurityType)pr.MailInSecurity[0]);

	std::vector<std::string> dirlist;

	if (imap.Connect())
	{
		if (imap.Login())
		{
			if (imap.GetDirectoryList(dirlist))
			{
				for (auto dir : dirlist)
				{
					resplines.push_back(dir);
				}

				imap.Logout();
				imap.Disconnect();

				return true;
			}
		}
	}

	return false;
}

bool MailClient::GetEmails(std::string &profilename, std::string &dirname, std::vector<MailHeader> &mails, std::vector<MailStorageInformation>& stgl)
{
	Profile pf = profileList[profilename];

	std::string resp = "";
	bool err = true;

	if (mailDb.GetEmails(resp, pf.ProfileName, dirname))
	{
		if (resp.length() > 0)
		{
			MailHeader hdr;
			MailStorageInformation stg;

			std::vector<std::string> tokens;

			strsplit(resp, tokens, '|');

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


			return true;
		}
	}

	return true;
}

bool MailClient::GetEmailsByTerm(std::string& profilename, std::string& dirname, std::string& term, std::vector<MailHeader>& mails, std::vector<MailStorageInformation>& stgl)
{
	Profile pf = profileList[profilename];

	std::string resp = "";

	if (mailDb.GetEmailsByTerm(resp, pf.ProfileName, dirname, term))
	{
		if (resp.length() > 0)
		{
			MailHeader hdr;
			MailStorageInformation stg;

			std::vector<std::string> tokens;

			strsplit(resp, tokens, '|');

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

			return true;
		}
	}

	return false;
}

bool MailClient::GetEmailHeader(std::string &profilename, std::string &dirname, std::string &uid)
{
	Profile pr = profileList[profilename];

	ImapClient imap;
	imap.SetAccountInformation(pr.MailInServer, atoi(pr.MailInPort.c_str()), pr.EMailId, pr.Password, (SecurityType)pr.MailInSecurity[0]);

	if (imap.Connect())
	{
		if (imap.Login())
		{
			if (imap.SelectDirectory(dirname))
			{
				Mail mail;
				MailStorageInformation inf;

				if (imap.GetMessageHeader(uid, mail))
				{
					inf.SetAccount(pr.ProfileName);
					inf.SetDirectory(dirname);
					inf.SetUid(uid);
					imap.Logout();
					return true;
				}
			}
		}
	}

	return false;
}

bool MailClient::GetEmailBody(std::string &profilename, std::string &dirname, std::string & uid)
{
	Profile pr = profileList[profilename];

	ImapClient imap;
	imap.SetAccountInformation(pr.MailInServer, atoi(pr.MailInPort.c_str()), pr.EMailId, pr.Password, (SecurityType)pr.MailInSecurity[0]);

	if (imap.Connect())
	{
		if (imap.Login())
		{
			if (imap.SelectDirectory(dirname))
			{
				Mail mail;
				MailStorageInformation inf;

				if (imap.GetMessageHeader(uid, mail))
				{
					inf.SetAccount(pr.ProfileName);
					inf.SetDirectory(dirname);
					inf.SetUid(uid);

					imap.GetMessageBody(uid, mail);
					mail.Body.SetMessageId(mail.Header.GetMessageId());
					imap.Logout();
					return true;
				}
			}
		}
	}

	return false;
}

bool MailClient::RemoveEmail(std::string &profilename, std::string &dirname, std::string & uid, std::string& messageid)
{
	std::string current_dir, parent_dir, email_dir, profile_dir, email_file;

	dircurrentdirectory(current_dir);
	dirgetparentdirectory(current_dir, parent_dir);

	email_dir = parent_dir + "/emails/";
	profile_dir = email_dir + profilename;

	email_file = profile_dir;
	email_file += "/";
	email_file += dirname;
	email_file += "/";
	email_file += messageid;
	email_file += ".eml";
	strremove(email_file, '<');
	strremove(email_file, '>');

	Profile pr = profileList[profilename];

	ImapClient imap;
	imap.SetAccountInformation(pr.MailInServer, atoi(pr.MailInPort.c_str()), pr.EMailId, pr.Password, (SecurityType)pr.MailInSecurity[0]);

	if (imap.Connect())
	{
		if (imap.Login())
		{
			if (imap.SelectDirectory(dirname))
			{
				if (imap.DeleteMessage(uid))
				{
					mailDb.DeleteEmail(profilename, dirname, uid);
					remove(email_file.c_str());
					imap.Logout();
					return true;
				}
			}
		}
	}

	return false;
}

bool MailClient::FlagEmail(std::string &profilename, std::string &dirname, std::string &uid, std::string &flag)
{
	Profile pr = profileList[profilename];

	ImapClient imap;
	imap.SetAccountInformation(pr.MailInServer, atoi(pr.MailInPort.c_str()), pr.EMailId, pr.Password, (SecurityType)pr.MailInSecurity[0]);

	if (imap.Connect())
	{
		if (imap.Login())
		{
			if (imap.SelectDirectory(dirname))
			{
				if (imap.FlagMessage(uid, flag))
				{
					imap.Logout();
					return true;
				}
			}
		}
	}

	return false;
}

bool MailClient::MarkEmailSeen(std::string &profilename, std::string &dirname, std::string &uid)
{
	Profile pr = profileList[profilename];

	ImapClient imap;
	imap.SetAccountInformation(pr.MailInServer, atoi(pr.MailInPort.c_str()), pr.EMailId, pr.Password, (SecurityType)pr.MailInSecurity[0]);

	if (imap.Connect())
	{
		if (imap.Login())
		{
			if (imap.SelectDirectory(dirname))
			{
				if (imap.MarkAsSeen(uid))
				{
					imap.Logout();
					return true;
				}
			}
		}
	}

	return false;
}

bool MailClient::PurgeDeleted(std::string &profilename, std::string &dirname)
{
	Profile pr = profileList[profilename];

	ImapClient imap;
	imap.SetAccountInformation(pr.MailInServer, atoi(pr.MailInPort.c_str()), pr.EMailId, pr.Password, (SecurityType)pr.MailInSecurity[0]);

	if (imap.Connect())
	{
		if (imap.Login())
		{
			if (imap.SelectDirectory(dirname))
			{
				if (imap.Expunge(dirname))
				{
					imap.Logout();
					return true;
				}
			}
		}
	}

	return false;
}

bool MailClient::GetProfile(std::string &emailid, Profile &prof)
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

std::string MailClient::KeyValue(std::string key)
{
	return configuration[key];
}

bool MailClient::SearchContacts(std::vector<std::string>& ctlist, std::string& term)
{
	std::string response = "";

	if (contactDb.SearchContacts(response, term))
	{
		std::vector<std::string> resplines;
		strsplit(response, resplines, '\n', true);

		for (auto resp : resplines)
		{
			ctlist.push_back(resp);
		}

		return true;
	}

	return false;
}

bool MailClient::GetAllContacts(std::vector<std::string>& ctlist)
{
	std::string response = "";

	if (contactDb.GetContactList(response))
	{
		std::vector<std::string> resplines;
		strsplit(response, resplines, '\n', true);

		for (auto resp : resplines)
		{
			ctlist.push_back(resp);
		}

		return true;
	}

	return false;
}

bool MailClient::GetContact(const std::string& contactId, Contact& obj)
{
	std::string response = "";

	if (contactDb.GetContacts(response, contactId))
	{
		std::vector<std::string> resplines;
		strsplit(response, resplines, '\n', true);

		for (auto resp : resplines)
		{
			obj.DeSerialize(resp);
			return true;
		}
	}

	return false;
}

bool MailClient::AddContact(Contact& obj)
{
	std::string temp;
	obj.Serialize(temp);

	std::vector<std::string> fieldlist;

	strsplit(temp, fieldlist, ',', false);

	if (contactDb.CreateContact(fieldlist))
	{
		return true;
	}
	
	return false;
}

bool MailClient::UpdateContact(Contact& obj)
{
	std::string temp;
	obj.Serialize(temp);

	std::vector<std::string> fieldlist;

	strsplit(temp, fieldlist, ',', false);

	if (contactDb.UpdateContact(fieldlist, fieldlist[0]))
	{
		return true;
	}

	return false;
}

bool MailClient::RemoveContact(const std::string& contactId)
{
	if (contactDb.RemoveContact(contactId))
	{
		return true;
	}

	return false;
}

void MailClient::LoadProfiles()
{
	std::string current_dir, parent_dir, email_dir, profile_dir;

	dircurrentdirectory(current_dir);
	dirgetparentdirectory(current_dir, parent_dir);

	email_dir = parent_dir + "/emails/";

	if (!dirisdirectory(email_dir))
	{
		dircreatedirectory(email_dir);
	}

	profileList.clear();

	std::vector<Profile> plist;

	mailDb.GetProfiles(plist, "");

	for (auto p : plist)
	{
		profileList[p.ProfileName] = p;

		profile_dir = email_dir + p.ProfileName;

		if (!dirisdirectory(profile_dir))
		{
			dircreatedirectory(profile_dir);
		}
	}
}

void mailBoxPoller(MailClient* appPtr)
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
