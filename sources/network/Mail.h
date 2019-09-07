#ifndef _MAIL
#define _MAIL


#include <string>
#include <list>
#include <vector>
#include <map>
#include <sstream>

using namespace std;

typedef enum MailTextEncoding
{
    QuotedPrintable,
    Base64,
	XToken,
	UnKnownEncoding
}MailTextEncoding;

typedef enum MimeType
{
    PlainText,
    Html,
    InlineImage,
    Attachment,
	Multipart,
	UnKnownType
}MimeType;

class MimeNode
{
public:
    std::string NodeName;
    std::string Data;
    MimeType NodeType;
    MailTextEncoding TextEncoding;

	MimeNode()
	{
		NodeType = UnKnownType;
		TextEncoding = UnKnownEncoding;
	}

	std::vector<MimeNode> ChildNodes;
};

class MailBody
{
public:
    MailBody();
    MailBody(const MailBody &other);
    ~MailBody();

    void operator=(const MailBody& other);

    void SetMessageId(std::string msgid);
    std::string GetMessageId();
    void AddNode(MimeNode node);
    std::vector<MimeNode> GetDataNodes();

    void Serialize(std::string &str);
    void DeSerialize();

    bool HasMultiMimeData();

private:
    std::string EncodeBase64(std::string str);
    std::string EncodeBase64(const char* buffer, unsigned long len);
    string DecodeBase64(std::string str);

    std::string messageId;
    std::vector<MimeNode> mailData;
};

///////////////////////////////////////////////////////////////////////

class MailHeader
{
public:

    MailHeader();
    MailHeader(const MailHeader &other);
    ~MailHeader();

    void operator=(const MailHeader& other);

    void SetSubject(std::string sub);
    void SetFrom(std::string fr);
    void SetMessageId(std::string msgid);
    void SetTimeStamp(std::string dt);
    void AddtoToList(std::string receipient, bool overwrite = false);
    void AddtoCcList(std::string receipient, bool overwrite = false);
    void AddtoBccList(std::string receipient, bool overwrite = false);
	void AddHeader(const std::string& key, const std::string& value);

    std::string GetSubject();
    std::string GetFrom();
    std::string GetMessageId();
    std::string GetTimeStamp();
    std::vector<std::string> GetToList();
    std::vector<std::string> GetCcList();
    std::vector<std::string> GetBccList();
    std::string GetTo();
    std::string GetCc();
    std::string GetBcc();

    void Serialize(std::string &str);
	void DeSerialize();
    void GenerateMessageId();
    void GenerateTimeStamp();

	std::vector<std::string> GetHeaders();
    std::string GetHeaderValue(std::string key);

private:
    std::string EncodeBase64(std::string str);
    std::string EncodeBase64(const char *buffer, unsigned long len);
    std::string DecodeBase64(std::string str);

    std::map<std::string, std::string> headers;
};

class Mail
{
public:
	MailHeader Header;
	MailBody Body;
	void Serialize();
	void DeSerialize();
	void AddHeader(std::string& str);
	void AddNode(std::string& substring, MimeNode& node, std::string boundary);
	void ParseNode(std::string& substring, MimeNode& node);


	std::string SerializedData;
};

#endif
