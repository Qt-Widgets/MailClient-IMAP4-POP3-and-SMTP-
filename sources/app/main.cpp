
#include "MailClient.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
	MailClient* app = new MailClient(argc, argv);

	setAppThemeLight(app);
	Q_INIT_RESOURCE(Resources);

	app->ShowSpalsh();
	app->processEvents();

	app->ShowSplashMessage("Opening storage");
	if (!app->InitializeDB())
	{
		QMessageBox msg;
		msg.setText("MailClient could not open storage");
		msg.exec();
		return -1;
	}
	
	app->FetchConfiguration();
	//app->FetchProfiles();
	//app->FetchDirectories();
	app->ShowSplashMessage("Initializing main window");

	if(!app->InitializeUI())
	{
		QMessageBox msg;
		msg.setText("The application has failed to initialize successfully");
		msg.exec();
		return -1;
	}

	if (!app->InitializeNetwork())
	{
		QMessageBox msg;
		msg.setText("The application has failed to initialize network resources");
		msg.exec();
	}

	app->ShowUI();
	return app->exec();
}
