#include "MainWindow.h"
#include "../../data/Contact.h"
#include "../..//uicommon/HorizontalLine.h"
#include "../..//uicommon/VerticalLine.h"
#include "../..//uicommon/ThemeHandler.h"
#include "../../utils/StringEx.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), contactList(true)
{
	initThemes();

	setMaximumHeight(768);
	setMaximumWidth(1024);

	setMinimumHeight(768);
	setMinimumWidth(1024);

    opsPtr = nullptr;
    currentItem = nullptr;
    isEdited = false;

    btnSearch.setText("Search");
	btnClose.setText("Close");
	btnCreate.setText("Create");
	btnRemove.setText("Remove");
	btnImport.setText("Import");

	themeSelector.setText("Dark Mode");

	auto c1 = connect(&btnCreate, &QAbstractButton::clicked, this, &MainWindow::eventAddEntry);
	auto c2 = connect(&btnRemove, &QAbstractButton::clicked, this, &MainWindow::eventRemoveEntry);
	auto c3 = connect(&contactList, &QListWidget::itemClicked, this, &MainWindow::eventContactSelected);
	auto c4 = connect(&btnSearch, &QAbstractButton::clicked, this, &MainWindow::eventSearch);
	auto c5 = connect(&btnImport, &QAbstractButton::clicked, this, &MainWindow::eventImportEntries);
	auto c6 = connect(&btnClose, &QAbstractButton::clicked, this, &MainWindow::close);
	auto c7 = connect(&contactDetail, &ContactDetails::SaveCurrentRecord, this, &MainWindow::eventSaveEntry);
	auto c8 = connect(&themeSelector, &QCheckBox::clicked, this, &MainWindow::eventSwitchTheme);

	searchLayout.addWidget(&btnImport);	
	searchLayout.addWidget(new VerticalLine());
	searchLayout.addWidget(&txtSearch);
    searchLayout.addWidget(&btnSearch);
	searchLayout.addWidget(new VerticalLine());
	searchLayout.addWidget(&btnRemove);
	searchLayout.addWidget(&btnCreate);


	layout.addLayout(&searchLayout, 1, 0, 1, 2);
	layout.addWidget(&contactList, 3, 0, 1, 1);
	layout.addWidget(&contactDetail, 3, 1, 1, 1);
	layout.addWidget(&themeSelector, 5, 0, 1, 1, Qt::AlignLeft);
	layout.addWidget(&btnClose, 5, 1, 1, 1, Qt::AlignRight);

	clientWidget.setLayout(&layout);
	setCentralWidget(&clientWidget);
    setWindowTitle(tr("Address Book"));
    QIcon icon(":/images/addressbook.png");
    setWindowIcon(icon);
}

void MainWindow::eventLoadContacts(std::vector<std::string> &ctlist)
{
	for (auto str : ctlist)
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

		QListWidgetItem* profileItem = new QListWidgetItem(&contactList);
		profileItem->setData(Qt::DisplayRole, QVariant(display_name));
		profileItem->setData(Qt::UserRole, QVariant(fields[0].c_str()));
		profileItem->setData(Qt::UserRole + 1, QVariant(fields[0].c_str()));
	}
}

void  MainWindow::eventContactSelected(QListWidgetItem* item)
{
	currentItem = item;
	currentContactId = item->data(Qt::UserRole + 1).toString();

	if (opsPtr)
	{
		isEdited = true;
		Contact rec;
		opsPtr->GetContact(currentContactId.toStdString(), rec);
		contactDetail.SetRecord(rec);
	}
}


void MainWindow::eventAddEntry()
{
    isEdited = false;
	contactDetail.ResetFields();
}

void MainWindow::eventRemoveEntry()
{
    if(opsPtr)
    {
        if(opsPtr->RemoveContact(currentContactId.toStdString()))
        {
			contactList.removeItemWidget(currentItem);
            delete currentItem;
            currentItem = nullptr;
        }
    }
}

void MainWindow::eventSaveEntry()
{
	Contact rec;
	rec= contactDetail.GetRecord();

	QString display_name;
	display_name += rec.LastName.c_str();
	display_name += ", ";
	display_name += rec.FirstName.c_str();

    if(opsPtr)
    {
        if(isEdited)
        {
            if(opsPtr->UpdateContact(rec))
            {
				currentItem->setData(Qt::DisplayRole, QVariant(display_name));
				currentItem->setData(Qt::UserRole, QVariant(rec.EmailId.c_str()));
				currentItem->setData(Qt::UserRole + 1, QVariant(rec.EmailId.c_str()));
				contactList.repaint();
            }
        }
        else
        {
            if(opsPtr->AddContact(rec))
            {
				QListWidgetItem *profileItem = new QListWidgetItem(&contactList);
				profileItem->setData(Qt::DisplayRole, QVariant(display_name));
				profileItem->setData(Qt::UserRole, QVariant(rec.EmailId.c_str()));
				profileItem->setData(Qt::UserRole + 1, QVariant(rec.EmailId.c_str()));
            }
        }
    }
}

void MainWindow::eventSearch()
{
	contactList.clear();

	std::vector<std::string> ctlist;

	if (txtSearch.text().length() > 0)
	{
		opsPtr->SearchContacts(ctlist, txtSearch.text().toStdString());
	}
	else
	{
		opsPtr->GetAllContacts(ctlist);
	}
	eventLoadContacts(ctlist);
}

void MainWindow::SetOpsHandler(ContactOperations* ptr)
{
    opsPtr = ptr;
}

void MainWindow::showEvent(QShowEvent *ev)
{
	QMainWindow::showEvent(ev);

    if(opsPtr)
    {
		std::vector<std::string> ctlist;
        opsPtr->GetAllContacts(ctlist);
        eventLoadContacts(ctlist);
    }
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
	QMainWindow::closeEvent(ev);
}

void MainWindow::eventImportEntries()
{
	QList<Contact> clist;

	QString filename = QFileDialog::getOpenFileName(
		this,
		"Open Document",
		QDir::currentPath(),
		"CSV files (*.csv)");

	if (!filename.isNull())
	{
		int first_name_pos = -1;
		int last_name_pos = -1;
		int middle_name_pos = -1;
		int email_id = -1;

		QFile inputFile(filename);

		int lineno = 0;

		if (inputFile.open(QIODevice::ReadOnly))
		{
			QTextStream in(&inputFile);

			while (!in.atEnd())
			{
				QString line = in.readLine();

				//Header positions - START
				if (lineno == 0)
				{
					QStringList fields = line.split(',', QString::KeepEmptyParts);

					int fieldno = 0;

					for (auto str : fields)
					{
						if (str.toLower().contains("name") && str.toLower().contains("first"))
						{
							first_name_pos = fieldno;
						}

						if (str.toLower().contains("name") && str.toLower().contains("last"))
						{
							last_name_pos = fieldno;
						}

						if (str.toLower().contains("name") && str.toLower().contains("middle"))
						{
							middle_name_pos = fieldno;
						}

						if (str.toLower().contains("email") || str.toLower().contains("mail"))
						{
							email_id = fieldno;
							break;
						}

						fieldno++;
					}

					lineno++;
					continue;
				}
				//Header positions - END

				QStringList fieldvalues = line.split(',', QString::KeepEmptyParts);

				Contact con;

				con.FirstName = fieldvalues.at(first_name_pos).toStdString();
				con.LastName = fieldvalues.at(last_name_pos).toStdString();
				con.MiddleName = fieldvalues.at(middle_name_pos).toStdString();
				con.EmailId = fieldvalues.at(email_id).toStdString();

				if (con.FirstName.length() > 0 && con.LastName.length() > 0 && con.EmailId.length() > 0)
				{
					if (con.MiddleName.length() < 1)
					{
						con.MiddleName = "-";
					}

					con.AlternateEmailIds = con.EmailId;
					QThread::currentThread()->msleep(100);
					clist.append(con);
				}

				lineno++;
			}

			inputFile.close();
		}
	
		for (auto rec : clist)
		{
			if (opsPtr->AddContact(rec))
			{
				QString display_name;
				display_name += rec.LastName.c_str();
				display_name += ", ";
				display_name += rec.FirstName.c_str();

				QListWidgetItem* profileItem = new QListWidgetItem(&contactList);
				profileItem->setData(Qt::DisplayRole, QVariant(display_name));
				profileItem->setData(Qt::UserRole, QVariant(rec.EmailId.c_str()));
				profileItem->setData(Qt::UserRole + 1, QVariant(rec.EmailId.c_str()));
			}
		}
	}
}

void MainWindow::eventSwitchTheme()
{
	if (themeSelector.checkState() == Qt::Checked)
	{
		emit SwitchToDarkTheme();
	}
	else
	{
		emit SwitchToLightTheme();
	}
}
