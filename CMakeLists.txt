cmake_minimum_required(VERSION 3.1.0)

project(MailClient)

if(WIN32)
message("WARNING: On Windows, please set the Qt5_DIR to the Qt5 CMake configuration path")
set(Qt5_DIR C:/Qt/5.13.0/msvc2017_64/lib/cmake/Qt5)
endif()

if(WIN32)
message("### WARNING: For Windows builds please adjust the location of libssl in next couple of lines")
include_directories(./sources/utils ./sources/app ./sources/data ./sources/gui ./sources/network ./sources/resources ./sources/3rdparty C:/oss/usr/include/ C:/oss/usr/include/openssl/)
link_directories(C:/oss/usr/lib)
else()
include_directories(./sources/utils ./sources/app ./sources/data ./sources/gui ./sources/network ./sources/resources ./sources/3rdparty /include/ /usr/include/ /usr/local/include/ /usr/include/openssl/)
link_directories(./lib/ /lib /usr/lib/ /usr/local/lib/ /usr/lib/x86_64-linux-gnu/ /usr/lib/arm-linux-gnu/)
endif()

if(WIN32)
link_libraries(ws2_32 libssl libcrypto)
else()
link_libraries(dl rt pthread ssl crypto)
endif()

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(project_sources 
./sources/network/Network.cpp 
./sources/network/Mail.cpp 
./sources/network/ImapClient.cpp 
./sources/network/SmtpClient.cpp 
./sources/network/TcpClient.cpp 
./sources/utils/Base64.cpp 
./sources/utils/Configuration.cpp 
./sources/utils/DateTime.cpp 
./sources/utils/Directory.cpp 
./sources/utils/Logger.cpp 
./sources/utils/SignalHandler.cpp 
./sources/utils/StringEx.cpp 
./sources/data/MailStorage.cpp
./sources/data/ContactDatabase.cpp
./sources/data/MailDatabase.cpp
./sources/3rdparty/sqlite3.c
./sources/gui/ThemeHandler.cpp 
./sources/gui/VerticalLine.cpp 
./sources/gui/HorizontalLine.cpp 
./sources/gui/RichDataList.cpp 
./sources/gui/PictureBox.cpp 
./sources/gui/ActionBuilder.cpp 
./sources/gui/Calender.cpp 
./sources/gui/DirectoryView.cpp 
./sources/gui/About.cpp 
./sources/gui/MailBoxView.cpp 
./sources/gui/MailComposer.cpp 
./sources/gui/MailView.cpp 
./sources/gui/MainWindow.cpp 
./sources/gui/ProfileView.cpp 
./sources/gui/SettingsView.cpp 
./sources/gui/ContactsView.cpp 
./sources/gui/ContactDetails.cpp 
./sources/gui/AddressBookWindow.cpp 
./sources/gui/Tasks.cpp 
./sources/app/MailClient.cpp 
./sources/app/main.cpp 
)

set(project_headers 
./sources/network/Network.h 
./sources/network/SecurityType.h 
./sources/network/Mail.h 
./sources/network/ImapClient.h 
./sources/network/SmtpClient.h 
./sources/network/TcpClient.h 
./sources/utils/Base64.h 
./sources/utils/Configuration.h 
./sources/utils/DateTime.h 
./sources/utils/Directory.h 
./sources/utils/Logger.h 
./sources/utils/SignalHandler.h 
./sources/utils/StringEx.h 
./sources/data/MailStorage.h
./sources/data/ContactDatabase.h
./sources/data/MailDatabase.h
./sources/data/Profile.h
./sources/data/Contact.h
./sources/3rdparty/sqlite3.h
./sources/3rdparty/sqlite3ext.h
./sources/gui/ThemeHandler.h 
./sources/gui/VerticalLine.h 
./sources/gui/HorizontalLine.h 
./sources/gui/RichDataList.h 
./sources/gui/PictureBox.h 
./sources/gui/ActionBuilder.h 
./sources/gui/Calender.h 
./sources/gui/DirectoryView.h 
./sources/gui/About.h 
./sources/gui/MailBoxView.h 
./sources/gui/MailComposer.h 
./sources/gui/MailView.h 
./sources/gui/MainWindow.h 
./sources/gui/ProfileView.h 
./sources/gui/SettingsView.h 
./sources/gui/ContactsView.h 
./sources/gui/ContactDetails.h 
./sources/gui/AddressBookWindow.h 
./sources/gui/Tasks.h 
./sources/app/MailClient.h 
)

set(project_resources 
./sources/resources/Resources.qrc
)

if(WIN32)
add_executable(MailClient WIN32 ${project_sources} ${project_headers} ${project_resources})
else()
add_executable(MailClient ${project_sources} ${project_headers} ${project_resources})
link_libraries(rt pthread dl)
endif()

target_link_libraries(MailClient Qt5::Widgets Qt5::Core)