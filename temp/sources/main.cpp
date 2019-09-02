#include "AddressBook.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    AddressBook* app = new AddressBook(argc, argv);

    Q_INIT_RESOURCE(Resources);

    if(!app->SetupCommQueue())
    {
        return -1;
    }

    app->ShowWindow();

    return app->exec();
}
