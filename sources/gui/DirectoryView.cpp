#include "DirectoryView.h"
#include "MainWindow.h"
#include "MailClient.h"
#include "../../data/Profile.h"
#include "../../utils/StringEx.h"

DirectoryView::DirectoryView(QWidget *parent) : QTreeWidget(parent)
{
    setHeaderHidden(false);
    setHeaderLabel("Mailboxes");
    setColumnWidth(0, 250);
    setMinimumWidth(250);
    setMaximumWidth(250);
    setAnimated(true);
    setExpandsOnDoubleClick(true);
	setSelectionMode(SingleSelection);
    auto c = connect(this, &QTreeWidget::itemClicked, this, &DirectoryView::eventDirectorySelected);
}

DirectoryView::~DirectoryView()
{
}

void DirectoryView::Initialize()
{
    for(auto pf : *mailClientPtr->ProfileList())
    {
        QTreeWidgetItem *treeItem = new QTreeWidgetItem(this);

		QString str;
		str += pf.ProfileName.c_str();
		str += " (";
		str += pf.EMailId.c_str();
		str += ")";

        treeItem->setText(0, str);
        treeItem->setData(0, Qt::UserRole, QVariant(pf.ProfileName.c_str()));
        treeItem->setData(0, Qt::UserRole+1, QVariant(pf.EMailId.c_str()));
		treeItem->setExpanded(true);
    }

    for(auto pf : *mailClientPtr->ProfileList())
    {
        std::vector<std::string>* dirlist = mailClientPtr->DirectoryList(pf.ProfileName);
        InitializeDirectoryList(pf.ProfileName, *dirlist);
    }

    QTreeWidgetItem *firstprofileitem = topLevelItem(0);
}

void DirectoryView::InitializeDirectoryList(std::string uname, std::vector< std::string> slist)
{
    QTreeWidgetItem *item = nullptr;

	std::string inboxstring = "";

    int tctr = 0;

    for(tctr = 0; tctr < slist.size(); tctr++)
    {
		std::string str = slist[tctr];

		strlower(str);

        if(strcontains(str.c_str(), "inbox"))
        {
           inboxstring = slist[tctr];
           break;
        }
    }

    if(inboxstring.length() > 1)
    {
		slist.erase(slist.begin()+tctr, slist.begin() + tctr + 1);
        slist.insert(slist.begin(), inboxstring);
    }

    for( int i = 0; i < topLevelItemCount(); ++i )
    {
       item = topLevelItem( i );

	   std::string udata = item->data(0, Qt::UserRole).toString().toStdString();

       if(udata == uname)
       {
           break;
       }
    }

    if(item != nullptr)
    {
        for(int idx = 0; idx < slist.size(); idx++)
        {
            QTreeWidgetItem *treeItem = new QTreeWidgetItem(item);

			std::string normalized_name = slist[idx];

			strremove(normalized_name, '"');

            treeItem->setText(0, normalized_name.c_str());
			std::string trdata = uname + ":" + normalized_name;
            treeItem->setData(0, Qt::UserRole, QVariant(trdata.c_str()));
        }
    }
}

void DirectoryView::eventDirectorySelected(QTreeWidgetItem *item, int column)
{
    (void)column;
    QTreeWidgetItem* parentptr = item->parent();

    //We are at top level
    if(parentptr == nullptr)
    {
        return;
    }

	emit ClearMailView();

	std::string itemdata = item->data(0, Qt::UserRole).toString().toStdString();

    std::vector<std::string> tokens;

	strsplit(itemdata, tokens, ':', true);

    if(tokens.size() > 1)
    {
		std::vector<MailHeader> mails;
		std::vector<MailStorageInformation> sinfo;

		std::string profile_name = tokens[0];
		std::string dir_name = tokens[1];

		if (mailClientPtr->GetEmails(profile_name, dir_name, mails, sinfo))
		{
			emit MailListRecieved(mails, sinfo);
		}
    }
}

void DirectoryView::AddProfile(std::string profile_name)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(this);

    treeItem->setText(0, profile_name.c_str());
    treeItem->setData(0, Qt::UserRole, QVariant(profile_name.c_str()));

	std::vector<std::string>* dirlist = mailClientPtr->DirectoryList(profile_name);
	InitializeDirectoryList(profile_name, *dirlist);
}

void DirectoryView::RemoveProfile(std::string profile_name)
{
    QTreeWidgetItem *item = nullptr;
    int i = 0;

    for( i = 0; i < topLevelItemCount(); ++i )
    {
       item = topLevelItem( i );

	   std::string udata = item->data(0, Qt::UserRole).toString().toStdString();

       if(udata == profile_name)
       {
           break;
       }
    }

    if(item != nullptr)
    {
        item = takeTopLevelItem(i);
        delete item;
    }
}

void DirectoryView::UpdateProfile(std::string profile_name)
{

}

void DirectoryView::Refresh()
{
	QList<QTreeWidgetItem*> items = selectedItems();

	QTreeWidgetItem* item = items.at(0);

	QTreeWidgetItem* parentptr = item->parent();

	//We are at top level
	if (parentptr == nullptr)
	{
		return;
	}

	std::string itemdata = item->data(0, Qt::UserRole).toString().toStdString();

	std::vector<std::string> tokens;

	strsplit(itemdata, tokens, ':', true);

	if (tokens.size() > 1)
	{
		std::vector<MailHeader> mails;
		std::vector<MailStorageInformation> sinfo;

		std::string profile_name = tokens[0];
		std::string dir_name = tokens[1];
		mailClientPtr->GetEmails(profile_name, dir_name, mails, sinfo);
		emit MailListRecieved(mails, sinfo);
	}
}

void DirectoryView::Expunge()
{
	QList<QTreeWidgetItem*> items = selectedItems();

	QTreeWidgetItem* item = items.at(0);

	QTreeWidgetItem* parentptr = item->parent();

	//We are at top level
	if (parentptr == nullptr)
	{
		return;
	}

	std::string itemdata = item->data(0, Qt::UserRole).toString().toStdString();

	std::vector<std::string> tokens;

	strsplit(itemdata, tokens, ':', true);

	if (tokens.size() > 1)
	{
		std::vector<MailHeader> mails;

		std::string profile_name = tokens[0];
		std::string dir_name = tokens[1];
		mailClientPtr->PurgeDeleted(profile_name, dir_name);
	}
}

void DirectoryView::Search(std::string searchterm)
{
	QList<QTreeWidgetItem*> items = selectedItems();

	QTreeWidgetItem* item = items.at(0);

	QTreeWidgetItem* parentptr = item->parent();

	//We are at top level
	if (parentptr == nullptr)
	{
		return;
	}

	std::string itemdata = item->data(0, Qt::UserRole).toString().toStdString();

	std::vector<std::string> tokens;

	strsplit(itemdata, tokens, ':', true);

	if (tokens.size() > 1)
	{
		std::vector<MailHeader> mails;
		std::vector<MailStorageInformation> sinfo;

		std::string profile_name = tokens[0];
		std::string dir_name = tokens[1];

		stralltrim(searchterm);

		if (searchterm.length() > 0)
		{
			mailClientPtr->GetEmailsByTerm(profile_name, dir_name, searchterm, mails, sinfo);
		}
		else
		{
			mailClientPtr->GetEmails(profile_name, dir_name, mails, sinfo);
		}

		emit MailListRecieved(mails, sinfo);
	}
}
