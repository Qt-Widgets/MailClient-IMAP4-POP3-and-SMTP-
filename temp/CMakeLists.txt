cmake_minimum_required(VERSION 3.1.0)

project(AddressBookUI)

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
../uicommon/PictureBox.cpp 
../uicommon/ThemeHandler.cpp 
../uicommon/HorizontalLine.cpp 
../uicommon/VerticalLine.cpp 
../uicommon/RichDataList.cpp 
sources/AddressBook.cpp 
sources/MainWindow.cpp 
sources/ContactDetails.cpp 
sources/main.cpp
)

set(project_headers 
../utils/CommQueue.h
../utils/StringEx.h
../uicommon/PictureBox.h 
../uicommon/ThemeHandler.h 
../uicommon/HorizontalLine.h 
../uicommon/VerticalLine.h
../uicommon/RichDataList.h 
../data/Contact.h
sources/AddressBook.h 
sources/MainWindow.h 
sources/ContactDetails.h 
sources/ContactOperations.h 
)

set(project_resources 
resources/Resources.qrc
)

if(WIN32)
add_executable(AddressBookUI WIN32 ${project_sources} ${project_headers} ${project_resources})
else()
add_executable(AddressBookUI ${project_sources} ${project_headers} ${project_resources})
link_libraries(rt pthread dl)
endif()

target_link_libraries(AddressBookUI Qt5::Widgets Qt5::Core)