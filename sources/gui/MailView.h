#ifndef _MAIL_VIEW
#define _MAIL_VIEW

#include <QtCore>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QListWidget>
#include <QPushButton>

#include "../../network/Mail.h"
#include "ThemeHandler.h"

class MailView : public QWidget
{
    Q_OBJECT
public:
    MailView(QWidget *parent = 0);
    void SetEMail(Mail eml);
    void Reset();

private:
	void DisplayViewableNode(MimeNode &node);
	void ListAttachmentNode(MimeNode& node);

    QGridLayout layout;
	QVBoxLayout mainLayout;

    QLabel lblFrom;
    QLabel lblTo;
    QLabel lblCC;
    QLabel lblBCC;
    QLabel lblSubject;
    QLabel lblTimeStamp;
    QLabel lblAttachments;

    QLabel txtFrom;
    QLabel txtTo;
    QLabel txtCC;
    QLabel txtBCC;
    QLabel txtSubject;
    QLabel txtTimeStamp;
    QListWidget lstAttachments;

    QTextBrowser viewer;
};

#endif
