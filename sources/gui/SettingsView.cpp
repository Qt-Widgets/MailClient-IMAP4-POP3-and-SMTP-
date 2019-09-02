#include "SettingsView.h"
#include "../../uicommon/ThemeHandler.h"

SettingsView::SettingsView(QWidget *parent) :
    QWidget(parent)
{
    setMaximumHeight(200);
    setMaximumWidth(400);

    chkAutoMark.setText("Automatically mark messages as read");
    lblMailInRefreshInterval.setText("Refresh interval");
    chkSaveOutgoing.setText("Save outgoing email");
	chkThemeSelector.setText("Dark Theme");

    btnSave.setText("Save");
    btnSave.setMaximumWidth(75);
    btnSave.setMinimumWidth(75);

    layout.addWidget(&lblMailInRefreshInterval, 0, 0);
    layout.addWidget(&txtRefreshInterval, 0, 1, Qt::AlignRight);   
	layout.addWidget(&chkAutoMark, 1, 0);
    layout.addWidget(&chkSaveOutgoing, 2, 0);
	layout.addWidget(&chkThemeSelector, 3, 0);
    layout.addWidget(&btnSave, 4, 1, Qt::AlignRight);

    setLayout(&layout);

    auto c1 = connect(&btnSave,SIGNAL(clicked()),this,SLOT(eventSave()));
	auto c2 = connect(&chkThemeSelector, &QCheckBox::clicked, this, &SettingsView::eventSwitchTheme);
}

void SettingsView::eventSave()
{
//    appConfiguration.beginGroup("ODBCConfiguration");
//    appConfiguration.setValue("DataSourceName", _txtDataSourceName.text());
//    appConfiguration.setValue("DatabaseUser", _txtDatabaseUsername.text());
//    appConfiguration.setValue("DatabasePassword", _txtDatabasePassword.text());
//    appConfiguration.endGroup();

//    appConfiguration.beginGroup("ProxyConfiguration");
//    appConfiguration.setValue("UseProxy", _chkProxyRequired.isChecked());
//    appConfiguration.setValue("ProxyServer", _txtProxyServer.text());
//    appConfiguration.setValue("ProxyPort", _txtProxyPort.text());
//    appConfiguration.setValue("ProxyUser", _txtProxyUsername.text());
//    appConfiguration.setValue("ProxyPassword", _txtProxyPassword.text());
//    appConfiguration.endGroup();

//    appConfiguration.beginGroup("LocaleSettings");
//    appConfiguration.setValue("LanguageFile", _lblLanguageFileName.text());
//    appConfiguration.endGroup();

//    appConfiguration.beginGroup("GISConfiguration");
//    appConfiguration.setValue("MapProvider", selectedMapProvider());
//    appConfiguration.setValue("UseCache", _chkCacheRequired.isChecked());
//    appConfiguration.setValue("HomeLongitude", _txtLon.text());
//    appConfiguration.setValue("HomeLatitude", _txtLat.text());
//    appConfiguration.endGroup();
    close();
}

void SettingsView::LoadConfiguration()
{
//    appConfiguration.beginGroup("ODBCConfiguration");
//    _txtDataSourceName.setText(appConfiguration.GetValue("DataSourceName").toString());
//    _txtDatabaseUsername.setText(appConfiguration.GetValue("DatabaseUser").toString());
//    _txtDatabasePassword.setText(appConfiguration.GetValue("DatabasePassword").toString());
//    appConfiguration.endGroup();

//    appConfiguration.beginGroup("ProxyConfiguration");
//    if(appConfiguration.GetValue("UseProxy").toBool())
//    {
//        _chkProxyRequired.setCheckState(Qt::Checked);
//    }
//    else
//    {
//        _chkProxyRequired.setCheckState(Qt::Unchecked);
//    }
//    _txtProxyServer.setText(appConfiguration.GetValue("ProxyServer").toString());
//    _txtProxyPort.setText(appConfiguration.GetValue("ProxyPort").toString());
//    _txtProxyUsername.setText(appConfiguration.GetValue("ProxyUser").toString());
//    _txtProxyPassword.setText(appConfiguration.GetValue("ProxyPassword").toString());
//    appConfiguration.endGroup();

//    appConfiguration.beginGroup("LocaleSettings");
//    _lblLanguageFileName.setText(appConfiguration.GetValue("LanguageFile").toString());
//    appConfiguration.endGroup();

//    appConfiguration.beginGroup("GISConfiguration");
//    if(appConfiguration.GetValue("UseCache").toBool())
//    {
//        _chkCacheRequired.setCheckState(Qt::Checked);
//    }
//    else
//    {
//        _chkCacheRequired.setCheckState(Qt::Unchecked);
//    }

//    QString mprov = appConfiguration.GetValue("MapProvider").toString();

//    if(mprov == "OSM")
//    {
//        _cmbMapProvider.setCurrentIndex(0);
//    }

//    if(mprov == "Yahoo")
//    {
//        _cmbMapProvider.setCurrentIndex(1);
//    }

//    if(mprov == "Google")
//    {
//        _cmbMapProvider.setCurrentIndex(2);
//    }
//    _txtLon.setText(appConfiguration.value("HomeLongitude", "").toString().trimmed());
//    _txtLat.setText(appConfiguration.value("HomeLatitude", "").toString().trimmed());
//    appConfiguration.endGroup();
}

void SettingsView::eventSwitchTheme()
{
	if (chkThemeSelector.checkState() == Qt::Checked)
	{
		emit SwitchToDarkTheme();
	}
	else
	{
		emit SwitchToLightTheme();
	}
}
