cmake_minimum_required(VERSION 3.1.0)

project(MailClientUI)

if(WIN32)
message("WARNING: On Windows, please set the Qt5_DIR to the Qt5 CMake configuration path")
set(Qt5_DIR C:/Qt/5.13.0/msvc2017_64/lib/cmake/Qt5)
endif()

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(project_sources 
../utils/CommQueue.cpp 
../utils/StringEx.cpp 
../utils/Base64.cpp 
../utils/DateTime.cpp 
../utils/Directory.cpp 
../data/MailStorage.cpp
../network/Mail.cpp 
gui/ThemeHandler.cpp 
gui/VerticalLine.cpp 
gui/HorizontalLine.cpp 
gui/RichDataList.cpp 
gui/PictureBox.cpp 
gui/ActionBuilder.cpp 
gui/Calender.cpp 
gui/DirectoryView.cpp 
gui/About.cpp 
gui/MailBoxView.cpp 
gui/MailComposer.cpp 
gui/MailView.cpp 
gui/MainWindow.cpp 
gui/ProfileView.cpp 
gui/SettingsView.cpp 
gui/ContactsView.cpp 
gui/ContactDetails.cpp 
gui/AddressBookWindow.cpp 
gui/Tasks.cpp 
app/main.cpp 
app/MailClientUI.cpp 
)

set(project_headers 
../utils/CommQueue.h 
../utils/StringEx.h 
../utils/Base64.h 
../utils/DateTime.h 
../utils/Directory.h 
../data/MailStorage.h 
../data/Profile.h 
../network/SecurityType.h
../network/Mail.h 
gui/ThemeHandler.h 
gui/VerticalLine.h 
gui/HorizontalLine.h 
gui/RichDataList.h 
gui/PictureBox.h 
gui/ActionBuilder.h 
gui/Calender.h 
gui/DirectoryView.h 
gui/About.h 
gui/MailBoxView.h 
gui/MailComposer.h 
gui/MailView.h 
gui/MainWindow.h 
gui/ProfileView.h 
gui/SettingsView.h 
gui/ContactsView.h 
gui/ContactDetails.h 
gui/AddressBookWindow.h 
gui/Tasks.h 
app/MailClientOperations.h 
app/ContactOperations.h
app/MailClientUI.h
)

set(project_resources 
resources/Resources.qrc
)

if(WIN32)
add_executable(MailClientUI WIN32 ${project_sources} ${project_headers} ${project_resources})
else()
add_executable(MailClientUI ${project_sources} ${project_headers} ${project_resources})
link_libraries(rt pthread dl)
endif()

target_link_libraries(MailClientUI Qt5::Widgets Qt5::Core)