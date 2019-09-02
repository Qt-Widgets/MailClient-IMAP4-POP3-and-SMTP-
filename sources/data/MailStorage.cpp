#include "MailStorage.h"
#include "../utils/Directory.h"
#include "../utils/StringEx.h"

MailStorage::MailStorage()
{
}

MailStorage::~MailStorage()
{

}

bool MailStorage::StoreMail(std::string& path, Mail& email)
{
    //Uncompressed data stored in plain text file
    std::string fname = path;
	fname += "/";
    fname += email.Header.GetMessageId();
    strremove(fname, '<');
    strremove(fname, '>');
    fname += ".eml";

    FILE* fp = fopen(fname.c_str(), "w");

    if(fp)
    {
		if (email.SerializedData.length() < 1)
		{
			email.Serialize();
		}

        fwrite(email.SerializedData.c_str(), email.SerializedData.length(), 1, fp);
        fflush(fp);
        fclose(fp);
        return true;
    }

    return false;
}

bool MailStorage::RetrieveMail(std::string& path, Mail& email)
{
    //Uncompressed data stored in plain text file
    std::string fname = path;
	fname += "/";
    fname += email.Header.GetMessageId();
    strremove(fname, '<');
    strremove(fname, '>');
    fname += ".eml";

    FILE* fp = fopen(fname.c_str(), "r");

    if(fp)
    {
        fseek(fp, 0, SEEK_END);
        size_t contentlength = ftell(fp);
        rewind(fp);

        unsigned char* attData = (unsigned char*)malloc(contentlength+(size_t)1);
		memset(attData, 0, contentlength + (size_t)1);
        fread(attData, contentlength, 1, fp);
        fclose(fp);

		email.SerializedData = (const char*)attData;

        free(attData);

        return true;
    }

    return false;
}
