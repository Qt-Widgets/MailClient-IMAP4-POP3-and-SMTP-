#include "MailClient.h"
#include "./utils/Directory.h"
#include "./utils/StringEx.h"

int main(int argc, char *argv[])
{
    MailClient* app = new MailClient(argc, argv);

	if (!app->Initialize())
	{
		return -1;
	}

    //app->Start();
	app->TestIn();

    return 0;
}