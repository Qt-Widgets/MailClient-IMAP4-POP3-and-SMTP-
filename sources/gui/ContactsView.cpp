#include "ContactsView.h"
#include "../app/MailClient.h"
#include "../utils/StringEx.h"

ContactsView::ContactsView(QWidget *parent) : QDialog(parent)
{
    QStringList lst;

    lst << "Name" << "EMail Id";
    listContactView.setHeaderLabels(lst);

	btnOK.setMaximumWidth(75);
	btnCancel.setMaximumWidth(75);

    auto c1 = connect(&btnOK, &QAbstractButton::clicked, this, &ContactsView::eventOk);
    auto c2 = connect(&btnCancel, &QAbstractButton::clicked, this, &ContactsView::evenCancel);

	btnLayout.addStretch();
	btnLayout.addWidget(&btnCancel);
	btnLayout.addWidget(&btnOK);

    mainLayout.addWidget(&listContactView);
    mainLayout.addLayout(&btnLayout);

    btnOK.setText("OK");
    btnCancel.setText("Cancel");

    mainLayout.setMargin(5);

    setLayout(&mainLayout);

    setMinimumHeight(480);
    setMinimumWidth(640);
    setMaximumHeight(480);
    setMaximumWidth(640);
	setModal(true);
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
}

void ContactsView::eventOk()
{
	close();
}

void ContactsView::evenCancel()
{
	QTreeWidgetItemIterator it(&listContactView);

	while (*it)
	{
		(*it)->setCheckState(0, Qt::CheckState::Unchecked);
		++it;
	}

	close();
}

void ContactsView::GetSelectedContacts(std::vector<std::string>& clist)
{
	QTreeWidgetItemIterator it(&listContactView);

	while (*it) 
	{
		if ((*it)->checkState(0) == Qt::CheckState::Checked)
		{
			clist.push_back(QVariant((*it)->data(1, Qt::UserRole)).toString().toStdString());
		}

		++it;
	}
}

void ContactsView::LoadContacts()
{
	listContactView.clear();

	std::vector<std::string> list;

    mailClientPtr->GetAllContacts(list);

	for(auto str : list)
    {
		std::vector<std::string> fields;
		strsplit(str, fields, ',');

		QString display_name;
		display_name += fields[3].c_str();
		display_name += ", ";
		display_name += fields[1].c_str();

		if (fields[2].length() > 0 && fields[2] != "-")
		{
			display_name += " ";
			display_name += fields[2].c_str();
		}

        QTreeWidgetItem *profileItem = new QTreeWidgetItem(&listContactView);
        profileItem->setText(0, display_name);
        profileItem->setText(1, fields[0].c_str());
        profileItem->setData(1, Qt::UserRole, QVariant(fields[0].c_str()));
		profileItem->setCheckState(0, Qt::CheckState::Unchecked);
    }
}