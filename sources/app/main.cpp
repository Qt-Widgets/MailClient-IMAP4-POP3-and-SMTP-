#include "RushPriority.h"

int main(int argc, char *argv[])
{
	RushPriority app(argc, argv);

	if (!app.Initialize())
	{
		return -1;
	}

	app.StartPollerLoop();
    app.StartMessageLoop();

    return 0;
}