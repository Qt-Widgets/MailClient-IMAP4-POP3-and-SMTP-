#include "MailComposer.h"
#include "../app/MailClient.h"
#include "../utils/StringEx.h"

MailComposer::MailComposer(QWidget *parent) : QWidget(parent)
{
	btnTo.setText("To");
	btnCC.setText("Cc");
	btnBCC.setText("Bcc");
	lblProfile.setText("Use Profile");
	btnSend.setText("Send");
	btnAttach.setText("Attach");
	btnDiscard.setText("Discard");
	chkUseHtml.setText("Send as HTML");
	lblSubject.setText("Subject");

    layout.addWidget(&btnSend, 0, 0, 1, 1);

    layout.addWidget(&btnTo, 1, 0, 1, 1);
    layout.addWidget(&txtTo, 1, 1, 1, 1);

    layout.addWidget(&btnCC, 2, 0, 1, 1);
    layout.addWidget(&txtCC, 2, 1, 1, 1);

    layout.addWidget(&btnBCC, 3, 0, 1, 1);
    layout.addWidget(&txtBCC, 3, 1, 1,1);

    layout.addWidget(&lblSubject, 4, 0, 1, 1);
    layout.addWidget(&txtSubject, 4, 1, 1, 1);

    layout.addWidget(&lblProfile, 5, 0, 1, 1);
    layout.addWidget(&cbxProfiles, 5, 1, 1, 1);

    layout.addWidget(&btnAttach, 6, 0, 1, 1);
    layout.addWidget(&lstAttachments, 6, 1, 1, 1);

	layout.addWidget(&chkUseHtml, 7, 0, 1, 1);
	layout.addWidget(&editor, 8, 0, 1, 2);
	layout.addWidget(&btnDiscard, 9, 0, 1, 1);

    lstAttachments.setFlow(QListWidget::LeftToRight);
    lstAttachments.setViewMode(QListView::ListMode);
    lstAttachments.setMovement(QListView::Static);
    lstAttachments.setMaximumHeight(48);
    lstAttachments.setMinimumHeight(48);
    lstAttachments.setSpacing(0);

    cbxProfiles.setMaximumWidth(150);

    mainLayout.addLayout(&layout, 0, 0);
    setLayout(&mainLayout);

    setMinimumHeight(600);
    setMinimumWidth(800);
	cbxProfiles.setMinimumWidth(175);

    auto c1 = connect(&btnTo, &QAbstractButton::clicked, this, &MailComposer::eventSelectTo);
	auto c2 = connect(&btnCC, &QAbstractButton::clicked, this, &MailComposer::eventSelectCc);
	auto c3 = connect(&btnBCC, &QAbstractButton::clicked, this, &MailComposer::eventSelectBcc);
	auto c4 = connect(&btnAttach, &QAbstractButton::clicked, this, &MailComposer::eventAttach);
	auto c5 = connect(&btnSend, &QAbstractButton::clicked, this, &MailComposer::eventSend);
	auto c6 = connect(&btnDiscard, &QAbstractButton::clicked, this, &MailComposer::eventDiscard);
 }

void MailComposer::LoadProfiles()
{
	cbxProfiles.clear();

	for (auto pf : *mailClientPtr->ProfileList())
	{
        cbxProfiles.addItem(pf.EMailId.c_str(), pf.ProfileName.c_str());
    }

	if (mailClientPtr->ProfileList()->size() == 1)
	{
		email.Header.SetFrom((*mailClientPtr->ProfileList())[0].EMailId);
		email.Header.AddHeader("Account", (*mailClientPtr->ProfileList())[0].ProfileName);
	}
}

void MailComposer::eventSelectTo()
{
	SelectContacts(&txtTo);
}

void MailComposer::eventSelectCc()
{
	SelectContacts(&txtCC);
}

void MailComposer::eventSelectBcc()
{
	SelectContacts(&txtBCC);
}

void MailComposer::SelectContacts(QLineEdit* dest)
{
	contacts.LoadContacts();
	contacts.exec();

	std::string str = dest->text().toStdString();

	std::vector<std::string> clist;
	contacts.GetSelectedContacts(clist);

	if (clist.size() > 0)
	{
		if (str.length() > 0)
		{
			str += ",";
		}		
		
		for (auto s : clist)
		{
			str += s + ",";
		}

		str.erase(str.size() - 1, 1);

		dest->setText(str.c_str());
	}
}

void MailComposer::eventSend()
{   
    if(txtTo.text().length() < 1)
    {
        return;
    }

    if(txtSubject.text().length() < 1 || editor.toPlainText().length() < 1)
    {
        return;
    }

	Mail eml;

	QString ts = QDateTime::currentDateTime().toString("ddd, dd MMM yyyy hh:mm:ss t");

	QString message_id = QUuid::createUuid().toString(QUuid::WithoutBraces).remove('-') + "@mailclient.com";

	eml.Header.SetTimeStamp(ts.toStdString());
	eml.Header.AddHeader("Account", cbxProfiles.currentData().toString().toStdString());
	eml.Header.SetFrom(cbxProfiles.currentText().toStdString());
	eml.Header.SetMessageId(message_id.toStdString());
	eml.Header.AddHeader("Directory", "Sent");
	eml.Header.AddtoToList(txtTo.text().toStdString());
	eml.Header.AddtoCcList(txtCC.text().toStdString());
	eml.Header.AddtoBccList(txtBCC.text().toStdString());
	eml.Header.SetSubject(txtSubject.text().toStdString());
	eml.Header.AddHeader("Directory", "O");
	eml.Header.AddHeader("Directory", "-1");

	MimeNode text_node;

	if (chkUseHtml.checkState() == Qt::CheckState::Checked)
	{
		text_node.Data += "<html>";
		text_node.Data += "<title>";
		text_node.Data += txtSubject.text().toStdString();
		text_node.Data += "</title>";
		text_node.Data += "<body>";
		text_node.Data += editor.toPlainText().toStdString();
		text_node.Data += "</body>";
		text_node.Data += "</html>";
		text_node.NodeType = Html;
	}
	else
	{
		text_node.Data = editor.toPlainText().toStdString();
		text_node.NodeType = PlainText;
	}

	eml.Body.AddNode(text_node);
	eml.Body.SetMessageId(eml.Header.GetMessageId());

	for (int i = 0; i < lstAttachments.count(); ++i)
	{
		QListWidgetItem* item = lstAttachments.item(i);

		MimeNode attachment_node;

		attachment_node.NodeType = Attachment;
		attachment_node.NodeName = item->data(Qt::UserRole).toString().toStdString();
		eml.Body.AddNode(attachment_node);
	}

    DisableInputs();

	if (!mailClientPtr->SendEmail(eml))
	{
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error");
        msgBox.setText("Mail could not be sent");
        msgBox.exec();
	}

	Reset();

	EnableInputs();

	emit Finished();
}

void MailComposer::eventDiscard()
{
	emit Finished();
}

void MailComposer::eventAttach()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),  QDir::homePath(), "All files (*.*)");

    if(!filename.isEmpty())
    {
		QString complete_base_name = QFileInfo(filename).baseName() + "." + QFileInfo(filename).completeSuffix();
		QListWidgetItem* newItem = new QListWidgetItem(&lstAttachments);
		newItem->setData(Qt::DisplayRole, complete_base_name);
		newItem->setData(Qt::UserRole, filename);
	}
}

void MailComposer::DisableInputs()
{
   btnTo.setEnabled(false);
   btnCC.setEnabled(false);
   btnBCC.setEnabled(false);
   btnAttach.setEnabled(false);
   btnSend.setEnabled(false);

   txtTo.setEnabled(false);
   txtCC.setEnabled(false);
   txtBCC.setEnabled(false);
   txtSubject.setEnabled(false);
   cbxProfiles.setEnabled(false);
   lstAttachments.setEnabled(false);
   editor.setEnabled(false);
}

void MailComposer::EnableInputs()
{
    btnTo.setEnabled(true);
    btnCC.setEnabled(true);
    btnBCC.setEnabled(true);
    btnAttach.setEnabled(true);
    btnSend.setEnabled(true);

    txtTo.setEnabled(true);
    txtCC.setEnabled(true);
    txtBCC.setEnabled(true);
    txtSubject.setEnabled(true);
    cbxProfiles.setEnabled(true);
    lstAttachments.setEnabled(true);
    editor.setEnabled(true);
}

void MailComposer::SetEMail(Mail &eml, CompositionMode mode)
{
	email = eml;	
	cbxProfiles.setCurrentText(email.Header.GetTo().c_str());

	if (mode == New)
	{
		Reset();
		return;
	}

	chkUseHtml.setVisible(false);
	cbxProfiles.setEnabled(false);

    txtSubject.setText(email.Header.GetSubject().c_str());

	if (mode == Reply)
	{
		txtTo.setText(email.Header.GetFrom().c_str());
	}

	if (mode == ReplyAll)
	{
		QString to_str;
		to_str += email.Header.GetFrom().c_str();
		to_str += ", ";
		to_str += email.Header.GetCc().c_str();
	}

	for (auto nd : eml.Body.GetDataNodes())
	{
		DisplayViewableNode(nd, eml.Header, mode);
		ListAttachmentNode(nd);
	}
}

void MailComposer::Reset()
{
	txtTo.setText("");
	txtCC.setText("");
	txtBCC.setText("");
	txtSubject.setText("");
	editor.setText("");
	chkUseHtml.setVisible(true);
	cbxProfiles.setEnabled(true);
	lstAttachments.clear();
}

void MailComposer::DisplayViewableNode(MimeNode& node, MailHeader& hdr, CompositionMode& mode)
{
	if (node.TextEncoding == QuotedPrintable)
	{
		QString html_string;

		if (node.NodeType == PlainText)
		{
			html_string += "<html>";
			html_string += "<title>";
			html_string += "</title>";
			html_string += "<body>";
			html_string += node.Data.c_str();
			html_string += "</body>";
			html_string += "</html>";
			html_string.replace("\n", "<br>");
		}
		else
		{
			html_string = node.Data.c_str();
		}

		if (mode != New)
		{
			QString old_header;

			old_header += "<br><br><br><br><br><br><br><br><br><br>";

			old_header += "____________________________________<br>";

			old_header += "From: ";
			old_header += hdr.GetFrom().c_str();
			old_header += "<br>";

			old_header += "Sent To: ";
			old_header += hdr.GetTo().c_str();

			if (hdr.GetCc().length() > 1)
			{
				old_header += ", ";
				old_header += hdr.GetCc().c_str();
			}

			old_header += "<br>";

			old_header += "Sent On: ";
			old_header += hdr.GetTimeStamp().c_str();
			old_header += "<br>";

			old_header += "Subject: ";
			old_header += hdr.GetSubject().c_str();
			old_header += "<br>";

			int pos = html_string.indexOf("<body", Qt::CaseInsensitive);

			if (pos > -1)
			{
				for ( ; html_string[pos] != '>' ; pos++)
				{

				}

				html_string.insert(pos+1, old_header);
			}
		}

		editor.setHtml(html_string);
	}

	for (auto nd : node.ChildNodes)
	{
		DisplayViewableNode(nd, hdr, mode);
	}
}

void MailComposer::ListAttachmentNode(MimeNode& node)
{
	if (node.TextEncoding == Base64 && (node.NodeType == Attachment || node.NodeType == InlineImage))
	{
		lstAttachments.addItem(node.NodeName.c_str());
	}

	for (auto nd : node.ChildNodes)
	{
		ListAttachmentNode(nd);
	}
}
