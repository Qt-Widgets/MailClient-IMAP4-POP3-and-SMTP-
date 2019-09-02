#include "MailView.h"

QString blank_html = "<html><title></title><body></body></html>";

MailView::MailView(QWidget *parent) : QWidget(parent)
{
    layout.addWidget(&lblFrom, 0, 0, 1, 1);
    layout.addWidget(&txtFrom, 0, 1, 1, 1);

    layout.addWidget(&lblTo, 1, 0, 1, 1);
    layout.addWidget(&txtTo, 1, 1, 1, 1);

    layout.addWidget(&lblCC, 2, 0, 1, 1);
    layout.addWidget(&txtCC, 2, 1, 1, 1);

    layout.addWidget(&lblBCC, 3, 0, 1, 1);
    layout.addWidget(&txtBCC, 3, 1, 1,1);

    layout.addWidget(&lblSubject, 4, 0, 1, 1);
    layout.addWidget(&txtSubject, 4, 1, 1, 1);

    layout.addWidget(&lblTimeStamp, 5, 0, 1, 1);
    layout.addWidget(&txtTimeStamp, 5, 1, 1, 1);

    lblFrom.setText("From: ");
    lblTo.setText("To: ");
    lblCC.setText("Cc: ");
    lblBCC.setText("Bcc: ");
    lblTimeStamp.setText("Date & Time:");
    lblAttachments.setText("Attachments: ");
    lblSubject.setText("Subject: ");

    lblFrom.setMaximumWidth(75);
    lblTo.setMaximumWidth(75);
    lblCC.setMaximumWidth(75);
    lblBCC.setMaximumWidth(75);
    lblTimeStamp.setMaximumWidth(75);
    lblAttachments.setMaximumWidth(75);
    lblSubject.setMaximumWidth(75);

    lstAttachments.setFlow(QListWidget::LeftToRight);
    lstAttachments.setViewMode(QListView::IconMode);
    lstAttachments.setMovement(QListView::Static);
    lstAttachments.setMaximumHeight(48);
    lstAttachments.setMinimumHeight(48);
    lstAttachments.setSpacing(0);

    lblFrom.setVisible(false);
    lblTo.setVisible(false);
    lblCC.setVisible(false);
    lblBCC.setVisible(false);
    lblSubject.setVisible(false);
    lblTimeStamp.setVisible(false);
    lblAttachments.setVisible(false);
    lstAttachments.setVisible(false);

	layout.setSpacing(0);
	layout.setMargin(5);
	mainLayout.addLayout(&layout);
	mainLayout.addWidget(&viewer);
	mainLayout.addWidget(&lstAttachments);
	mainLayout.setSpacing(0);
	mainLayout.setMargin(0);
	setLayout(&mainLayout);
 }

void MailView::SetEMail(Mail eml)
{
	lstAttachments.clear();
	lstAttachments.setVisible(false);
	viewer.setHtml(blank_html);

    lblFrom.setVisible(true);
    lblTo.setVisible(true);
    lblCC.setVisible(true);
    lblBCC.setVisible(true);
    lblSubject.setVisible(true);
    lblTimeStamp.setVisible(true);

    txtFrom.setText(eml.Header.GetFrom().c_str());
    txtTo.setText(eml.Header.GetTo().c_str());
    txtCC.setText(eml.Header.GetCc().c_str());
    txtBCC.setText(eml.Header.GetBcc().c_str());
    txtSubject.setText(eml.Header.GetSubject().c_str());
    txtTimeStamp.setText(eml.Header.GetTimeStamp().c_str());

	for (auto nd : eml.Body.GetDataNodes())
	{
		DisplayViewableNode(nd);
		ListAttachmentNode(nd);
	}
}

void MailView::Reset()
{
    txtFrom.setText("");
    txtTo.setText("");
    txtCC.setText("");
    txtBCC.setText("");
    txtSubject.setText("");
    txtTimeStamp.setText("");
    viewer.setHtml("");
    lstAttachments.clear();

    lblFrom.setVisible(false);
    lblTo.setVisible(false);
    lblCC.setVisible(false);
    lblBCC.setVisible(false);
    lblSubject.setVisible(false);
    lblTimeStamp.setVisible(false);
    lblAttachments.setVisible(false);
}

void MailView::DisplayViewableNode(MimeNode& node)
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
		viewer.setHtml(html_string);
	}

	for (auto nd : node.ChildNodes)
	{
		DisplayViewableNode(nd);
	}
}

void MailView::ListAttachmentNode(MimeNode& node)
{
	if (node.TextEncoding == Base64 && (node.NodeType == Attachment || node.NodeType == InlineImage))
	{
		lstAttachments.addItem(node.NodeName.c_str());

		QListWidgetItem* newItem = new QListWidgetItem(&lstAttachments);
		newItem->setData(Qt::UserRole, node.NodeName.c_str());

		lstAttachments.setVisible(true);
	}

	for (auto nd : node.ChildNodes)
	{
		ListAttachmentNode(nd);
	}
}
