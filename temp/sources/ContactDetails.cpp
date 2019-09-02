#include "ContactDetails.h"
#include "../../uicommon/ThemeHandler.h"
#include "../../uicommon/VerticalLine.h"
#include "../../uicommon/HorizontalLine.h"

ContactDetails::ContactDetails(QWidget *parent)
    : QWidget(parent)
{
    firstnameLabel.setText("First Name");
    middlenameLabel.setText("Middle Name");
    lastnameLabel.setText("Last Name");
    nicknameLabel.setText("Nick Name");
    emailLabel.setText("Email");

    workphoneLabel.setText("Work Phone");
    homephoneLabel.setText("Home Phone");
    faxLabel.setText("Fax");
    mobilenoLabel.setText("Mobile Phone");
    websiteLabel.setText("Website");

    addressLine1Label.setText("Address Line 1");
    addressLine2Label.setText("Address Line 2");
    cityLabel.setText("City");
    stateLabel.setText("State");
    countryLabel.setText("Country");
    zipcodeLabel.setText("Zip Code");

    anniversaryLabel.setText("Date Of Birth");
    notesLabel.setText("Notes");
    pictureLabel.setText("Picture");

    saveButton.setText("Save");
    resetButton.setText("Reset");
    picWidget.setText("Click to \n attach picture");
    picWidget.setFixedSize(QSize(150, 150));

    gLayout.setColumnStretch(1, 2);

    gLayout.addWidget(&emailLabel, 0, 0);
    gLayout.addWidget(&emailText, 0, 1);    
	gLayout.addWidget(&firstnameLabel, 1, 0);
    gLayout.addWidget(&firstnameText, 1, 1);
    gLayout.addWidget(&middlenameLabel, 2, 0);
    gLayout.addWidget(&middlenameText, 2, 1);
    gLayout.addWidget(&lastnameLabel, 3, 0);
    gLayout.addWidget(&lastnameText, 3, 1);
    gLayout.addWidget(&nicknameLabel, 4, 0);
    gLayout.addWidget(&nicknameText, 4, 1);

    gLayout.addWidget(&workphoneLabel, 0, 2);
    gLayout.addWidget(&workphoneText, 0, 3);
    gLayout.addWidget(&homephoneLabel, 1, 2);
    gLayout.addWidget(&homephoneText, 1, 3);
    gLayout.addWidget(&faxLabel, 2, 2);
    gLayout.addWidget(&faxText, 2, 3);
    gLayout.addWidget(&mobilenoLabel, 3, 2);
    gLayout.addWidget(&mobilenoText, 3, 3);
    gLayout.addWidget(&websiteLabel, 4, 2);
    gLayout.addWidget(&websiteText, 4, 3);

	gLayout.addWidget(new HorizontalLine(), 5, 0, 1, 4);

    gLayout.addWidget(&addressLine1Label, 6, 0);
    gLayout.addWidget(&addressLine1Text, 6, 1);
    gLayout.addWidget(&addressLine2Label, 6, 2);
    gLayout.addWidget(&addressLine2Text, 6, 3);
    gLayout.addWidget(&cityLabel, 7, 0);
    gLayout.addWidget(&cityText, 7, 1);
    gLayout.addWidget(&stateLabel, 7, 2);
    gLayout.addWidget(&stateText, 7, 3);
    gLayout.addWidget(&countryLabel, 8, 0);
    gLayout.addWidget(&countryText, 8, 1);
    gLayout.addWidget(&zipcodeLabel, 8, 2);
    gLayout.addWidget(&zipcodeText, 8, 3);

	gLayout.addWidget(new HorizontalLine(), 9, 0, 1, 4);

    gLayout.addWidget(&notesLabel, 10, 1);
    gLayout.addWidget(&notesText, 11, 1);
    gLayout.addWidget(&anniversaryLabel, 10, 2, Qt::AlignTop);
    gLayout.addWidget(&anniversaryText, 10, 3, Qt::AlignTop);
    gLayout.addWidget(&picWidget, 11, 3, Qt::AlignCenter);

    buttonLayout.addWidget(&saveButton);
    buttonLayout.addWidget(&resetButton);

    gLayout.addLayout(&buttonLayout, 12, 3, Qt::AlignRight);

    mainLayout.addLayout(&gLayout);
    setLayout(&mainLayout);

	QMetaObject::Connection conn1 = connect(&saveButton, &QAbstractButton::clicked, this, &ContactDetails::eventSaveRecord);
	QMetaObject::Connection conn2 = connect(&resetButton, &QAbstractButton::clicked, this, &ContactDetails::eventResetRecord);
    QMetaObject::Connection conn3 = connect(&picWidget, &PictureBox::clicked, this, &ContactDetails::eventPictureSelect);

    setWindowTitle(tr("Contact Information"));
    QIcon icon(":/images/contact.png");
    setWindowIcon(icon);
}

void ContactDetails::eventPictureSelect()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Set picture",  QDir::homePath(), "Image Files (*.png *.jpg *.bmp)");

    if(fileName.length() < 1)
    {
        return;
    }

    QPixmap pix;
    pix.load(fileName);
    QPixmap scaledpix = pix.scaled(150, 150);

    picWidget.setPixmap(scaledpix);
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    scaledpix.save(&buffer, "PNG");
    pixBase64 = QString::fromLatin1(byteArray.toBase64().data());
}

Contact ContactDetails::GetRecord()
{
    Contact struc;

    struc.FirstName = firstnameText.text().toStdString();
    struc.MiddleName = middlenameText.text().toStdString();
    struc.LastName = lastnameText.text().toStdString();
    struc.EmailId = emailText.text().toStdString();
    struc.WebSite = websiteText.text().toStdString();
    struc.AddressLine1 = addressLine1Text.text().toStdString();
    struc.AddressLine2 = addressLine2Text.text().toStdString();
    struc.City = cityText.text().toStdString();
    struc.State = stateText.text().toStdString();
    struc.Country = countryText.text().toStdString();
    struc.ZipCode = zipcodeText.text().toStdString();
    struc.WorkPhone = workphoneText.text().toStdString();
    struc.HomePhone = homephoneText.text().toStdString();
    struc.Fax = faxText.text().toStdString();
    struc.MobileNo = mobilenoText.text().toStdString();
    struc.NickName = nicknameText.text().toStdString();
    struc.Anniversary = anniversaryText.dateTime().toUTC().toString("yyyyddMMhhmmss").toStdString();
    struc.Notes = notesText.toPlainText().toStdString();
    struc.Photo = pixBase64.toStdString();

     if(struc.WebSite.length() < 1)
        struc.WebSite = "_";   
	 
	 struc.AlternateEmailIds = struc.EmailId;

    if(struc.FirstName.length() < 1)
        struc.FirstName = "_";

    if(struc.MiddleName.length() < 1)
        struc.MiddleName = "_";

    if(struc.LastName.length() < 1)
        struc.LastName = "_";

    if(struc.EmailId.length() < 1)
        struc.EmailId = "_";

    if(struc.AddressLine1.length() < 1)
        struc.AddressLine1 = "_";

    if(struc.AddressLine2.length() < 1)
        struc.AddressLine2 = "_";

    if(struc.City.length() < 1)
        struc.City = "_";

    if(struc.State.length() < 1)
        struc.State = "_";

    if(struc.Country.length() < 1)
        struc.Country = "_";

    if(struc.ZipCode.length() < 1)
        struc.ZipCode = "_";

    if(struc.WorkPhone.length() < 1)
        struc.WorkPhone = "_";

    if(struc.HomePhone.length() < 1)
        struc.HomePhone = "_";

    if(struc.Fax.length() < 1)
        struc.Fax = "_";

    if(struc.MobileNo.length() < 1)
        struc.MobileNo = "_";

    if(struc.NickName.length() < 1)
        struc.NickName = "_";

    if(struc.Anniversary.length() < 1)
        struc.Anniversary = "_";

    if(struc.Notes.length() < 1)
        struc.Notes = "_";

    if(struc.Photo.length() < 1)
        struc.Photo = "_";

    return struc;
}

void ContactDetails::SetRecord(const Contact &rec)
{
    firstnameText.setText(rec.FirstName.c_str());
    middlenameText.setText(rec.MiddleName.c_str());
    lastnameText.setText(rec.LastName.c_str());
    emailText.setText(rec.EmailId.c_str());
    websiteText.setText(rec.WebSite.c_str());
    addressLine1Text.setText(rec.AddressLine1.c_str());
    addressLine2Text.setText(rec.AddressLine2.c_str());
    cityText.setText(rec.City.c_str());
    stateText.setText(rec.State.c_str());
    countryText.setText(rec.Country.c_str());
    zipcodeText.setText(rec.ZipCode.c_str());
    workphoneText.setText(rec.WorkPhone.c_str());
    homephoneText.setText(rec.HomePhone.c_str());
    faxText.setText(rec.Fax.c_str());
    mobilenoText.setText(rec.MobileNo.c_str());
    nicknameText.setText(rec.NickName.c_str());
    notesText.setPlainText(rec.Notes.c_str());
    pixBase64 = rec.Photo.c_str();

	QDateTime dt = QDateTime::fromString(rec.Anniversary.c_str(), "ddMMyyyy");
	anniversaryText.setDateTime(dt);

	QPixmap scaledpix;
	QImage image;

	bool image_ok = false;

	if (pixBase64.length() > 0)
	{
		QByteArray base64buff;
		base64buff.append(pixBase64);
		QByteArray by = QByteArray::fromBase64(base64buff);
		image = QImage::fromData(by, "PNG");

		if (!image.isNull())
		{
			scaledpix = QPixmap::fromImage(image).scaled(159, 159);
			image_ok = true;
		}
	}
	
	if(!image_ok)
	{
		image.load(":/images/user-icon.png");
		scaledpix = QPixmap::fromImage(image).scaled(159, 159);
	}

    picWidget.setPixmap(scaledpix);

    emailText.setEnabled(false);
}

void ContactDetails::ResetFields()
{
    firstnameText.setText("");
    middlenameText.setText("");
    lastnameText.setText("");
    emailText.setText("");
    websiteText.setText("");
    addressLine1Text.setText("");
    addressLine2Text.setText("");
    cityText.setText("");
    stateText.setText("");
    countryText.setText("");
    zipcodeText.setText("");
    workphoneText.setText("");
    homephoneText.setText("");
    faxText.setText("");
    mobilenoText.setText("");
    nicknameText.setText("");
	anniversaryText.setDateTime(QDateTime::currentDateTime());
    notesText.setPlainText("");

    picWidget.setText("Click to \n attach picture");

    emailText.setEnabled(true);
}

void ContactDetails::eventSaveRecord()
{
	emit SaveCurrentRecord();
}

void ContactDetails::eventResetRecord()
{
	ResetFields();
}