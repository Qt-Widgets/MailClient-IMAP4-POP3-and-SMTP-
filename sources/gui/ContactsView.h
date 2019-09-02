#ifndef _CONTACTS_VIEW
#define _CONTACTS_VIEW

#include <QMainWindow>
#include <QWidget>
#include <QToolBar>
#include <QtCore>
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
#include <vector>
#include <string>

class ContactsView : public QDialog
{
    Q_OBJECT
public:
    ContactsView(QWidget* parent = nullptr);
    void LoadContacts();
	void GetSelectedContacts(std::vector<std::string> &clist);

private slots:
	void eventOk();
	void evenCancel();

private:
    QTreeWidget listContactView;

    QPushButton btnOK;
    QPushButton btnCancel;

    QVBoxLayout mainLayout;
	QHBoxLayout btnLayout;
};

#endif
