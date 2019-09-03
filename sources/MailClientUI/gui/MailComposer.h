#ifndef _MAIL_COMPOSER
#define _MAIL_COMPOSER

#include <QtCore>
#include <QFileDialog>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QMovie>
#include <QCheckBox>

#include "../../data/Profile.h"
#include "../../network/Mail.h"
#include "ThemeHandler.h"
#include "ContactsView.h"

typedef enum CompositionMode
{
	Forward,
	Reply,
	ReplyAll,
	New
}CompositionMode;

class MailComposer : public QWidget
{
    Q_OBJECT
public:
    MailComposer(QWidget *parent = 0);
    void SetEMail(Mail &eml, CompositionMode mode);
	void Reset();
	void LoadProfiles();

signals:
	void Finished();

private slots:
	void eventSelectTo();
	void eventSelectCc();
	void eventSelectBcc();
    void eventSend();
	void eventDiscard();
    void eventAttach();

private:
	void SelectContacts(QLineEdit* dest);
	void DisableInputs();
	void EnableInputs();

	void DisplayViewableNode(MimeNode& node, MailHeader &hdr, CompositionMode &mode);
	void ListAttachmentNode(MimeNode& node);

    QGridLayout layout;
    QGridLayout mainLayout;

    QPushButton btnTo;
    QPushButton btnCC;
    QPushButton btnBCC;
    QLabel lblSubject;
    QLabel lblProfile;
    QPushButton btnAttach;

    QLineEdit txtTo;
    QLineEdit txtCC;
    QLineEdit txtBCC;
    QLineEdit txtSubject;
    QComboBox cbxProfiles;
    QListWidget lstAttachments;

    QPushButton btnSend;
	QCheckBox chkUseHtml;
	QPushButton btnDiscard;
	QTextEdit   editor;

	ContactsView contacts;

	Mail		email;
	MailStorageInformation minfo;
};

#endif
