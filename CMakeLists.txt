cmake_minimum_required(VERSION 3.1.0)

project(MailClient)

if(WIN32)
message("WARNING: On Windows, please set the Qt5_DIR to the Qt5 CMake configuration path")
set(Qt5_DIR C:/Qt/5.13.0/msvc2017_64/lib/cmake/Qt5)
endif()

if(WIN32)
message("### WARNING: For Windows builds please adjust the location of libssl in next couple of lines")
include_directories(./sources/ ../../utils ../../3rdparty C:/oss/usr/include/ C:/oss/usr/include/openssl/)
link_directories(C:/oss/usr/lib)
else()
include_directories(./sources/ ./include/ ../../utils ../../3rdparty /include/ /usr/include/ /usr/local/include/ /usr/include/openssl/)
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
add_executable(MailClient WIN32 ${project_sources} ${project_headers} ${project_resources})
else()
add_executable(MailClient ${project_sources} ${project_headers} ${project_resources})
link_libraries(rt pthread dl)
endif()

target_link_libraries(MailClient Qt5::Widgets Qt5::Core)