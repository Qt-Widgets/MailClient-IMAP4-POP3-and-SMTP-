cmake_minimum_required(VERSION 3.0)

project(MailClient)

set(MailClient_VERSION_MAJOR 1)
set(MailClient_VERSION_MINOR 0)

if(WIN32)
message("WARNING: On Windows, please set the Qt5_DIR to the Qt5 CMake configuration path")
set(Qt5_DIR C:/Qt/5.13.0/msvc2017_64/lib/cmake/Qt5)
endif()

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


if(WIN32)
include_directories(./sources/ c:/oss/usr/include/ c:/oss/usr/include/openssl/)
link_directories(c:/oss/usr/lib)
else()
include_directories(./sources/ ./include/ /include/ /usr/include/ /usr/local/include/ /usr/include/openssl/)
link_directories(./lib/ /lib /usr/lib/ /usr/local/lib/ /usr/lib/x86_64-linux-gnu/ /usr/lib/arm-linux-gnu/)
endif()

if(WIN32)
link_libraries(ws2_32 libssl libcrypto)
else()
link_libraries(rt pthread dl ssl crypto)
endif()

set(PROJECT_SOURCE_DIR ./sources/)

set(SOURCE
   ${SOURCE}
   ${PROJECT_SOURCE_DIR}/3rdparty/sqlite3.c
   ${PROJECT_SOURCE_DIR}/utils/Base64.cpp
   ${PROJECT_SOURCE_DIR}/utils/Configuration.cpp
   ${PROJECT_SOURCE_DIR}/utils/DateTime.cpp
   ${PROJECT_SOURCE_DIR}/utils/Directory.cpp
   ${PROJECT_SOURCE_DIR}/utils/Logger.cpp
   ${PROJECT_SOURCE_DIR}/utils/SignalHandler.cpp
   ${PROJECT_SOURCE_DIR}/utils/StringEx.cpp
   ${PROJECT_SOURCE_DIR}/app/MailClient.cpp
   ${PROJECT_SOURCE_DIR}/network/ImapClient.cpp
   ${PROJECT_SOURCE_DIR}/network/SmtpClient.cpp
   ${PROJECT_SOURCE_DIR}/network/TcpClient.cpp
   ${PROJECT_SOURCE_DIR}/network/Network.cpp
   ${PROJECT_SOURCE_DIR}/data/Mail.cpp
   ${PROJECT_SOURCE_DIR}/data/ContactDatabase.cpp
   ${PROJECT_SOURCE_DIR}/data/MailDatabase.cpp
   ${PROJECT_SOURCE_DIR}/data/MailStorage.cpp
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/ActiveLabel.cpp 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/Button.cpp 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/CentralWidget.cpp 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/DataList.cpp 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/HorizontalLine.cpp 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/IndicatorBar.cpp 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/IndicatorCircular.cpp 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/PictureBox.cpp 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/RichDataList.cpp 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/RichLabel.cpp 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/ThemeHandler.cpp 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/VerticalLine.cpp 
   ${PROJECT_SOURCE_DIR}/gui/ActionBuilder.cpp 
   ${PROJECT_SOURCE_DIR}/gui/Calender.cpp 
   ${PROJECT_SOURCE_DIR}/gui/DirectoryView.cpp 
   ${PROJECT_SOURCE_DIR}/gui/About.cpp 
   ${PROJECT_SOURCE_DIR}/gui/MailBoxView.cpp 
   ${PROJECT_SOURCE_DIR}/gui/MailComposer.cpp 
   ${PROJECT_SOURCE_DIR}/gui/MailView.cpp 
   ${PROJECT_SOURCE_DIR}/gui/MainWindow.cpp 
   ${PROJECT_SOURCE_DIR}/gui/ProfileView.cpp 
   ${PROJECT_SOURCE_DIR}/gui/SettingsView.cpp 
   ${PROJECT_SOURCE_DIR}/gui/ContactsView.cpp 
   ${PROJECT_SOURCE_DIR}/app/MailClient.cpp
   ${PROJECT_SOURCE_DIR}/app/main.cpp
)

set(HEADERS
   ${HEADERS}
   ${PROJECT_SOURCE_DIR}/3rdparty/sqlite3.h
   ${PROJECT_SOURCE_DIR}/3rdparty/sqlite3ext.h
   ${PROJECT_SOURCE_DIR}/utils/Base64.h
   ${PROJECT_SOURCE_DIR}/utils/Configuration.h
   ${PROJECT_SOURCE_DIR}/utils/DateTime.h
   ${PROJECT_SOURCE_DIR}/utils/Directory.h
   ${PROJECT_SOURCE_DIR}/utils/Logger.h
   ${PROJECT_SOURCE_DIR}/utils/SignalHandler.h
   ${PROJECT_SOURCE_DIR}/utils/StringEx.h
   ${PROJECT_SOURCE_DIR}/app/MailClient.h
   ${PROJECT_SOURCE_DIR}/network/ImapClient.h
   ${PROJECT_SOURCE_DIR}/network/SmtpClient.h
   ${PROJECT_SOURCE_DIR}/network/TcpClient.h
   ${PROJECT_SOURCE_DIR}/network/Network.h
   ${PROJECT_SOURCE_DIR}/data/Mail.h
   ${PROJECT_SOURCE_DIR}/data/Contact.h
   ${PROJECT_SOURCE_DIR}/data/Profile.h
   ${PROJECT_SOURCE_DIR}/data/SecurityType.h
   ${PROJECT_SOURCE_DIR}/data/ContactDatabase.h
   ${PROJECT_SOURCE_DIR}/data/MailDatabase.h
   ${PROJECT_SOURCE_DIR}/data/MailStorage.h
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/ActiveLabel.h 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/Button.h 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/CentralWidget.h 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/DataList.h 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/HorizontalLine.h 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/IndicatorBar.h 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/IndicatorCircular.h 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/PictureBox.h 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/RichDataList.h 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/RichLabel.h 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/ThemeHandler.h 
   ${PROJECT_SOURCE_DIR}/qtextrawidgets/VerticalLine.h 
   ${PROJECT_SOURCE_DIR}/gui/ActionBuilder.h 
   ${PROJECT_SOURCE_DIR}/gui/Calender.h 
   ${PROJECT_SOURCE_DIR}/gui/DirectoryView.h 
   ${PROJECT_SOURCE_DIR}/gui/About.h 
   ${PROJECT_SOURCE_DIR}/gui/MailBoxView.h 
   ${PROJECT_SOURCE_DIR}/gui/MailComposer.h 
   ${PROJECT_SOURCE_DIR}/gui/MailView.h 
   ${PROJECT_SOURCE_DIR}/gui/MainWindow.h 
   ${PROJECT_SOURCE_DIR}/gui/ProfileView.h 
   ${PROJECT_SOURCE_DIR}/gui/SettingsView.h 
   ${PROJECT_SOURCE_DIR}/gui/ContactsView.h 
   ${PROJECT_SOURCE_DIR}/gui/MailClientOperations.h 
   ${PROJECT_SOURCE_DIR}/app/MailClient.h
)

set(project_resources 
${PROJECT_SOURCE_DIR}/resources/Resources.qrc
)

if(WIN32)
add_executable(MailClient WIN32 ${project_sources} ${project_headers} ${project_resources})
else()
add_executable(MailClient ${project_sources} ${project_headers} ${project_resources})
link_libraries(rt pthread dl)
endif()

target_link_libraries(MailClient Qt5::Widgets Qt5::Core Qt5::Gui)
