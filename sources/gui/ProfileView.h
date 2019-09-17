#ifndef _PROFILES_WINDOWS
#define _PROFILES_WINDOWS

#include <QtWidgets>
#include <QFrame>
#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QTreeWidgetItem>
#include <QPixmap>
#include <QMessageBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QComboBox>
#include <QToolBar>
#include "../data/Profile.h"
#include "ThemeHandler.h"

#include <string>
#include <vector>

using namespace std;

class ProfileView : public QWidget
{
    Q_OBJECT
public:
    ProfileView(QWidget* parent = nullptr);
    void Initialize();

signals:
	void ProfileAdded(std::string &profilename, std::string& emailId);
	void ProfileUpdated(std::string &profilename, std::string& emailId);
	void ProfileRemoved(std::string &profilename, std::string& emailId);

private slots:
    void eventProfileSelected(QTreeWidgetItem *item, int column);
    void eventResetFields();
    void eventRemoveEntry();
    void eventSaveEntry();

private:
	void showEvent(QShowEvent* event);

    QGridLayout		widgetLayout;
    QTreeWidget		lstViewProfiles;

    QLabel			lblMailInGrp;
    QLabel			lblMailInServ;
    QLabel			lblMailInPort;
    QLabel			lblMailInSecr;

    QLineEdit		txtMailInServ;
    QLineEdit		txtMailInPort;
    QComboBox		cmbMailInSecr;

    QLabel			lblMailOutGrp;
    QLabel			lblMailOutServ;
    QLabel			lblMailOutPort;
    QLabel			lblMailOutSecr;

    QLineEdit		txtMailOutServ;
    QLineEdit		txtMailOutPort;
    QComboBox		cmbMailOutSecr;

    QLabel			lblName;
    QLineEdit		txtName;
    QLabel			lblEMail;
    QLineEdit		txtEMail;
    QLabel			lblPassword;
    QLineEdit		txtPassword;
    QLabel			lblError;

	QPushButton		btnNew;
	QPushButton		btnRem;
	QPushButton		btnSave;    
	
	Profile			currentRecord;
    QTreeWidgetItem* currentItem;
	   
	bool isEditMode;
};

#endif
