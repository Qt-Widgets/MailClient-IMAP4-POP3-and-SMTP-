#include "Mail.h"
#include <string.h>
#include <memory.h>
#include "../utils/StringEx.h"
#include "../utils/Base64.h"
#include "../utils/Directory.h"
#include "../utils/Timestamp.h"

void translateMonthToNumber(std::string& str);

MailBody::MailBody()
{
	_MessageFormat = PlainText;
}

MailBody::~MailBody()
{

}

MailBody::MailBody(const MailBody &other)
{
    _Message = other._Message;
    _MessageId = other._MessageId;

    for(auto a : other._AttachementList)
    {
        _AttachementList.push_back(a);
    }
}

void MailBody::operator=(const MailBody& other)
{
    _Message = other._Message;
    _MessageId = other._MessageId;

    for(auto a : other._AttachementList)
    {
        _AttachementList.push_back(a);
    }
}

void MailBody::setMessage(string message)
{
    _Message = message;
}

void MailBody::attachFile(string filename)
{
    _AttachementList.push_back(filename);
}

void MailBody::setMessageId(string msgid)
{
    _MessageId = msgid;
}

string MailBody::message()
{
    return _Message;
}

std::vector<string> MailBody::attachments()
{
    return _AttachementList;
}

string MailBody::messageId()
{
    return _MessageId;
}

MimeType MailBody::messageFormat()
{
	return _MessageFormat;
}

void MailBody::setMessageFormat(MimeType mtype)
{
	_MessageFormat = mtype;
}

void MailBody::serialize(string &str)
{
    str.clear();

    if(_AttachementList.size() < 1)
    {
        str += "\n" + _Message;
    }
    else
    {
        char linebuffer[1024] = {0};
        std::string attachmentBoundary = "";
        std::string ts;
        strtimestamp(ts);

        attachmentBoundary = "--";
        attachmentBoundary += ts;
        attachmentBoundary += ts;
        attachmentBoundary += "--";

        memset(linebuffer, 0, 1024);
        sprintf(linebuffer, "MIME-Version: 1.0\n");
        str += linebuffer;
        memset(linebuffer, 0, 1024);
        sprintf(linebuffer, "Content-Type:multipart/mixed;boundary=\"%s\"\n", attachmentBoundary.c_str());
        str += linebuffer;

        memset(linebuffer, 0, 1024);
        sprintf(linebuffer, "--%s\n", attachmentBoundary.c_str());
        str += linebuffer;

        str += "Content-type: text/plain; charset=iso-8859-1\n";
        str += "Content-Transfer-Encoding: quoted-printable\n";

        memset(linebuffer, 0, 1024);
        sprintf(linebuffer, "\n%s\n", _Message.c_str());
        str += linebuffer;

        for(int idx = 0; idx < _AttachementList.size(); idx++)
        {
            std::string fname = _AttachementList[idx];

            if(!dirfileexists(fname))
            {
                continue;
            }


            FILE* attachmentfile = fopen(fname.c_str(), "rb");

            if (attachmentfile == NULL)
            {
                continue;
            }

            fseek(attachmentfile, 0, SEEK_END);
            int contentlength = ftell(attachmentfile);
            rewind(attachmentfile);

            unsigned char* attData = (unsigned char*)calloc(1, contentlength);
            fread(attData, contentlength, 1, attachmentfile);
            fclose(attachmentfile);

            std::string b64content;
            class Base64 b64;
            unsigned long olen;
            b64.EncodeBase64(attData, contentlength,olen, b64content);
            free(attData);

            memset(linebuffer, 0, 1024);
            sprintf(linebuffer, "--%s\n", attachmentBoundary.c_str());
            str += linebuffer;

            memset(linebuffer, 0, 1024);
            sprintf(linebuffer, "Content-Type:application/octet-stream;name=\"%s\"\n", fname.c_str());
            str += linebuffer;

            memset(linebuffer, 0, 1024);
            sprintf(linebuffer, "Content-Transfer-Encoding:base64\n");
            str += linebuffer;

            memset(linebuffer, 0, 1024);
            sprintf(linebuffer, "Content-Disposition:attachment;filename=\"%s\"\n\n", fname.c_str());
            str += linebuffer;

            str += b64content;
            str += "\n\n";

        }

        memset(linebuffer, 0, 1024);
        sprintf(linebuffer, "--%s--\n", attachmentBoundary.c_str());
        str += linebuffer;
    }
	
	str += "\r\n.\r\n";
}

void MailBody::deSerialize()
{
    parseAttachments();
    parseTexts();
}

bool MailBody::hasMimeData()
{
    strsplit(_Message, _MimeData, "\r\n", true);

    while(true)
    {
        if(_MimeData[_MimeData.size() - 1].length() < 1)
        {
            _MimeData.pop_back();
        }
        else
        {
            break;
        }
    }

    std::string firstline = _MimeData.at(0);
    std::string lastline = _MimeData[_MimeData.size() - 1];
    bool hasMimeBoundary = false;
    bool hasContentTypeTag = false;

    if(firstline+"--" == lastline)
    {
        hasMimeBoundary = true;

        std::string tag = _MimeData[1];
        strlower(tag);

        if(strsubstringpos(tag.c_str(), "content-type") > -1)
        {
           hasContentTypeTag = true;
        }
    }

    if(hasMimeBoundary && hasContentTypeTag)
    {
        return true;
    }

    return false;
}

void MailBody::parseAttachments()
{
    std::vector<std::string> displaydata;

    int count = _MimeData.size();

    for(int idx = 0; idx < count; idx++)
    {
        std::string line = _MimeData[idx];

        std::string tag = _MimeData[idx];
        strlower(tag);

        if(strsubstringpos(tag.c_str(), "content-type") > -1 && strsubstringpos(tag.c_str(), "name=") > -1)
        {
            idx--;
            extractAttachment(idx);
            continue;
        }

        displaydata.push_back(line);
    }

    _MimeData.clear();

    for(auto p : displaydata)
    {
        _MimeData.push_back(p);
    }
}

void MailBody::extractAttachment(int &ctr)
{
    int startline = ctr;
    std::string buffer = "";
    std::string start = _MimeData.at(ctr);
    std::string inlinecontentid;

    bool startaccumulating= false;
    MimeNode value;

    value.NodeType = MimeType::Attachment;

    ctr++;

    while(true)
    {
        buffer = _MimeData[ctr];
        stralltrim(buffer);

        if(start == buffer || start+"--" == buffer)
        {
            break;
        }

        if(buffer.length() < 1)
        {
            if(startaccumulating == false)
            {
                startaccumulating = true;
            }
            else
            {
                startaccumulating = false;
            }
            ctr++;
            continue;
        }

        if(startaccumulating)
        {
            if(buffer.length() > 0)
            {
                value.Data += buffer;
            }

            if(strsubstringpos(buffer.c_str(), "==") > -1)
            {
                startaccumulating = false;
            }
        }
        else
        {
            std::string tag = buffer;
            strlower(tag);

            if(strsubstringpos(tag.c_str(), "content-disposition") > -1 && strsubstringpos(tag.c_str(), "inline") > -1)
            {
                value.NodeType = MimeType::InlineImage;
            }

            if(strsubstringpos(tag.c_str(), "content-id") > -1)
            {
                int cpos = strcharacterpos(buffer.c_str(), ':');
                buffer = buffer.erase(0, (size_t)cpos+1);
                strremove(buffer, '<');
                strremove(buffer, '>');
                inlinecontentid = buffer;
                stralltrim(inlinecontentid);
            }
        }

        ctr++;
    }

    std::string name = _MimeData[(size_t)startline+1];
    size_t pos = strsubstringpos(name.c_str(), "name=");
	name = name.erase(0, pos + 6);

    if(value.NodeType == MimeType::Attachment)
    {
        value.NodeName = name;
        strremove(value.NodeName, '"');
    }
    else
    {
        value.NodeName = inlinecontentid;
    }

    _ParsedMimeData.push_back(value);
    _AttachementList.push_back(value.NodeName);
}

void MailBody::parseTexts()
{
    std::vector<std::string> displaydata;

    int count = _MimeData.size();

    for(int idx = 0; idx < count; idx++)
    {
        std::string line = _MimeData[idx];

        std::string tag = _MimeData[idx];
        strlower(tag);
        stralltrim(tag);

        if(strsubstringpos(tag.c_str(), "content-type:") > -1
            && ( strsubstringpos(tag.c_str(), "text/plain") > -1 
				|| strsubstringpos(tag.c_str(), "text/html") > -1) )
        {
            idx--;
            extractText(idx);
            continue;
        }

        displaydata.push_back(line);
    }

    _MimeData.clear();
}

void MailBody::extractText(int &ctr)
{
    std::string buffer = "";
    std::string start = _MimeData.at(ctr);

    bool startaccumulating= false;
    MimeNode value;

    ctr++;

    while(true)
    {
        buffer = _MimeData[ctr];
        stralltrim(buffer);

        if(start == buffer || start+"--" == buffer)
        {
            break;
        }

        if(!startaccumulating)
        {
            if(buffer.length() < 1)
            {
                startaccumulating = true;
                ctr++;
                continue;
            }
        }

        if(startaccumulating)
        {
            if(buffer.length() < 1)
            {
                value.Data += "\r\n";
            }
            else
            {
                if(buffer[buffer.length() -1] == '=' && value.TextEncoding == MailTextEncoding::QuotedPrintable)
                {
                    buffer = buffer.erase(buffer.length() -1, 1);
                }

                value.Data += buffer;
            }
        }
        else
        {
            std::string tag = buffer;
            strlower(tag);
            stralltrim(tag);

            if(strsubstringpos(tag.c_str(), "content-type") > -1 && strsubstringpos(tag.c_str(), "text/plain") > -1)
            {
                value.NodeType = MimeType::PlainText;
            }

            if(strsubstringpos(tag.c_str(), "content-type") > -1 && strsubstringpos(tag.c_str(), "text/html") > -1)
            {
                value.NodeType = MimeType::Html;
            }

            if(strsubstringpos(tag.c_str(), "content-transfer-encoding") > -1 && strsubstringpos(tag.c_str(), "base64") > -1)
            {
                value.TextEncoding = MailTextEncoding::Base64;
            }

            if(strsubstringpos(tag.c_str(), "content-transfer-encoding") > -1 && strsubstringpos(tag.c_str(), "quoted-printable") > -1)
            {
                value.TextEncoding = MailTextEncoding::QuotedPrintable;
            }
        }

        ctr++;
    }

    if(value.TextEncoding == MailTextEncoding::Base64)
    {
        value.Data = decodeBase64(value.Data);
    }

    _ParsedMimeData.push_back(value);
}

std::vector<MimeNode> *MailBody::mimeNodes()
{
    return &_ParsedMimeData;
}

string MailBody::encodeBase64(std::string str)
{
    std::string ret;
    class Base64 b64;
    unsigned long outlen;

    b64.EncodeBase64((const unsigned char*)str.c_str(), str.length(), outlen, ret);

    return ret;
}

string MailBody::encodeBase64(const char* buffer, unsigned long len)
{
    std::string ret;
    class Base64 b64;
    unsigned long outlen;

    b64.EncodeBase64((const unsigned char*)buffer, len, outlen, ret);

    return ret;
}

std::string MailBody::decodeBase64(string str)
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

MailHeader::MailHeader(const MailHeader &other)
{
    _Subject = other._Subject;
    _From = other._From;
    _MessageId = other._MessageId;
    _TimeStamp = other._TimeStamp;
    _ToList = other._ToList;
    _CcList = other._CcList;
    _BccList = other._BccList;
    _Direction = other._Direction;
    _Status = other._Status;

}

void MailHeader::operator=(const MailHeader& other)
{
    _Subject = other._Subject;
    _From = other._From;
    _MessageId = other._MessageId;
    _TimeStamp = other._TimeStamp;
    _ToList = other._ToList;
    _CcList = other._CcList;
    _BccList = other._BccList;
    _Direction = other._Direction;
    _Status = other._Status;
}

void MailHeader::addtoToList(string receipient, bool overwrite)
{
    if(_ToList.length() < 1)
    {
        _ToList = receipient;
        stralltrim(_ToList);
    }
    else
    {
        if(overwrite)
        {
            _ToList = receipient;
        }
        else
        {
			if (_ToList.length() > 0)
			{
				_ToList += ",";
			}
            _ToList += receipient;
            stralltrim(_ToList);
        }
    }
}

void MailHeader::addtoCcList(string receipient, bool overwrite)
{
    if(_CcList.length() < 1)
    {
        _CcList = receipient;
        stralltrim(_CcList);
    }
    else
    {
        if(overwrite)
        {
            _CcList = receipient;
        }
        else
        {
			if (_CcList.length() > 0)
			{
				_CcList += ",";
			}
            _CcList += receipient;
            stralltrim(_CcList);
        }
    }
}

void MailHeader::addtoBccList(string receipient, bool overwrite)
{
    if(_BccList.length() < 1)
    {
        _BccList = receipient;
        stralltrim(_BccList);
    }
    else
    {
        if(overwrite)
        {
            _BccList = receipient;
        }
        else
        {
			if (_BccList.length() > 0)
			{
				_BccList += ",";
			}
            _BccList += receipient;
            stralltrim(_BccList);
        }
    }
}

void MailHeader::setSubject(string subject)
{
    _Subject = subject;
}

void MailHeader::setFrom(string from)
{
    _From = from;
}

void MailHeader::setMessageId(string msgid)
{
    _MessageId = msgid;
}

void MailHeader::setTimeStamp(string dt)
{
	std::vector<std::string> tokens;
	
	strsplit(dt, tokens, ' ', true);

	tokens.pop_back();
	tokens.erase(tokens.begin());

	translateMonthToNumber(tokens[1]);
	strremove(tokens[3], ':');

    _TimeStamp = tokens[2] + tokens[1] + tokens[0] + tokens[3];
}

void MailHeader::setDirection(string dir)
{
    _Direction = dir;
}

void MailHeader::setStatus(string st)
{
    _Status = st;
}

void MailHeader::setAccount(std::string ac)
{
    _Account = ac;
}

std::vector<string> MailHeader::toList()
{
    std::vector<std::string> ret;

    strsplit(_ToList, ret, ",", true);

    return ret;
}

std::vector<string> MailHeader::ccList()
{
    std::vector<std::string> ret;

    strsplit(_CcList, ret, ",", true);

    return ret;
}

std::vector<string> MailHeader::bccList()
{
    std::vector<std::string> ret;

    strsplit(_BccList, ret, ",", true);

    return ret;
}

string MailHeader::to()
{
    return _ToList;
}

std::string MailHeader::cc()
{
    return _CcList;
}

string MailHeader::bcc()
{
    return _BccList;
}

std::string MailHeader::subject()
{
    return _Subject;
}

string MailHeader::from()
{
    return _From;
}

std::string MailHeader::messageId()
{
	std::string temp;
	strtimestamp(temp);
	temp += _From;

	class Base64 b64;
	unsigned long olen;
	b64.EncodeBase64((const unsigned char*)temp.c_str(), temp.length(), olen, _MessageId);

    return _MessageId;
}

string MailHeader::timeStamp()
{
    return _TimeStamp;
}

std::string MailHeader::direction()
{
    return _Direction;
}

string MailHeader::status()
{
    return _Status;
}

std::string MailHeader::account()
{
    return _Account;
}

void MailHeader::serialize(string &str, MailBody *bodyptr)
{
    str.clear();

	std::string temp;
	strtimestamp(temp);
	temp += _From;

	class Base64 b64;
	unsigned long olen;
	b64.EncodeBase64((const unsigned char*)temp.c_str(), temp.length(), olen, _MessageId);

	Timestamp ts;
	_TimeStamp = ts.getDateString("yyyy/MM/dd hh:mm:ss");

	str += "Message-ID: ";
	str += _MessageId;
	str += "\r\n";

	str += "Date: ";
	str += _TimeStamp;
	str += "\r\n";

    str += "To: ";
    str += _ToList;
    str += "\r\n";

    str += "From: ";
    str += _From;
    str += "\r\n";

    str += "Subject: ";
    str += _Subject;
    str += "\r\n";

    if(_CcList.size() > 0)
    {
        str += "cc: ";
        str += _CcList;
        str += "\r\n";
    }

    if(_BccList.size() > 0)
    {
        str += "bcc: ";
        str += _BccList;
        str += "\r\n";
    }

	strsplit(str, _HeaderList, "\r\n", true);

    std::string bodystr;

	if (bodyptr != nullptr)
	{
		bodyptr->serialize(bodystr);
	}

    str += bodystr;
}

std::vector<std::string> MailHeader::headers()
{
	return _HeaderList;
}

void MailHeader::deSerialize()
{

}

void MailHeader::normalizeAddresses(std::string &address_str)
{
	std::vector<std::string> pass1;
	std::vector<std::string> pass2;
	std::string str;
	std::string line;

	int idx = 0;
	int ctr = 0;

	pass1.clear();
	pass2.clear();

	str = address_str;
	strreplace(str, '<', ',');
	strreplace(str, '>', ',');

	strsplit(address_str, pass1, ',', true);

	ctr = pass1.size();

	for (idx = 0; idx < ctr; idx++)
	{
		line = pass1[idx];

		stralltrim(line);
		if (strcharacterpos(line.c_str(), '@') > -1 && line[0] != '"')
		{
			pass2.push_back(line);
		}
	}

	ctr = pass2.size();

	for (idx = 0; idx < ctr; idx++)
	{
		address_str += pass2[idx];

		if (idx < ctr - 1)
		{
			address_str += ", ";
		}
	}
}

void MailHeader::normalizeAddresses()
{
	normalizeAddresses(_From);
	normalizeAddresses(_ToList);
	normalizeAddresses(_CcList);
	normalizeAddresses(_BccList);
}

string MailHeader::encodeBase64(std::string str)
{
    std::string ret;
    class Base64 b64;
    unsigned long outlen;

    b64.EncodeBase64((const unsigned char*)str.c_str(), str.length(), outlen, ret);

    return ret;
}

string MailHeader::encodeBase64(const char* buffer, unsigned long len)
{
    std::string ret;
    class Base64 b64;
    unsigned long outlen;

    b64.EncodeBase64((const unsigned char*)buffer, len, outlen, ret);

    return ret;
}

string MailHeader::decodeBase64(std::string str)
{
    std::string ret;
    class Base64 b64;
    unsigned long outlen;

    ret = (const char*)b64.DecodeBase64(str.c_str(), str.length(), outlen);

    return ret;
}

///////////////////////////////////////////////////////////////////////////

void translateMonthToNumber(std::string &str)
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

