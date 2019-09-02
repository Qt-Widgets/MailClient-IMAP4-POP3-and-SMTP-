#ifndef _SETTINGS_VIEW
#define _SETTINGS_VIEW

#include <QtCore>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QVBoxLayout>

class SettingsView : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsView(QWidget *parent = 0);
    void LoadConfiguration();

signals:
	void SwitchToDarkTheme();
	void SwitchToLightTheme();

private slots:
    void eventSave();
	void eventSwitchTheme();

private:
    QGridLayout layout;
    QCheckBox   chkAutoMark;
    QLabel		lblMailInRefreshInterval;
    QSpinBox	txtRefreshInterval;
    QCheckBox   chkSaveOutgoing;
	QCheckBox   chkThemeSelector;
    QPushButton btnSave;
};

#endif // SETTINGSDIALOG_H
