#ifndef _MAILBOX_VIEW
#define _MAILBOX_VIEW

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QtCore>
#include "MailView.h"
#include "../../network/Mail.h"
#include "RichDataList.h"
#include <map>
#include <string>

using namespace std;

class MailInfo
{
public:
	MailHeader Header;
	MailStorageInformation Storage;
};

class MailBoxView : public RichDataList
{
	Q_OBJECT
public:
	MailBoxView(QWidget* parent = nullptr);
	~MailBoxView();
	void Initialize(MailView* ptr);

	void MoveToNext();
	void MoveToPrevious();

	void DeleteCurrent();
	void FlagCurrent();
	void MarkAsSeenCurrent();

	MailInfo GetCurrentToken();

signals:
	void MailSelected(MailInfo emlhdr);

public slots:
	void eventMailSelected(QListWidgetItem* item);
	void eventMailListReceived(std::vector<MailHeader>& mlist, std::vector<MailStorageInformation>& storgaeinfo);
	void eventClearMailView();

private:
	void HeaderReceived(MailHeader emlhdr, MailStorageInformation& storage);

	MailView* mailViewPtr;
	QListWidgetItem* currentViewItem;

	MailInfo currentToken;
	std::map<std::string, MailInfo> mailLookup;
};

#endif

