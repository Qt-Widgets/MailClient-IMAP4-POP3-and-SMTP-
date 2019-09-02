#include "ProfileView.h"
#include "MailClient.h"

ProfileView::ProfileView(QWidget* parent) : QWidget(parent)
{
	isEditMode = false;

    QIcon icon(":/images/profiles.png");
    setWindowIcon(icon);

    QLayout *ptr = layout();   
    if(ptr != nullptr)
    {
        delete ptr;
    }

    currentRecord.EMailId = "";

    widgetLayout.addWidget(&lstViewProfiles, 0, 0, 1, 4, Qt::AlignTop | Qt::AlignLeft);

	lblMailInGrp.setText("Mail Incoming");
	lblMailInServ.setText("Server");
	lblMailInPort.setText("Port");
	lblMailInSecr.setText("Encryption");
	lblName.setText("Profile Name");

	lblMailOutGrp.setText("Mail Outcoimg");
	lblMailOutServ.setText("Server");
	lblMailOutPort.setText("Port");
	lblMailOutSecr.setText("Encryption");
	lblEMail.setText("Email Id");
	lblPassword.setText("Password");
	btnNew.setText("New");
	btnRem.setText("Remove");
	btnSave.setText("Save");

	widgetLayout.addWidget(&btnNew, 1, 1);
	widgetLayout.addWidget(&btnRem, 1, 3);

	widgetLayout.addWidget(&lblMailInGrp, 2, 0);
	widgetLayout.addWidget(&lblMailInServ, 4, 0);
	widgetLayout.addWidget(&lblMailInPort, 5, 0);
	widgetLayout.addWidget(&lblMailInSecr, 6, 0);

	widgetLayout.addWidget(&txtMailInServ, 4, 1);
	widgetLayout.addWidget(&txtMailInPort, 5, 1);
	widgetLayout.addWidget(&cmbMailInSecr, 6, 1);

	widgetLayout.addWidget(&lblMailOutGrp, 2, 2);
	widgetLayout.addWidget(&lblMailOutServ, 4, 2);
	widgetLayout.addWidget(&lblMailOutPort, 5, 2);
	widgetLayout.addWidget(&lblMailOutSecr, 6, 2);

	widgetLayout.addWidget(&txtMailOutServ, 4, 3);
	widgetLayout.addWidget(&txtMailOutPort, 5, 3);
	widgetLayout.addWidget(&cmbMailOutSecr, 6, 3);

	widgetLayout.addWidget(&lblName, 10, 0);
	widgetLayout.addWidget(&txtName, 10, 1);
	widgetLayout.addWidget(&lblEMail, 11, 0);
	widgetLayout.addWidget(&txtEMail, 11, 1);
	widgetLayout.addWidget(&lblPassword, 11, 2);
	widgetLayout.addWidget(&txtPassword, 11, 3);

	widgetLayout.addWidget(&btnSave, 12, 3);

	cmbMailInSecr.addItem("SSL");
	cmbMailInSecr.addItem("TLS");
	cmbMailInSecr.addItem("None");

	cmbMailOutSecr.addItem("None");
	cmbMailOutSecr.addItem("SSL");
	cmbMailOutSecr.addItem("TLS");

	txtPassword.setEchoMode(QLineEdit::Password);

	widgetLayout.addWidget(&lblError, 13, 0, 1, 4);

	lblError.setMinimumHeight(32);

    widgetLayout.setContentsMargins(QMargins(15, 15, 15, 15));
    widgetLayout.setSpacing(5);
    setLayout(&widgetLayout);
    setWindowTitle(tr("Email Profiles"));

    setMinimumHeight(415);
    setMaximumHeight(495);

    setMinimumWidth(480);
    setMaximumWidth(480);

    lstViewProfiles.setMinimumHeight(190);
    lstViewProfiles.setMaximumHeight(190);

    lstViewProfiles.setMinimumWidth(width() - 30);
    lstViewProfiles.setMaximumWidth(width() - 30);

    QStringList lst;

    lst << "Profile Name" << "Email Id" << " ";

    lstViewProfiles.setSelectionMode(QAbstractItemView::SingleSelection);

     lstViewProfiles.setHeaderLabels(lst);
     lstViewProfiles.setColumnWidth(0,150);
     lstViewProfiles.setColumnWidth(1,150);
     lstViewProfiles.setColumnWidth(2,50);

    QMetaObject::Connection c1 = connect(&btnNew, &QPushButton::clicked, this, &ProfileView::eventResetFields);
	QMetaObject::Connection c2 = connect(&btnRem, &QPushButton::clicked, this, &ProfileView::eventRemoveEntry);
	QMetaObject::Connection c3 = connect(&btnSave, &QPushButton::clicked, this, &ProfileView::eventSaveEntry);
	QMetaObject::Connection c4 = connect(&lstViewProfiles, &QTreeWidget::itemClicked, this, &ProfileView::eventProfileSelected);
}

void ProfileView::Initialize()
{
    std::vector<Profile> profiles;
    mailClientPtr->GetProfileList(profiles);

    if(profiles.size() < 1)
    {
        return;
    }

    for(auto ps : profiles)
    {
         QTreeWidgetItem *profileItem = new QTreeWidgetItem(&lstViewProfiles);
         profileItem->setText(0, ps.ProfileName.c_str());
         profileItem->setText(1, ps.EMailId.c_str());
         profileItem->setText(2, " ");
         profileItem->setData(0, Qt::UserRole, QVariant(ps.EMailId.c_str()));
     }
}

void ProfileView::eventProfileSelected(QTreeWidgetItem *item, int column)
{
    (void)column;
    currentRecord.EMailId = item->data(0, Qt::UserRole).toString().toStdString();
    currentItem = item;

    if(mailClientPtr->GetProfile(currentRecord.EMailId, currentRecord))
    {
		btnSave.setEnabled(true);
		btnRem.setEnabled(true);

		isEditMode = true;

        txtMailInServ.setText(currentRecord.MailInServer.c_str());
        txtMailInPort.setText(currentRecord.MailInPort.c_str());
        txtMailOutServ.setText(currentRecord.MailOutServer.c_str());
        txtMailOutPort.setText(currentRecord.MailOutPort.c_str());

        txtName.setText(currentRecord.ProfileName.c_str());
        txtEMail.setText(currentRecord.EMailId.c_str());
        txtPassword.setText(currentRecord.Password.c_str());

        char ich = currentRecord.MailInSecurity[0];
        char och = currentRecord.MailOutSecurity[0];

        if(ich == 'S')
            cmbMailInSecr.setCurrentIndex(0);

        if(ich == 'T')
            cmbMailInSecr.setCurrentIndex(1);

        if(ich == 'N')
            cmbMailInSecr.setCurrentIndex(2);


        if(och == 'N')
            cmbMailOutSecr.setCurrentIndex(0);

        if(och == 'S')
            cmbMailOutSecr.setCurrentIndex(1);

        if(och == 'T')
            cmbMailOutSecr.setCurrentIndex(2);
    }
}

void ProfileView::eventRemoveEntry()
{
    if(mailClientPtr->GetProfile(currentRecord.EMailId, currentRecord))
    {
        std::string prfremoved = currentRecord.EMailId;

        eventResetFields();

        mailClientPtr->RemoveProfile(currentRecord.ProfileName);

        currentRecord.ProfileName = "";
        currentRecord.EMailId = "";
        currentRecord.Password = "";
        currentRecord.MailInServer = "";
        currentRecord.MailInPort = "";
        currentRecord.MailInSecurity = "";
        currentRecord.MailOutServer = "";
        currentRecord.MailOutPort = "";
        currentRecord.MailOutSecurity = "";
        currentRecord.LastSync ="19800101";

        int idx = lstViewProfiles.indexOfTopLevelItem(currentItem);
        currentItem = lstViewProfiles.takeTopLevelItem(idx);

        delete currentItem;
        currentItem = nullptr;

		emit ProfileRemoved(currentRecord.ProfileName);

		btnSave.setEnabled(false);
		btnRem.setEnabled(false);
    }
}

void ProfileView::eventResetFields()
{
	isEditMode = false;

	btnSave.setEnabled(true);
	btnRem.setEnabled(false);

    txtMailInServ.setText("");
    txtMailInPort.setText("993");
    txtName.setText("");

    txtMailOutServ.setText("");
    txtMailOutPort.setText("587");
    txtEMail.setText("");

    cmbMailInSecr.setCurrentIndex(0);
    cmbMailOutSecr.setCurrentIndex(0);

    currentRecord.ProfileName = "";
    currentRecord.EMailId = "";

    currentRecord.MailInServer = "";
    currentRecord.MailInPort = "";
    currentRecord.MailInSecurity = "";

    currentRecord.MailOutServer = "";
    currentRecord.MailOutPort = "";
    currentRecord.MailOutSecurity = "";
}

void ProfileView::eventSaveEntry()
{
    lblError.setText("");

    currentRecord.ProfileName = txtName.text().toStdString();
    currentRecord.EMailId = txtEMail.text().toStdString();
    currentRecord.Password = txtPassword.text().toStdString();

    currentRecord.MailInServer = txtMailInServ.text().toStdString();
    currentRecord.MailInPort = txtMailInPort.text().toStdString();
    currentRecord.MailInSecurity = cmbMailInSecr.currentText().at(0).toLatin1();

    currentRecord.MailOutServer = txtMailOutServ.text().toStdString();
    currentRecord.MailOutPort = txtMailOutPort.text().toStdString();
    currentRecord.MailOutSecurity = cmbMailOutSecr.currentText().at(0).toLatin1();

    if(! isEditMode)
    {
        currentRecord.LastSync = "01-JAN-1980";
        if(mailClientPtr->AddProfile(currentRecord))
        {
            QTreeWidgetItem *profileItem = new QTreeWidgetItem(&lstViewProfiles);
            profileItem->setText(0, currentRecord.ProfileName.c_str());
            profileItem->setText(1, currentRecord.EMailId.c_str());
            profileItem->setText(2, " ");
            profileItem->setData(0, Qt::UserRole, QVariant(currentRecord.EMailId.c_str()));
			emit ProfileAdded(currentRecord.ProfileName);
        }
    }
    else
    {
        mailClientPtr->UpdateProfile(currentRecord);
		emit ProfileUpdated(currentRecord.ProfileName);
	}

	close();
}

void ProfileView::showEvent(QShowEvent* event)
{
	if (mailClientPtr->ProfileList()->size() < 1)
	{
		eventResetFields();
	}
}
