#ifndef ADDRESS_BOOK
#define ADDRESS_BOOK

#include <QtCore>
#include <QObject>
#include <QStringBuilder>
#include <QUrlQuery>
#include <QDebug>
#include <QList>
#include <QPair>
#include <QApplication>
#include "MainWindow.h"
#include "../../utils/CommQueue.h"
#include "../../data/Contact.h"
#include "ContactOperations.h"

class AddressBook : public QApplication, public ContactOperations
{
    Q_OBJECT
public:
    explicit AddressBook(int argc, char *argv[]);
    virtual ~AddressBook();
    bool SetupCommQueue();
    void ShowWindow();

	bool SearchContacts(std::vector<std::string> &ctlist, std::string &term);
	bool GetAllContacts(std::vector<std::string> &ctlist);
    bool GetContact(const std::string &contactId, Contact &obj);
    bool AddContact(const Contact &obj);
    bool UpdateContact(const Contact &obj);
    bool RemoveContact(const std::string &contactId);

private slots:
	void eventSwitchToLightTheme();
	void eventSwitchToDarkTheme();

private:
    void SerializeContact(const Contact &obj, string &str);
    MainWindow appWindow;
    CommQueue requestQueue;
};

#endif
