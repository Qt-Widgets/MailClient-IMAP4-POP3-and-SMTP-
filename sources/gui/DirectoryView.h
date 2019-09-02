#ifndef _DIRECTORY_VIEW
#define _DIRECTORY_VIEW

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QtCore>
#include <QMap>
#include "../../data/Mail.h"
#include <vector>
#include <string>

class DirectoryView : public QTreeWidget
{
    Q_OBJECT
public:
    DirectoryView(QWidget *parent = nullptr);
    ~DirectoryView();
    void Initialize();

	void Refresh();
	void Expunge();
	void Search(std::string searchterm);

    void AddProfile(std::string emailId);
    void RemoveProfile(std::string emailId);
    void UpdateProfile(std::string emailId);

signals:
    void LoadDirectory(std::string uname, std::string dirname, std::string&uidlist);
	void MailListRecieved(std::vector<MailHeader>& mails, std::vector<MailStorageInformation>& storgaeinfo);
	void ClearMailView();

private slots:
    void eventDirectorySelected(QTreeWidgetItem *item, int column);

private:
    void InitializeDirectoryList(std::string uname, std::vector<std::string> slist);
};

#endif // DIRECTORYVIEW_H
