#ifndef _ADDRESS_BOOK_WINDOW
#define _ADDRESS_BOOK_WINDOW

#include <QDialog>
#include <QWidget>
#include <QToolBar>
#include <QtCore>
#include <QCheckBox>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTreeWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QAction>
#include <QFileDialog>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QGridLayout>
#include <vector>
#include <string>
#include "ContactDetails.h"
#include "../app/ContactOperations.h"
#include "RichDataList.h"

class AddressBookWindow : public QWidget
{
    Q_OBJECT
public:
	AddressBookWindow(QWidget* parent = nullptr);
    void SetOpsHandler(ContactOperations* ptr);

signals:
	void SwitchToDarkTheme();
	void SwitchToLightTheme();

private slots:
    void eventLoadContacts(std::vector<std::string> &ctlist);
	void eventContactSelected(QListWidgetItem* item);
	void eventAddEntry();
    void eventRemoveEntry();
    void eventSaveEntry();
    void eventImportEntries();
    void eventSearch();
	void eventSwitchTheme();

protected:
    void showEvent(QShowEvent *ev);
    void closeEvent(QCloseEvent *ev);

private:
	ContactDetails contactDetail;

    QHBoxLayout searchLayout;
	QPushButton btnCreate;
	QPushButton btnRemove;
    QLineEdit   txtSearch;
    QPushButton btnSearch;
	QPushButton btnImport;

	QPushButton	btnClose;
	QCheckBox themeSelector;

    QGridLayout layout;
	//QWidget clientWidget;

    ContactOperations* opsPtr;

	QListWidgetItem* currentItem;

    QString currentContactId;
    bool isEdited;

	RichDataList contactList;
};

#endif
