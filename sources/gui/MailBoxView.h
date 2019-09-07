#ifndef _MAILBOX_VIEW
#define _MAILBOX_VIEW

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QtCore>
#include "MailView.h"
#include "../network/Mail.h"
#include "RichDataList.h"
#include <map>
#include <string>

using namespace std;

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

	MailHeader GetCurrentToken();

signals:
	void MailSelected(MailHeader emlhdr);

public slots:
	void eventMailSelected(QListWidgetItem* item);
	void eventMailListReceived(std::vector<MailHeader>& mlist);
	void eventClearMailView();

private:
	void HeaderReceived(MailHeader emlhdr);

	MailView* mailViewPtr;
	QListWidgetItem* currentViewItem;

	MailHeader currentToken;
	std::map<std::string, MailHeader> mailLookup;
};

#endif

