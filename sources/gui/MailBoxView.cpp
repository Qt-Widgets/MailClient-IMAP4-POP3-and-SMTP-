#include "MailBoxView.h"
#include "MailView.h"
#include "../network/Mail.h"
#include "../data/MailStorage.h"
#include "../utils/StringEx.h"
#include "../app/MailClient.h"

MailBoxView::MailBoxView(QWidget* parent) : RichDataList(true)
{
	setMaximumWidth(350);
	setMinimumWidth(350);

	mailViewPtr = nullptr;
	currentViewItem = nullptr;

	auto c = connect(this, &QListWidget::itemClicked, this, &MailBoxView::eventMailSelected);
}

MailBoxView::~MailBoxView()
{
}

void MailBoxView::Initialize(MailView* ptr)
{
	mailViewPtr = ptr;
}

void MailBoxView::eventMailSelected(QListWidgetItem* item)
{
	currentViewItem = item;

	std::string currUID = currentViewItem->data(Qt::UserRole + 1).toString().toStdString();

	if (mailLookup.find(currUID) != mailLookup.end())
	{
		if (mailViewPtr != nullptr)
		{
			currentToken = mailLookup[currUID];
			emit MailSelected(currentToken);
			mailClientPtr->MarkEmailSeen(currentToken.Storage.GetAccount(), currentToken.Storage.GetDirectory(), currentToken.Storage.GetUid());
			item->setData(Qt::UserRole + 2, QVariant(false));
			update();
		}
	}
}

void MailBoxView::eventClearMailView()
{
	clear();
	mailLookup.clear();
	currentViewItem = nullptr;
}

void MailBoxView::eventMailListReceived(std::vector<MailHeader>& mlist, std::vector<MailStorageInformation>& storgaeinfo)
{
	clear();
	mailLookup.clear();
	currentViewItem = nullptr;

	int count = mlist.size();

	for (int index = 0; index < count; index++)
	{
		HeaderReceived(mlist[index], storgaeinfo[index]);
	}
}

void MailBoxView::HeaderReceived(MailHeader emlhdr, MailStorageInformation& storage)
{
	std::string temp_timestamp = emlhdr.GetTimeStamp();

	size_t tz_str_pos = std::string::npos;

	tz_str_pos = temp_timestamp.find('+');

	if (tz_str_pos == std::string::npos)
	{
		tz_str_pos = temp_timestamp.find('-');
	}

	if (tz_str_pos != std::string::npos)
	{
		temp_timestamp = temp_timestamp.erase(tz_str_pos, temp_timestamp.length() - tz_str_pos);
	}

	if (mailLookup.find(emlhdr.GetMessageId()) == mailLookup.end())
	{
		QString subject = emlhdr.GetSubject().c_str();
		if (subject.toLower().contains("utf-8"))
		{
			subject = QString::fromUtf8(emlhdr.GetSubject().c_str());
		}		
		
		QString description;
		description += emlhdr.GetFrom().c_str();
		description += ", ";
		description += temp_timestamp.c_str();

		QListWidgetItem* mailItem = new QListWidgetItem(this);
		mailItem->setData(Qt::DisplayRole, QVariant(subject));
		mailItem->setData(Qt::UserRole, QVariant(description));
		mailItem->setData(Qt::UserRole + 1, QVariant(storage.GetUid().c_str()));

		MailInfo inf;
		inf.Header = emlhdr;
		inf.Storage = storage;

		mailLookup[storage.GetUid()] = inf;
	}
}

void MailBoxView::DeleteCurrent()
{
	if (currentViewItem == nullptr)
	{
		return;
	}

	std::string currUID = currentViewItem->data(Qt::UserRole + 1).toString().toStdString();

	if (mailLookup.find(currUID) != mailLookup.end())
	{
		if (mailViewPtr != nullptr)
		{
			currentToken = mailLookup[currUID];
		}
	}

	if (mailClientPtr->RemoveEmail(currentToken.Storage.GetAccount(), currentToken.Storage.GetDirectory(), currentToken.Storage.GetUid(), currentToken.Header.GetMessageId()))
	{
		removeItemWidget(currentViewItem);
		delete currentViewItem;
		currentViewItem = nullptr;
	}
}

void MailBoxView::FlagCurrent()
{
	if (currentViewItem == nullptr)
	{
		return;
	}

	std::string currUID = currentViewItem->data(Qt::UserRole + 1).toString().toStdString();

	if (mailLookup.find(currUID) != mailLookup.end())
	{
		if (mailViewPtr != nullptr)
		{
			currentToken = mailLookup[currUID];
		}
	}

	std::string flag = "Flagged";

	if (mailClientPtr->FlagEmail(currentToken.Storage.GetAccount(), currentToken.Storage.GetDirectory(), currentToken.Storage.GetUid(), flag))
	{

	}
}

void MailBoxView::MarkAsSeenCurrent()
{
	if (currentViewItem == nullptr)
	{
		return;
	}

	std::string currUID = currentViewItem->data(Qt::UserRole + 1).toString().toStdString();

	if (mailLookup.find(currUID) != mailLookup.end())
	{
		if (mailViewPtr != nullptr)
		{
			currentToken = mailLookup[currUID];
		}
	}

	mailClientPtr->MarkEmailSeen(currentToken.Storage.GetAccount(), currentToken.Storage.GetDirectory(), currentToken.Storage.GetUid());
}

void MailBoxView::MoveToNext()
{
	if (currentViewItem == nullptr)
	{
		return;
	}
}

void MailBoxView::MoveToPrevious()
{
	if (currentViewItem == nullptr)
	{
		return;
	}
}

MailInfo MailBoxView::GetCurrentToken()
{
	return currentToken;
}