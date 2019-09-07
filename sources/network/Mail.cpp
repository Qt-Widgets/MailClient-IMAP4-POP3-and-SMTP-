#include "Mail.h"
#include <string.h>
#include <memory.h>
#include "../utils/StringEx.h"
#include "../utils/Base64.h"
#include "../utils/Directory.h"
#include "../utils/DateTime.h"

void translateMonthToNumber(std::string& str);

MailBody::MailBody()
{
	messageId ="";
    mailData.clear();
}

MailBody::~MailBody()
{

}

MailBody::MailBody(const MailBody& other)
{
	messageId = other.messageId;
    mailData.clear();
    mailData = other.mailData;
}

void MailBody::operator=(const MailBody& other)
{
	messageId = other.messageId;
    mailData.clear();
    mailData = other.mailData;
}

void MailBody::SetMessageId(string msgid)
{
	messageId = msgid;
}

std::string MailBody::GetMessageId()
{
	return messageId;
}

void MailBody::AddNode(MimeNode node)
{
    mailData.push_back(node);
}


void MailBody::Serialize(std::string &str)
{
    str.clear();

    std::string mime_boundary = "";
    std::string ts;
    strtimestamp(ts);

    mime_boundary = "--";
    mime_boundary += ts;
    mime_boundary += ts;
    mime_boundary += "--";
    char linebuffer[1024] = { 0 };

    if(mailData.size() < 1)
    {
        return;
    }

    if(mailData.size() == 1)
    {
        if(mailData[0].NodeType == PlainText)
        {
            str = mailData[0].Data;
            return;
        }
    }

    memset(linebuffer, 0, 1024);
    sprintf(linebuffer, "MIME-Version: 1.0\n");
    str += linebuffer;
    memset(linebuffer, 0, 1024);
    sprintf(linebuffer, "Content-Type:multipart/mixed;boundary=\"%s\"\n", mime_boundary.c_str());
    str += linebuffer;

    for(auto node : mailData)
    {
        if (node.NodeType == Html || node.NodeType == PlainText)
        {

            memset(linebuffer, 0, 1024);
            sprintf(linebuffer, "--%s\n", mime_boundary.c_str());
            str += linebuffer;

            if(node.NodeType == Html)
            {
                str += "Content-type: text/html; charset=iso-8859-1\n";
            }
            else
            {
                str += "Content-type: text/plain; charset=iso-8859-1\n";
            }

            str += "Content-Transfer-Encoding: quoted-printable\n";

            memset(linebuffer, 0, 1024);
            sprintf(linebuffer, "\n%s\n", node.Data.c_str());
            str += linebuffer;

            memset(linebuffer, 0, 1024);
            sprintf(linebuffer, "--%s--\n", mime_boundary.c_str());
            str += linebuffer;
        }

        if(node.NodeType == Attachment || node.NodeType == InlineImage)
        {
            std::string fname = node.NodeName;

            if (!dirfileexists(fname))
            {
                continue;
            }

            FILE* attachmentfile = fopen(fname.c_str(), "rb");

            if (attachmentfile == NULL)
            {
                continue;
            }

            fseek(attachmentfile, 0, SEEK_END);
            size_t contentlength = ftell(attachmentfile);
            rewind(attachmentfile);

            unsigned char* attData = (unsigned char*)calloc(1, contentlength);
            fread(attData, contentlength, 1, attachmentfile);
            fclose(attachmentfile);

            std::string b64content;
            class Base64 b64;
            unsigned long olen;
            b64.EncodeBase64(attData, contentlength, olen, b64content);
            free(attData);

            memset(linebuffer, 0, 1024);
            sprintf(linebuffer, "--%s\n", mime_boundary.c_str());
            str += linebuffer;

			std::string complete_base_name;

			dirgetname(fname, complete_base_name);

            memset(linebuffer, 0, 1024);
            sprintf(linebuffer, "Content-Type:application/octet-stream;name=\"%s\"\n", complete_base_name.c_str());
            str += linebuffer;

            memset(linebuffer, 0, 1024);
            sprintf(linebuffer, "Content-Transfer-Encoding:base64\n");
            str += linebuffer;

            memset(linebuffer, 0, 1024);

            if(node.NodeType == Attachment)
            {
                sprintf(linebuffer, "Content-Disposition:attachment;filename=\"%s\"\n\n", fname.c_str());
            }
            else
            {
                sprintf(linebuffer, "Content-Disposition:inline;filename=\"%s\"\n\n", fname.c_str());
            }

            str += linebuffer;

            str += b64content;
            str += "\n\n";
        }
    }

    memset(linebuffer, 0, 1024);
    sprintf(linebuffer, "--%s--\n", mime_boundary.c_str());
    str += linebuffer;
}

void MailBody::DeSerialize()
{
}

bool MailBody::HasMultiMimeData()
{
    if(mailData.size() > 1)
    {
        return true;
    }

    return false;
}

std::vector<MimeNode> MailBody::GetDataNodes()
{
    return mailData;
}

string MailBody::EncodeBase64(std::string str)
{
	std::string ret;
	class Base64 b64;
	unsigned long outlen;

	b64.EncodeBase64((const unsigned char*)str.c_str(), str.length(), outlen, ret);

	return ret;
}

string MailBody::EncodeBase64(const char* buffer, unsigned long len)
{
	std::string ret;
	class Base64 b64;
	unsigned long outlen;

	b64.EncodeBase64((const unsigned char*)buffer, len, outlen, ret);

	return ret;
}

std::string MailBody::DecodeBase64(string str)
{
	std::string ret;
	class Base64 b64;
	unsigned long outlen;

	ret = (const char*)b64.DecodeBase64(str.c_str(), str.length(), outlen);

	return ret;
}

///////////////////////////////////////////////////////////////////////////

MailHeader::MailHeader()
{

}

MailHeader::~MailHeader()
{

}

MailHeader::MailHeader(const MailHeader& other)
{
    headers.clear();
    headers = other.headers;
}

void MailHeader::operator=(const MailHeader& other)
{
    headers.clear();
    headers = other.headers;
}

void MailHeader::AddtoToList(string receipient, bool overwrite)
{
    std::string toList = headers["To"];

	if (toList.length() < 1)
	{
		toList = receipient;
		stralltrim(toList);
	}
	else
	{
		if (overwrite)
		{
			toList = receipient;
		}
		else
		{
			if (toList.length() > 0)
			{
				toList += ",";
			}
			toList += receipient;
			stralltrim(toList);
		}
	}

    headers["To"] = toList;
}

void MailHeader::AddtoCcList(string receipient, bool overwrite)
{
    std::string ccList = headers["CC"];

    if (ccList.length() < 1)
	{
		ccList = receipient;
		stralltrim(ccList);
	}
	else
	{
		if (overwrite)
		{
			ccList = receipient;
		}
		else
		{
			if (ccList.length() > 0)
			{
				ccList += ",";
			}
			ccList += receipient;
			stralltrim(ccList);
		}
	}

    headers["CC"] = ccList;
}

void MailHeader::AddtoBccList(string receipient, bool overwrite)
{
    std::string bccList = headers["BCC"];

    if (bccList.length() < 1)
	{
		bccList = receipient;
		stralltrim(bccList);
	}
	else
	{
		if (overwrite)
		{
			bccList = receipient;
		}
		else
		{
			if (bccList.length() > 0)
			{
				bccList += ",";
			}
			bccList += receipient;
			stralltrim(bccList);
		}
	}

    headers["BCC"] = bccList;
}

void MailHeader::SetSubject(string sub)
{
    headers["Subject"] = sub;
}

void MailHeader::SetFrom(string fr)
{
    headers["From"] = fr;
}

void MailHeader::SetMessageId(string msgid)
{
    headers["Message-Id"] = msgid;
}

void MailHeader::SetTimeStamp(string dt)
{
    headers["Date"] = dt;
}


std::vector<string> MailHeader::GetToList()
{
	std::vector<std::string> ret;

    std::string toList = GetTo();

	strsplit(toList, ret, ",", true);

	return ret;
}

std::vector<string> MailHeader::GetCcList()
{
	std::vector<std::string> ret;

    std::string ccList = GetCc();

	strsplit(ccList, ret, ",", true);

	return ret;
}

std::vector<string> MailHeader::GetBccList()
{
	std::vector<std::string> ret;

    std::string bccList = GetBcc();

	strsplit(bccList, ret, ",", true);

	return ret;
}

string MailHeader::GetTo()
{
    return headers["To"];
}

std::string MailHeader::GetCc()
{
    return headers["CC"];
}

string MailHeader::GetBcc()
{
    return headers["BCC"];
}

std::string MailHeader::GetSubject()
{
    return headers["Subject"];
}

string MailHeader::GetFrom()
{
    return headers["From"];
}


std::string MailHeader::GetMessageId()
{
    return headers["Message-Id"];
}

string MailHeader::GetTimeStamp()
{
    return headers["Date"];
}

void MailHeader::GenerateMessageId()
{
    std::string messageId;

    std::string temp;
    strtimestamp(temp);
    temp += GetFrom();

    class Base64 b64;
    unsigned long olen;
    b64.EncodeBase64((const unsigned char*)temp.c_str(), temp.length(), olen, messageId);

    headers["Message-Id"] = messageId;
}

void MailHeader::GenerateTimeStamp()
{
    DateTime ts;
    std::string timeStamp = ts.getDateString("yyyy/MM/dd hh:mm:ss");
    headers["Date"] = timeStamp;
}

void MailHeader::Serialize(string& str)
{
    str.clear();

    const char keys[7][16] = { "Message-ID", "Date", "To", "From", "Subject", "cc", "bcc" };

    for(int index = 0 ; index < 7 ;index++ )
    {
        std::string val = headers[keys[index]];

        if(val.length() > 0)
        {
            str += keys[index];
            str += ": ";
            str += val;
            str += "\n";
        }
    }
}

void  MailHeader::AddHeader(const std::string& key, const std::string& value)
{
	headers[key] = value;
}


std::vector<std::string> MailHeader::GetHeaders()
{
    vector<std::string> keys;

    for(map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
    {
        keys.push_back(it->first);
    }

    return keys;
}

std::string MailHeader::GetHeaderValue(std::string key)
{
    return headers[key];
}

void MailHeader::DeSerialize()
{

}

string MailHeader::EncodeBase64(std::string str)
{
	std::string ret;
	class Base64 b64;
	unsigned long outlen;

	b64.EncodeBase64((const unsigned char*)str.c_str(), str.length(), outlen, ret);

	return ret;
}

string MailHeader::EncodeBase64(const char* buffer, unsigned long len)
{
	std::string ret;
	class Base64 b64;
	unsigned long outlen;

	b64.EncodeBase64((const unsigned char*)buffer, len, outlen, ret);

	return ret;
}

string MailHeader::DecodeBase64(std::string str)
{
	std::string ret;
	class Base64 b64;
	unsigned long outlen;

	ret = (const char*)b64.DecodeBase64(str.c_str(), str.length(), outlen);

	return ret;
}

///////////////////////////////////////////////////////////////////////////

void Mail::Serialize()
{
	SerializedData.clear();

    const char keys[7][16] = { "Message-ID", "Date", "To", "From", "Subject", "cc", "bcc" };

    for(int index = 0 ; index < 7 ;index++ )
    {
        std::string val = Header.GetHeaderValue(keys[index]);

        if(val.length() > 0)
        {
			SerializedData += keys[index];
			SerializedData += ": ";
			SerializedData += val;
			SerializedData += "\n";
        }
    }

    std::string mime_boundary = "";
    std::string ts;
    strtimestamp(ts);

    mime_boundary = "--";
    mime_boundary += ts;
    mime_boundary += ts;
    mime_boundary += "--";
    char linebuffer[1024] = { 0 };

    std::vector<MimeNode> mailData = Body.GetDataNodes();

    if(mailData.size() < 1)
    {
        return;
    }

    if(mailData.size() == 1)
    {
        if(mailData[0].NodeType == PlainText)
        {
			SerializedData = mailData[0].Data;
            return;
        }
    }

    memset(linebuffer, 0, 1024);
    sprintf(linebuffer, "MIME-Version: 1.0\n");
	SerializedData += linebuffer;
    memset(linebuffer, 0, 1024);
    sprintf(linebuffer, "Content-Type:multipart/mixed;boundary=\"%s\"\n", mime_boundary.c_str());
	SerializedData += linebuffer;

    for(auto node : mailData)
    {
        if (node.NodeType == Html || node.NodeType == PlainText)
        {

            memset(linebuffer, 0, 1024);
            sprintf(linebuffer, "--%s\n", mime_boundary.c_str());
			SerializedData += linebuffer;

            if(node.NodeType == Html)
            {
				SerializedData += "Content-type: text/html; charset=iso-8859-1\n";
            }
            else
            {
				SerializedData += "Content-type: text/plain; charset=iso-8859-1\n";
            }

			SerializedData += "Content-Transfer-Encoding: quoted-printable\n";

            memset(linebuffer, 0, 1024);
            sprintf(linebuffer, "\n%s\n", node.Data.c_str());
			SerializedData += linebuffer;
        }

        if(node.NodeType == Attachment || node.NodeType == InlineImage)
        {
            std::string fname = node.NodeName;

            if (!dirfileexists(fname))
            {
                continue;
            }


            FILE* attachmentfile = fopen(fname.c_str(), "rb");

            if (attachmentfile == NULL)
            {
                continue;
            }

            fseek(attachmentfile, 0, SEEK_END);
            size_t contentlength = ftell(attachmentfile);
            rewind(attachmentfile);

            unsigned char* attData = (unsigned char*)calloc(1, contentlength);
            fread(attData, contentlength, 1, attachmentfile);
            fclose(attachmentfile);

            std::string b64content;
            class Base64 b64;
            unsigned long olen;
            b64.EncodeBase64(attData, contentlength, olen, b64content);
            free(attData);

            memset(linebuffer, 0, 1024);
            sprintf(linebuffer, "--%s\n", mime_boundary.c_str());
			SerializedData += linebuffer;

            memset(linebuffer, 0, 1024);
            sprintf(linebuffer, "Content-Type:application/octet-stream;name=\"%s\"\n", fname.c_str());
			SerializedData += linebuffer;

            memset(linebuffer, 0, 1024);
            sprintf(linebuffer, "Content-Transfer-Encoding:base64\n");
			SerializedData += linebuffer;

            memset(linebuffer, 0, 1024);

            if(node.NodeType == Attachment)
            {
                sprintf(linebuffer, "Content-Disposition:attachment;filename=\"%s\"\n\n", fname.c_str());
            }
            else
            {
                sprintf(linebuffer, "Content-Disposition:inline;filename=\"%s\"\n\n", fname.c_str());
            }

			SerializedData += linebuffer;

			SerializedData += b64content;
			SerializedData += "\n\n";
        }
    }

    memset(linebuffer, 0, 1024);
    sprintf(linebuffer, "--%s--\n", mime_boundary.c_str());
	SerializedData += linebuffer;
}

void Mail::DeSerialize()
{
	std::vector<std::string> temp;

	strsplit(SerializedData, temp, "\n", true);

	int used_lines = 0;

	std::string mime_boundary;

	for(auto str : temp)
	{
		if (strcontains(str.c_str(), "Message-ID:")
			|| strcontains(str.c_str(), "Message-Id:")
			|| strcontains(str.c_str(), "Date:")
			|| strcontains(str.c_str(), "Subject:")
			|| strcontains(str.c_str(), "From:")
			|| strcontains(str.c_str(), "To:")
			|| strcontains(str.c_str(), "CC:")
			|| strcontains(str.c_str(), "BCC:"))
		{
			AddHeader(str);
			used_lines++;
			continue;
		}

		std::string temp = str;
		strlower(temp);

		if (str.length() < 1 || str.find("--") == 0)
		{
			mime_boundary = str;
			break;
		}

		if (strcontains(temp.c_str(), "mime-version:") || (strcontains(temp.c_str(), "content-type:") && strcontains(temp.c_str(), "multipart/alternative")))
		{
			used_lines++;
			continue;
		}				
		
		if (strcontains(temp.c_str(), "boundary="))
		{
			std::string tokens;
			strsplit(str, "boundary=", tokens, mime_boundary, true);
			strremove(mime_boundary, '"');
			used_lines++;
			break;
		}
	}

	temp.erase(temp.begin(), temp.begin()+used_lines);

	SerializedData.clear();

	for (auto str : temp)
	{
		SerializedData += str + "\n";
	}

	temp.clear();

	if (mime_boundary.length() > 0)
	{
		strsplit(SerializedData, temp, mime_boundary, true);

		for (auto str : temp)
		{
			MimeNode node;
			AddNode(str, node, mime_boundary);

			if (node.NodeType != UnKnownType && node.TextEncoding != UnKnownEncoding)
			{
				Body.AddNode(node);
			}
		}
	}
	else
	{
		MimeNode node;
		node.Data = SerializedData;

		if ( (strcontains(SerializedData.c_str(), "<html") || strcontains(SerializedData.c_str(), "<HTML"))
			&& (strcontains(SerializedData.c_str(), "</html>") || strcontains(SerializedData.c_str(), "</HTML>"))
			)
		{
			node.NodeType = Html;
		}
		else
		{
			node.NodeType = PlainText;
		}
		node.TextEncoding = QuotedPrintable;
		Body.AddNode(node);
	}

	SerializedData.clear();
}

void Mail::AddHeader(std::string& str)
{
	stralltrim(str);
	std::string key;
	std::string value;

	strsplit(str, ':', key, value, true);

	Header.AddHeader(key, value);
}

void Mail::AddNode(std::string& substring, MimeNode& node, std::string boundary)
{
	std::vector<std::string> lines;

	if (strcontains(substring.c_str(), "boundary=") && strcontains(substring.c_str(), "multipart/alternative"))
	{
		size_t pos = -1;
		pos = substring.find("boundary=");
		substring = substring.erase(0, pos + strlen("boundary="));
		pos = substring.find("\n");
		std::string sub_boundary = substring.substr(0, pos + 1);
		substring = substring.erase(0, pos + 1);
		strremove(sub_boundary, '"');
		strremove(sub_boundary, '\n');
		AddNode(substring, node, sub_boundary);
	}
	else
	{
		if ((strcontains(substring.c_str(), "Content-Type:") || strcontains(substring.c_str(), "Content-type:") || strcontains(substring.c_str(), "content-type:"))
			&& !strcontains(substring.c_str(), boundary.c_str()))
		{
			ParseNode(substring, node);
		}
		else
		{
			strsplit(substring, lines, boundary);

			if (lines.size() > 0)
			{
				for (auto str : lines)
				{
					MimeNode subnode;
					ParseNode(str, subnode);

					if (subnode.NodeType != UnKnownType && subnode.TextEncoding != UnKnownEncoding)
					{
						node.ChildNodes.push_back(subnode);
					}
				}
			}
			else
			{
				MimeNode subnode;
				ParseNode(substring, subnode);

				if (subnode.NodeType != UnKnownType && subnode.TextEncoding != UnKnownEncoding)
				{
					node.ChildNodes.push_back(subnode);
				}
			}
		}
	}

	if (node.ChildNodes.size() > 0)
	{
		node.NodeType = Multipart;
		node.TextEncoding = XToken;
	}
}

void Mail::ParseNode(std::string& substring, MimeNode& node)
{
	std::vector<std::string> lines;
	strsplit(substring, lines, '\n');

	if (lines.size() > 0)
	{
		if (lines[lines.size() - 1] == "--")
		{
			//Remove boundary marker
			lines.pop_back();
		}
	}

	for (auto str : lines)
	{
		std::string temp = str;

		strlower(temp);

		if (strcontains(temp.c_str(), "content-type") && strcontains(temp.c_str(), "text/plain"))
		{
			node.NodeType = PlainText;
			continue;
		}

		if (strcontains(temp.c_str(), "content-type") && strcontains(temp.c_str(), "text/html"))
		{
			node.NodeType = Html;
			continue;
		}

		if (strcontains(temp.c_str(), "content-transfer-encoding") && strcontains(temp.c_str(), "base64"))
		{
			node.TextEncoding = Base64;
			continue;
		}

		if (strcontains(temp.c_str(), "content-transfer-encoding") && strcontains(temp.c_str(), "quoted-printable"))
		{
			node.TextEncoding = QuotedPrintable;
			continue;
		}

		if (strcontains(temp.c_str(), "content-disposition") && strcontains(temp.c_str(), "inline"))
		{
			node.NodeType = InlineImage;


			if (strcontains(temp.c_str(), "content-disposition") && strcontains(temp.c_str(), "name="))
			{
				std::string tokens;
				strsplit(str, "name=", tokens, node.NodeName, true);
				strremove(node.NodeName, '"');

				size_t pos = strcharacterpos(node.NodeName.c_str(), ';');
				if (pos != std::string::npos)
				{
					node.NodeName = node.NodeName.erase(pos, node.NodeName.length() - pos);
				}

				continue;
			}

			if (strcontains(temp.c_str(), "content-disposition") && strcontains(temp.c_str(), "filename="))
			{
				std::string tokens;
				strsplit(str, "filename=", tokens, node.NodeName, true);
				strremove(node.NodeName, '"');

				size_t pos = strcharacterpos(node.NodeName.c_str(), ';');
				if (pos != std::string::npos)
				{
					node.NodeName = node.NodeName.erase(pos, node.NodeName.length() - pos);
				}

				continue;
			}

			continue;
		}

		if (strcontains(temp.c_str(), "content-disposition") && strcontains(temp.c_str(), "attachment"))
		{
			node.NodeType = Attachment;

			if (strcontains(temp.c_str(), "content-disposition") && strcontains(temp.c_str(), "name="))
			{
				std::string tokens;
				strsplit(str, "name=", tokens, node.NodeName, true);
				strremove(node.NodeName, '"');
				continue;
			}

			if (strcontains(temp.c_str(), "content-disposition") && strcontains(temp.c_str(), "filename="))
			{
				std::string tokens;
				strsplit(str, "filename=", tokens, node.NodeName, true);
				strremove(node.NodeName, '"');
				continue;
			}

			continue;
		}

		if (node.NodeType != UnKnownType && node.TextEncoding != UnKnownEncoding)
		{
			if (node.TextEncoding == Base64 && str.length() < 1)
			{
				//Skip blank lines for Base64 data
				continue;
			}		
			node.Data += str;
			node.Data += "\n";
		}
	}
}

///////////////////////////////////////////////////////////////////////////

void translateMonthToNumber(std::string& str)
{
	strlower(str);

	if (str == "jan")
	{
		str = "01";
		return;
	}

	if (str == "feb")
	{
		str = "02";
		return;
	}

	if (str == "mar")
	{
		str = "03";
		return;
	}

	if (str == "apr")
	{
		str = "04";
		return;
	}

	if (str == "may")
	{
		str = "05";
		return;
	}

	if (str == "jun")
	{
		str = "06";
		return;
	}

	if (str == "jul")
	{
		str = "07";
		return;
	}

	if (str == "aug")
	{
		str = "08";
		return;
	}

	if (str == "sep")
	{
		str = "09";
		return;
	}

	if (str == "oct")
	{
		str = "10";
		return;
	}

	if (str == "nov")
	{
		str = "11";
		return;
	}

	if (str == "dec")
	{
		str = "12";
		return;
	}
}

