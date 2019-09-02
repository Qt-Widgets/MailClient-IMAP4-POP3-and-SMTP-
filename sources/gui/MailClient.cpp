#include "MailClient.h"
#include "MainWindow.h"
#include "SettingsView.h"
#include "../../utils/StringEx.h"
#include "../../data/MailStorage.h"

MailClient* mailClientPtr = nullptr;

void DeSerializeProfile(Profile& prf, std::string& str);
void SerializeProfile(Profile& prf, std::string& str);

MailClient::MailClient(int argc, char *argv[]) : QApplication (argc, argv)
{
    mailClientPtr = this;
    QPixmap pix(":images/splashscreen.png");
	applicationSpalsh.setPixmap(pix);
}

MailClient::~MailClient()
{
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

bool MailClient::SetupCommQueue()
{
    mailQueue = mailRequestQueue.Open("rushpriority", false);

    contactQueue = contactRequestQueue.Open("addressbooklist", false);

    if(mailQueue && contactQueue)
        return true;
    else
        return false;
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
	std::string comm = "rushpriority_ui|GET CONFIGURATION\n";

	mailRequestQueue.WriteLine(comm);

	std::string line;

	while (mailRequestQueue.ReadLine(line))
	{
		if (line == "<END>")
		{
			break;
		}

		std::vector<std::string> tokens;

		strsplit(line, tokens, "::", true);

		if (tokens.size() == 2)
		{
			configuration[tokens[0]] = tokens[1];
		}
	}

	return true;
}

bool MailClient::GetAllContacts(std::vector<std::string> &ctlist)
{
    std::string comm = "rushpriority_ui|LIST\n";

    contactRequestQueue.WriteLine(comm);

    std::string line;

    while(contactRequestQueue.ReadLine(line))
    {
        if(line == "<END>")
        {
            break;
        }

        ctlist.push_back(line);
    }

	ctlist.erase(ctlist.begin(), ctlist.begin() + 1);

    if(ctlist.size() < 1)
    {
        return false;
    }

    return true;
}

bool MailClient::GetProfileList(std::vector<Profile> &ctlist)
{
    std::string comm = "rushpriority_ui|LIST PROFILES\n";

    mailRequestQueue.WriteLine(comm);

    std::string line;

	std::vector<std::string> resplines;

    while(mailRequestQueue.ReadLine(line))
    {
        if(line == "<END>")
        {
            break;
        }

        resplines.push_back(line);
    }

    if(resplines.size() < 2)
    {
        return false;
    }

    //Omit the header
	resplines.erase(resplines.begin(), resplines.begin() + 1);

    for(auto pr : resplines)
    {
		std::vector<std::string> fields;

        Profile prf;
        DeSerializeProfile(prf, pr);

        ctlist.push_back(prf);
    }

    if(ctlist.size() < 1)
    {
        return false;
    }

    return true;
}

bool MailClient::GetProfileInformation(std::string &str, Profile &prf)
{
    std::string comm = "rushpriority_ui|PROFILE INFO|";
    comm += str;
    comm += "\n";

    mailRequestQueue.WriteLine(comm);

    std::string line;

	std::vector<std::string> resplines;

    while(mailRequestQueue.ReadLine(line))
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

bool MailClient::AddProfile(Profile &obj)
{
    std::string comm = "rushpriority_ui|PROFILE ADD|";
	std::string temp;
    SerializeProfile(obj, temp);
    comm += temp;
    comm += "\n";

    mailRequestQueue.WriteLine(comm);

    std::string line;
	bool success = false;

	while (mailRequestQueue.ReadLine(line))
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

bool MailClient::UpdateProfile(Profile &obj)
{
    std::string comm = "rushpriority_ui|PROFILE UPDATE|";
    std::string temp;
    SerializeProfile(obj, temp);
    comm += temp;
    comm += "\n";

    mailRequestQueue.WriteLine(comm);

    std::string line;
	bool success = false;

	while (mailRequestQueue.ReadLine(line))
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

bool MailClient::RemoveProfile(std::string &str)
{
    std::string comm = "rushpriority_ui|PROFILE REMOVE|";
    std::string temp;
    comm += str;
    comm += "\n";

    mailRequestQueue.WriteLine(comm);

    std::string line;
	bool success = false;

	while (mailRequestQueue.ReadLine(line))
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

bool MailClient::SendEmail(Mail &eml, MailStorageInformation &stg)
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

	mailRequestQueue.WriteLine(comm);

	std::string line;

	bool sent = false;

	while (mailRequestQueue.ReadLine(line))
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

bool MailClient::GetAccountDirectories(std::string &profilename, std::vector<std::string> &resplines)
{
    std::string comm = "rushpriority_ui|LIST DIRECTORIES|";
    comm += profilename;
    comm += "\n";

    mailRequestQueue.WriteLine(comm);

    std::string line;

    resplines.clear();

    while(mailRequestQueue.ReadLine(line))
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

bool MailClient::GetEmails(std::string &profilename, std::string &dirname, std::vector<MailHeader> &mails, std::vector<MailStorageInformation>& stgl)
{
    std::string comm = "rushpriority_ui|LIST EMAILS|";
    comm += profilename;
    comm += "|";
    comm += dirname;
    comm += "\n";

    mailRequestQueue.WriteLine(comm);

    std::string line;

	bool header_read = false;

    while(mailRequestQueue.ReadLine(line))
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

bool MailClient::GetEmailsByTerm(std::string& profilename, std::string& dirname, std::string& term, std::vector<MailHeader>& mails, std::vector<MailStorageInformation>& stgl)
{
	std::string comm = "rushpriority_ui|SEARCH EMAILS|";
	comm += profilename;
	comm += "|";
	comm += dirname;
	comm += "|";
	comm += term;
	comm += "\n";

	mailRequestQueue.WriteLine(comm);

	std::string line;

	bool header_read = false;

	while (mailRequestQueue.ReadLine(line))
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

bool MailClient::GetEmailHeader(std::string &profilename, std::string &dirname, std::string &uid)
{
	std::string comm = "rushpriority_ui|EMAIL HEADER|";
	comm += profilename;
	comm += "|";
	comm += dirname;
	comm += "|";
	comm += uid;
	comm += "\n";

	mailRequestQueue.WriteLine(comm);

	std::string line;
	bool success = false;

	while (mailRequestQueue.ReadLine(line))
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

bool MailClient::GetEmailBody(std::string &profilename, std::string &dirname, std::string & uid)
{
	std::string comm = "rushpriority_ui|EMAIL BODY|";
	comm += profilename;
	comm += "|";
	comm += dirname;
	comm += "|";
	comm += uid;
	comm += "\n";

	mailRequestQueue.WriteLine(comm);

	std::string line;
	bool success = false;

	while (mailRequestQueue.ReadLine(line))
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

bool MailClient::RemoveEmail(std::string &profilename, std::string &dirname, std::string & uid, std::string& messageid)
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

	mailRequestQueue.WriteLine(comm);

	std::string line;
	bool success = false;

	while (mailRequestQueue.ReadLine(line))
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

bool MailClient::FlagEmail(std::string &profilename, std::string &dirname, std::string &uid, std::string &flag)
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

	mailRequestQueue.WriteLine(comm);

	std::string line;
	bool success = false;

	while (mailRequestQueue.ReadLine(line))
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

bool MailClient::MarkEmailSeen(std::string &profilename, std::string &dirname, std::string &uid)
{
	std::string comm = "rushpriority_ui|EMAIL MARK|";
	comm += profilename;
	comm += "|";
	comm += dirname;
	comm += "|";
	comm += uid;
	comm += "\n";

	mailRequestQueue.WriteLine(comm);

	std::string line;
	bool success = false;

	while (mailRequestQueue.ReadLine(line))
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

bool MailClient::PurgeDeleted(std::string &profilename, std::string &dirname)
{
	std::string comm = "rushpriority_ui|DIRECTORY PURGE|";
	comm += profilename;
	comm += "|";
	comm += dirname;
	comm +=	"\n";

	mailRequestQueue.WriteLine(comm);

	std::string line;
	bool success = false;

	while (mailRequestQueue.ReadLine(line))
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

void DeSerializeProfile(Profile &prf, std::string& str)
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

void SerializeProfile(Profile& prf, std::string& str)
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