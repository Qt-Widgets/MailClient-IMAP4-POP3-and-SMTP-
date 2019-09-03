# Rushpriority v2.0 building and installation

## Building

### The following libraries and tools are required for RushPriority v2.0

* Qt 5.x, Core, Gui and Widget modules
* OpenSSL
* SQLite (included in the source tree)
* C++11 or higher
* CMake 3.10 or higher
* MSVC for Visual Studio 2019 for Windows and GCC 8.0 or Clang 8.0 for Linux/Mac/BSD

### The following tool configuration applies to Windows

* The default Qt installation is assumed to be location at C:/Qt, therefore the CMakeLists.txt must be adjusted for GUI projects accordingly.
* The default OpenSSL is assumed to be located at D:/OSS/usr, therefore the applications dependent on OpenSSL must have their CMakeLists.txt adjusted accordingly.

## Project Organization

The project is organized into the following components
* RushPriority (the email client engine, that is assumed to be run as a background service)
* RushPriorityUI - The mail client User Interface that communicates over message queues with the mail client engine
* AddressBook - Standalone contact management module that is assumed to be run as a background service.
* AddressBookUI - The address book User Interface that communicates over message queues with the address book engine

## Deployment
The four (4) executables from the relevent projects i.e. RushPriority, RushPriorityUI, AddressBook and AddressBookUI should be copied to desired installation location.
The following directories are automatically created at the level of the installtion directory.
* logs
* emails
* db

For example if executables are copied and run from RP2/bin, then RP2/logs, RP2/db and RP2/emails will be automatically created.
When "db" is created the required SQLite database is created by the backround applications

The following DLL file(s) and 3rd party executable(s) must be present with the built executables on Windows
* Qt5Core.dll
* Qt5Gui.dll
* Qt5Widget.dll
* libcrypto-1_1-x64.dll or libcrypto-1_1-x86.dll (depending on application build)
* libssl-1_1-x64.dll or libssl-1_1-x86.dll (depending on application build)
* NSSM.exe from https://nssm.cc/ for running the engine (background applications) as Windows service

** The RushPriority.db contains a Mailbox table. This table can be truncated or mails for specific profile-folder combination can be deleted and then it will cause RushPriority core process to download all emails from the IMAP server (all or profile-folder whichever be the case). **

## Application dependencies
* AddressBookUI requires AddressBook to be present
* RushPriorityUI requires RushPriority and AddressBook to be present


```diff
- NOTE: The RushPriority and AddressBook backend services are meant to be started by UIs, if not running. 
- This is being automated. 
- Therefore, at present they may be manually started if not running.
