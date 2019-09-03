#include "MailClientCore.h"

int main(int argc, char *argv[])
{
	MailClientCore app(argc, argv);

	if (!app.Initialize())
	{
		return -1;
	}

	app.StartPollerLoop();
    app.StartMessageLoop();

    return 0;
}