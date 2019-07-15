#ifndef _MAIL
#define _MAIL


#include <string>
#include <list>
#include <vector>
#include <map>

using namespace std;

typedef enum MailTextEncoding
{
    QuotedPrintable,
    Base64
}MailTextEncoding;

typedef enum MimeType
{
    PlainText,
    Html,
    InlineImage,
    Attachment
}MimeType;

class MimeNode
{
public:
    std::string NodeName;
    std::string Data;
    MimeType NodeType;
    MailTextEncoding TextEncoding;
};

class MailBody
{
public:

    MailBody();
    MailBody(const MailBody &other);
    ~MailBody();

    void operator=(const MailBody& other);

    void setMessage(std::string message);
    void setMessageId(std::string msgid);

    void attachFile(std::string filename);

    std::string message();
    std::vector<string> attachments();
    std::string messageId();

    void serialize(std::string &str);
	void serialize(std::vector<std::string>& datalines);
    void deSerialize();

    bool hasMimeData();

    std::vector<MimeNode>* mimeNodes();

	MimeType messageFormat();
	void setMessageFormat(MimeType mtype);

private:
    void parseAttachments();
    void parseTexts();
    void extractAttachment(int &ctr);
    void extractText(int &ctr);

    std::string encodeBase64(std::string str);
    std::string encodeBase64(const char* buffer, unsigned long len);
    string decodeBase64(std::string str);

    std::vector<std::string> _AttachementList;
    std::string _Message;
    std::string _MessageId;

    std::vector<std::string> _MimeData;
    std::vector<MimeNode> _ParsedMimeData;

	MimeType _MessageFormat;
};

///////////////////////////////////////////////////////////////////////

class MailHeader
{
public:

    MailHeader();
    MailHeader(const MailHeader &other);
    ~MailHeader();

    void operator=(const MailHeader& other);

    void addtoToList(std::string receipient, bool overwrite = false);
    void addtoCcList(std::string receipient, bool overwrite = false);
    void addtoBccList(std::string receipient, bool overwrite = false);

    void setSubject(std::string subject);
    void setFrom(std::string from);
    void setMessageId(std::string msgid);
    void setTimeStamp(std::string dt);
    void setDirection(std::string dir);
    void setStatus(std::string st);
    void setAccount(std::string ac);

    std::vector<std::string> toList();
    std::vector<std::string> ccList();
    std::vector<std::string> bccList();

    std::string to();
    std::string cc();
    std::string bcc();

    std::string subject();
    std::string from();
    std::string messageId();
    std::string timeStamp();

    std::string direction();
    std::string status();
    std::string account();

    void serialize(std::string &str, MailBody *bodyptr);
	void serialize(std::vector<std::string> &datalines, MailBody* bodyptr);
	void deSerialize();

    void normalizeAddresses();
	void normalizeAddresses(std::string &address_str);

	std::vector<std::string> headers();

private:
    std::string encodeBase64(std::string str);
    std::string encodeBase64(const char *buffer, unsigned long len);
    std::string decodeBase64(std::string str);

    std::string _Subject;
    std::string _From;
    std::string _ToList;
    std::string _CcList;
    std::string _BccList;
    std::string _MessageId;
    std::string _TimeStamp;
    std::string _Direction;
    std::string _Status;
    std::string _Account;

	std::vector<std::string> _HeaderList;
};

class Mail
{
public:
	MailHeader Header;
	MailBody Body;
};

#endif
