#ifndef _CONTACT_DETAILS
#define _CONTACT_DETAILS

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QCalendarWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateEdit>
#include <QPixmap>
#include <QIcon>
#include <QDir>
#include <QFileDialog>
#include <QByteArray>
#include <QBuffer>
#include "../data/Contact.h"
#include "PictureBox.h"

class ContactDetails : public QWidget
{
    Q_OBJECT
public:
	ContactDetails(QWidget *parent = 0);
    
    Contact GetRecord();
    void SetRecord(const Contact &rec);
    void ResetFields();

signals:
	void SaveCurrentRecord();

private slots:
    void eventPictureSelect();
	void eventSaveRecord();
	void eventResetRecord();

private:
    QLabel firstnameLabel;
    QLabel middlenameLabel;
    QLabel lastnameLabel;
    QLabel nicknameLabel;
    QLabel emailLabel;

    QLabel addressLine1Label;
    QLabel addressLine2Label;
    QLabel cityLabel;
    QLabel stateLabel;
    QLabel countryLabel;
    QLabel zipcodeLabel;

    QLabel workphoneLabel;
    QLabel homephoneLabel;
    QLabel faxLabel;
    QLabel mobilenoLabel;
    QLabel websiteLabel;

    QLabel anniversaryLabel;
    QLabel notesLabel;
    QLabel pictureLabel;

    ////////////////////

    QLineEdit firstnameText;
    QLineEdit middlenameText;
    QLineEdit lastnameText;
    QLineEdit nicknameText;
    QLineEdit emailText;

    QLineEdit addressLine1Text;
    QLineEdit addressLine2Text;
    QLineEdit cityText;
    QLineEdit stateText;
    QLineEdit countryText;
    QLineEdit zipcodeText;

    QLineEdit workphoneText;
    QLineEdit homephoneText;
    QLineEdit faxText;
    QLineEdit mobilenoText;
    QLineEdit websiteText;

	QDateEdit anniversaryText;
    QTextEdit notesText;
    QString pixBase64;

    /////////////////////

    QPushButton saveButton;
    QPushButton resetButton;
	PictureBox picWidget;

    QGridLayout gLayout;
    QHBoxLayout buttonLayout;
    QVBoxLayout mainLayout;
};

#endif
