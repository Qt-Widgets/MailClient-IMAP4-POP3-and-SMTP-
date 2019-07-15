#include "MailClient.h"
#include "./utils/Directory.h"

int main(int argc, char *argv[])
{
    MailClient* app = new MailClient(argc, argv);

	if (!app->Initialize())
	{
		return -1;
	}

    //app->Start();
	app->Test();

    return 0;
}