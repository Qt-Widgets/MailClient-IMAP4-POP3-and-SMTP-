#include "RushPriority.h"
#include "../../utils/StringEx.h"
#include "../../network/TcpClient.h"

std::string error_str = "ERROR\n";
std::string success_str = "OK\n";
std::string resp_term = "<END>\n";

RushPriority* mailClientPtr = nullptr;

void mailBoxPoller(RushPriority* appPtr);

RushPriority::RushPriority(int argc, char *argv[])
{
    mailClientPtr = this;
	Logger::GetInstance()->SetModuleName(argv[0]);
	Logger::GetInstance()->StartLogging();
}

RushPriority::~RushPriority()
{
}

MailDatabase* RushPriority::MailDb()
{
	return &mailDb;
}

bool RushPriority::Initialize()
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

	std::string curr_dir, parent_dir;

	dircurrentdirectory(curr_dir);
	dirgetparentdirectory(curr_dir, parent_dir);

	std::string dbcdir = parent_dir + "/db/";

	std::string mail_db_file = dbcdir + "RushPriorityMails.db";
	std::string contact_db_file = dbcdir + "RushPriorityContacts.db";

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

void RushPriority::StartPollerLoop()
{
	std::thread pollerthread(mailBoxPoller, this);
	pollerthread.detach();
}

void RushPriority::StartMessageLoop()
{
	LoadProfiles();

	while (true)
	{
		if (!cmdStream.Open("rushpriority", true))
		{
			return;
		}

		while (true)
		{
			std::string request;

			std::vector<std::string> params;

			if (!cmdStream.ReadLine(request))
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
				cmdStream.Close();
				return;
			}

			if (strcontains(request.c_str(), "LIST PROFILES"))
			{
				GetProfileList();
			}

			if (strcontains(request.c_str(), "LIST CONFIG"))
			{
				LoadConfiguration();
			}

			if (strcontains(request.c_str(), "SAVE CONFIG"))
			{
				if (params.size() == 3)
				{
					SaveConfiguration(params[2]);
				}
			}

			if (strcontains(request.c_str(), "PROFILE INFO"))
			{
				if (params.size() == 3)
				{
					GetProfileInformation(params[2]);
				}
			}

			if (strcontains(request.c_str(), "PROFILE ADD"))
			{
				if (params.size() == 3)
				{
					AddProfile(params[2]);
				}
			}

			if (strcontains(request.c_str(), "PROFILE UPDATE"))
			{
				if (params.size() == 3)
				{
					UpdateProfile(params[2]);
				}
			}

			if (strcontains(request.c_str(), "PROFILE REMOVE"))
			{
				if (params.size() == 3)
				{
					RemoveProfile(params[2]);
				}
			}

			if (strcontains(request.c_str(), "SEND EMAIL"))
			{
				if (params.size() == 13)
				{
					SendEmail(params);
				}
			}

			if (strcontains(request.c_str(), "LIST DIRECTORIES"))
			{
				if (params.size() == 3)
				{
					GetAccountDirectories(params[2]);
				}
			}

			if (strcontains(request.c_str(), "LIST EMAILS"))
			{
				if (params.size() == 4)
				{
					GetEmails(params[2], params[3]);
				}
			}

			if (strcontains(request.c_str(), "SEARCH EMAILS"))
			{
				if (params.size() == 5)
				{
					GetEmailsByTerm(params[2], params[3], params[4]);
				}
			}

			if (strcontains(request.c_str(), "EMAIL HEADER"))
			{
				if (params.size() == 5)
				{
					GetEmailHeader(params[2], params[3], params[4]);
				}
			}

			if (strcontains(request.c_str(), "EMAIL BODY"))
			{
				if (params.size() == 5)
				{
					GetEmailBody(params[2], params[3], params[4]);
				}
			}

			if (strcontains(request.c_str(), "EMAIL REMOVE"))
			{
				if (params.size() == 6)
				{
					RemoveEmail(params[2], params[3], params[4], params[5]);
				}
			}

			if (strcontains(request.c_str(), "EMAIL FLAG"))
			{
				if (params.size() == 6)
				{
					FlagEmail(params[2], params[3], params[4], params[5]);
				}
			}

			if (strcontains(request.c_str(), "EMAIL MARK"))
			{
				if (params.size() == 5)
				{
					MarkEmail(params[2], params[3], params[4]);
				}
			}

			if (strcontains(request.c_str(), "DIRECTORY PURGE"))
			{
				if (params.size() == 4)
				{
					PurgeDeleted(params[2], params[3]);
				}
			}

			if (strcontains(request.c_str(), "CONATCT LIST"))
			{
				GetContactList();
			}

			if (strcontains(request.c_str(), "CONATCT INFO"))
			{
				if (params.size() == 3)
				{
					GetContactDetails(params[2]);
				}
			}

			if (strcontains(request.c_str(), "CONATCT ADD"))
			{
				if (params.size() == 3)
				{
					CreateContact(params[2]);
				}
			}

			if (strcontains(request.c_str(), "CONATCT UPDATE"))
			{
				if (params.size() == 3)
				{
					UpdateContact(params[2]);
				}
			}

			if (strcontains(request.c_str(), "CONATCT REMOVE"))
			{
				if (params.size() == 3)
				{
					RemoveContact(params[2]);
				}
			}

			if (strcontains(request.c_str(), "GET CONFIGURATION"))
			{
				GetConfiguration();
			}

			if (strcontains(request.c_str(), "SAVE CONFIGURATION"))
			{
				if (params.size() == 3)
				{
					SaveConfiguration(params[2]);
				}
			}
		}

		cmdStream.Close();
	}
}

void RushPriority::GetProfileList()
{
	std::string response = "";

	if (mailDb.GetProfiles(response, ""))
	{
		std::vector<std::string> resplines;
		strsplit(response, resplines, '\n', true);

		for (auto resp : resplines)
		{
			std::string temp = resp + "\n";
			cmdStream.WriteLine(temp);
		}
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}

	cmdStream.WriteLine(resp_term);
}

void RushPriority::GetProfileInformation(std::string& str)
{
	std::string response = "";

	if (mailDb.GetProfiles(response, str))
	{
		std::vector<std::string> resplines;
		strsplit(response, resplines, '\n', true);

		for (auto resp : resplines)
		{
			std::string temp = resp + "\n";
			cmdStream.WriteLine(temp);
		}
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}

	cmdStream.WriteLine(resp_term);
}

void RushPriority::AddProfile(std::string& str)
{
	std::string response = str;
	std::vector<std::string> fields;
	strsplit(str, fields, '|', true);

	if (mailDb.CreateProfile(fields))
	{
		LoadProfiles();

		cmdStream.WriteLine(success_str);
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}

	cmdStream.WriteLine(resp_term);
}

void RushPriority::UpdateProfile(std::string& str)
{
	std::string response = str;
	std::vector<std::string> fields;
	strsplit(str, fields, '|', true);

	if (mailDb.UpdateProfile(fields, fields[0]))
	{
		LoadProfiles();

		cmdStream.WriteLine(success_str);
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}

	cmdStream.WriteLine(resp_term);
}

void RushPriority::RemoveProfile(std::string& str)
{
	if (mailDb.DeleteProfile(str))
	{
		LoadProfiles();

		cmdStream.WriteLine(success_str);
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}

	cmdStream.WriteLine(resp_term);
}

void RushPriority::SendEmail(std::vector<std::string>& strlist)
{
	bool send_success = false;

	Mail mail;
	MailStorageInformation stg;

	stg.SetAccount(strlist[2]);
	mail.Header.SetMessageId(strlist[3]);
	stg.SetUid(strlist[4]);
	mail.Header.SetSubject(strlist[5]);
	mail.Header.SetFrom(strlist[6]);
	mail.Header.AddtoToList(strlist[7]);
	mail.Header.AddtoCcList(strlist[8]);
	mail.Header.AddtoBccList(strlist[9]);
	stg.SetDirectory(strlist[10]);
	stg.SetStatus(strlist[11]);
	mail.Header.SetTimeStamp(strlist[12]);

	Profile pr = profileList[strlist[2]];

	std::string current_dir, parent_dir, email_dir, profile_dir;

	dircurrentdirectory(current_dir);
	dirgetparentdirectory(current_dir, parent_dir);

	email_dir = parent_dir + "/emails/";
	profile_dir += email_dir + strlist[2];

	std::string eml_out_dir;
	eml_out_dir += profile_dir;
	eml_out_dir += "/OUTBOX/";

	MailStorage ms;

	mail.Body.SetMessageId(mail.Header.GetMessageId());
	ms.RetrieveMail(eml_out_dir, mail);
	
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
							if (smtp.SendMail(mail))
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
					if (smtp.SendMail(mail))
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
		cmdStream.WriteLine(success_str);
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}

	cmdStream.WriteLine(resp_term);
}

void RushPriority::GetAccountDirectories(std::string& str)
{
	Profile pr = profileList[str];

	ImapClient imap;
	imap.SetAccountInformation(pr.MailInServer, atoi(pr.MailInPort.c_str()), pr.EMailId, pr.Password, (SecurityType)pr.MailInSecurity[0]);

	std::vector<std::string> dirlist;

	bool err = true;

	if (imap.Connect())
	{
		if (imap.Login())
		{
			if (imap.GetDirectoryList(dirlist))
			{
				for (auto dir : dirlist)
				{
					std::string str = dir;
					str += "\n";
					cmdStream.WriteLine(str);
				}

				imap.Disconnect();

				err = false;
			}
		}
	}

	if (err)
	{
		cmdStream.WriteLine(error_str);
	}
	else
	{
		cmdStream.WriteLine(success_str);
	}

	cmdStream.WriteLine(resp_term);
}

void RushPriority::GetEmails(std::string& str1, std::string& str2)
{
	Profile pf = profileList[str1];

	std::string resp = "";
	bool err = true;

	if (mailDb.GetEmails(resp, pf.ProfileName, str2))
	{
		if (resp.length() > 0)
		{
			err = false;
			cmdStream.WriteLine(resp);
		}
	}

	if (err)
	{
		cmdStream.WriteLine(error_str);
	}
	else
	{
		cmdStream.WriteLine(success_str);
	}

	cmdStream.WriteLine(resp_term);
}

void RushPriority::GetEmailsByTerm(std::string& str1, std::string& str2, std::string& str3)
{
	Profile pf = profileList[str1];

	std::string resp = "";
	bool err = true;

	if (mailDb.GetEmailsByTerm(resp, pf.ProfileName, str2, str3))
	{
		if (resp.length() > 0)
		{
			err = false;
			cmdStream.WriteLine(resp);
		}
	}

	if (err)
	{
		cmdStream.WriteLine(error_str);
	}
	else
	{
		cmdStream.WriteLine(success_str);
	}

	cmdStream.WriteLine(resp_term);
}

void RushPriority::GetEmailHeader(std::string& profilename, std::string& directory, std::string& uid)
{
	Profile pr = profileList[profilename];

	ImapClient imap;
	imap.SetAccountInformation(pr.MailInServer, atoi(pr.MailInPort.c_str()), pr.EMailId, pr.Password, (SecurityType)pr.MailInSecurity[0]);

	if (imap.Connect())
	{
		if (imap.Login())
		{
			if (imap.SelectDirectory(directory))
			{
				Mail mail;
				MailStorageInformation inf;

				if (imap.GetMessageHeader(uid, mail))
				{
					inf.SetAccount(pr.ProfileName);
					inf.SetDirectory(directory);
					inf.SetUid(uid);
					cmdStream.WriteLine(success_str);
				}
			}
		}
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}
	cmdStream.WriteLine(resp_term);
}

void RushPriority::GetEmailBody(std::string& profilename, std::string& directory, std::string& uid)
{
	Profile pr = profileList[profilename];

	ImapClient imap;
	imap.SetAccountInformation(pr.MailInServer, atoi(pr.MailInPort.c_str()), pr.EMailId, pr.Password, (SecurityType)pr.MailInSecurity[0]);

	if (imap.Connect())
	{
		if (imap.Login())
		{
			if (imap.SelectDirectory(directory))
			{
				Mail mail;
				MailStorageInformation inf;

				if (imap.GetMessageHeader(uid, mail))
				{
					inf.SetAccount(pr.ProfileName);
					inf.SetDirectory(directory);
					inf.SetUid(uid);

					imap.GetMessageBody(uid, mail);
					mail.Body.SetMessageId(mail.Header.GetMessageId());
					cmdStream.WriteLine(success_str);
				}
			}
		}
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}
	cmdStream.WriteLine(resp_term);
}

void RushPriority::RemoveEmail(std::string& profilename, std::string& directory, std::string& uid, std::string& messageid)
{
	std::string current_dir, parent_dir, email_dir, profile_dir, email_file;

	dircurrentdirectory(current_dir);
	dirgetparentdirectory(current_dir, parent_dir);

	email_dir = parent_dir + "/emails/";
	profile_dir = email_dir + profilename;

	email_file = profile_dir;
	email_file += "/";
	email_file += directory;
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
			if (imap.SelectDirectory(directory))
			{
				if (imap.DeleteMessage(uid))
				{
					cmdStream.WriteLine(success_str);
					mailDb.DeleteEmail(profilename, directory, uid);
					remove(email_file.c_str());
				}
			}
		}
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}
	cmdStream.WriteLine(resp_term);
}

void RushPriority::FlagEmail(std::string& profilename, std::string& directory, std::string& uid, std::string & flag)
{
	Profile pr = profileList[profilename];

	ImapClient imap;
	imap.SetAccountInformation(pr.MailInServer, atoi(pr.MailInPort.c_str()), pr.EMailId, pr.Password, (SecurityType)pr.MailInSecurity[0]);

	if (imap.Connect())
	{
		if (imap.Login())
		{
			if (imap.SelectDirectory(directory))
			{
				if (imap.FlagMessage(uid, flag))
				{
					cmdStream.WriteLine(success_str);
				}
			}
		}
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}
	cmdStream.WriteLine(resp_term);
}

void RushPriority::MarkEmail(std::string& profilename, std::string& directory, std::string& uid)
{
	Profile pr = profileList[profilename];

	ImapClient imap;
	imap.SetAccountInformation(pr.MailInServer, atoi(pr.MailInPort.c_str()), pr.EMailId, pr.Password, (SecurityType)pr.MailInSecurity[0]);

	if (imap.Connect())
	{
		if (imap.Login())
		{
			if (imap.SelectDirectory(directory))
			{
				if (imap.MarkAsSeen(uid))
				{
					cmdStream.WriteLine(success_str);
				}
			}
		}
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}
	cmdStream.WriteLine(resp_term);
}

void RushPriority::PurgeDeleted(std::string& profilename, std::string& directory)
{
	Profile pr = profileList[profilename];

	ImapClient imap;
	imap.SetAccountInformation(pr.MailInServer, atoi(pr.MailInPort.c_str()), pr.EMailId, pr.Password, (SecurityType)pr.MailInSecurity[0]);

	if (imap.Connect())
	{
		if (imap.Login())
		{
			if (imap.SelectDirectory(directory))
			{
				if (imap.Expunge(directory))
				{
					cmdStream.WriteLine(success_str);
				}
			}
		}
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}
	cmdStream.WriteLine(resp_term);

}

void RushPriority::GetContactList()
{
	std::string response = "";

	if (contactDb.GetContactList(response))
	{
		std::vector<std::string> resplines;
		strsplit(response, resplines, '\n', true);

		for (auto resp : resplines)
		{
			std::string temp = resp + "\n";
			cmdStream.WriteLine(temp);
		}
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}
	cmdStream.WriteLine(resp_term);
}

void RushPriority::GetContactDetails(std::string& contactId)
{
	std::string response = "";

	if (contactDb.GetContacts(response, contactId))
	{
		std::vector<std::string> resplines;
		strsplit(response, resplines, '\n', true);

		for (auto resp : resplines)
		{
			std::string temp = resp + "\n";
			cmdStream.WriteLine(temp);
		}
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}
	cmdStream.WriteLine(resp_term);
}

void RushPriority::CreateContact(std::string& userinfo)
{
	std::vector<std::string> fieldlist;

	strsplit(userinfo, fieldlist, ',', false);

	if (contactDb.CreateContact(fieldlist))
	{
		cmdStream.WriteLine(success_str);
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}
	cmdStream.WriteLine(resp_term);
}

void RushPriority::UpdateContact(std::string& userinfo)
{
	std::vector<std::string> fieldlist;

	strsplit(userinfo, fieldlist, ',', false);

	if (contactDb.UpdateContact(fieldlist, fieldlist[0]))
	{
		cmdStream.WriteLine(success_str);
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}
	cmdStream.WriteLine(resp_term);
}

void RushPriority::RemoveContact(std::string& contactId)
{
	if (contactDb.RemoveContact(contactId))
	{
		cmdStream.WriteLine(success_str);
	}
	else
	{
		cmdStream.WriteLine(error_str);
	}
	cmdStream.WriteLine(resp_term);
}

void RushPriority::LoadProfiles()
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

void RushPriority::LoadConfiguration()
{

}

void RushPriority::SaveConfiguration(std::string& str)
{

}

void RushPriority::GetConfiguration()
{
	std::string current_dir, parent_dir, email_dir, profile_dir;

	dircurrentdirectory(current_dir);
	dirgetparentdirectory(current_dir, parent_dir);

	email_dir = parent_dir + "/emails/";

	if (!dirisdirectory(email_dir))
	{
		dircreatedirectory(email_dir);
	}

	std::string resp;
	resp += "emaildirectory::";
	resp += email_dir;
	resp += "\n";

	cmdStream.WriteLine(resp);
	cmdStream.WriteLine(resp_term);
}

void mailBoxPoller(RushPriority* appPtr)
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
