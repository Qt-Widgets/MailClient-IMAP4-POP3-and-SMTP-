cmake_minimum_required(VERSION 3.0)

project(MailClientCore)

set(MailClientCore_VERSION_MAJOR 1)
set(MailClientCore_VERSION_MINOR 0)

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

set(SOURCE
   ${SOURCE}
   ${PROJECT_SOURCE_DIR}../../utils/CommQueue.cpp
   ${PROJECT_SOURCE_DIR}../../utils/Base64.cpp
   ${PROJECT_SOURCE_DIR}../../utils/Directory.cpp
   ${PROJECT_SOURCE_DIR}../../utils/StringEx.cpp
   ${PROJECT_SOURCE_DIR}../../utils/Logger.cpp
   ${PROJECT_SOURCE_DIR}../../utils/DateTime.cpp
   ${PROJECT_SOURCE_DIR}../../utils/Configuration.cpp
   ${PROJECT_SOURCE_DIR}../../3rdparty/sqlite3.c
   ${PROJECT_SOURCE_DIR}../../data/MailDatabase.cpp
   ${PROJECT_SOURCE_DIR}../../data/ContactDatabase.cpp
   ${PROJECT_SOURCE_DIR}../../data/MailStorage.cpp
   ${PROJECT_SOURCE_DIR}../../network/Mail.cpp
   ${PROJECT_SOURCE_DIR}../../network/ImapClient.cpp
   ${PROJECT_SOURCE_DIR}../../network/SmtpClient.cpp
   ${PROJECT_SOURCE_DIR}../../network/TcpClient.cpp
   ${PROJECT_SOURCE_DIR}../../network/Network.cpp
   ${PROJECT_SOURCE_DIR}/app/MailClientCore.cpp
   ${PROJECT_SOURCE_DIR}/app/main.cpp
)

set(HEADERS
   ${HEADERS}
   ${PROJECT_SOURCE_DIR}../../utils/CommQueue.h   
   ${PROJECT_SOURCE_DIR}../../utils/Base64.h
   ${PROJECT_SOURCE_DIR}../../utils/Directory.h
   ${PROJECT_SOURCE_DIR}../../utils/StringEx.h
   ${PROJECT_SOURCE_DIR}../../utils/Logger.h
   ${PROJECT_SOURCE_DIR}../../utils/DateTime.h
   ${PROJECT_SOURCE_DIR}../../utils/Configuration.h
   ${PROJECT_SOURCE_DIR}../../3rdparty/sqlite3.h
   ${PROJECT_SOURCE_DIR}../../3rdparty/sqlite3ext.h   
   ${PROJECT_SOURCE_DIR}../../data/MailDatabase.h
   ${PROJECT_SOURCE_DIR}../../data/ContactDatabase.h
   ${PROJECT_SOURCE_DIR}../../data/Contact.h
   ${PROJECT_SOURCE_DIR}../../data/Profile.h
   ${PROJECT_SOURCE_DIR}../../data/MailStorage.h
   ${PROJECT_SOURCE_DIR}../../network/SecurityType.h
   ${PROJECT_SOURCE_DIR}../../network/Mail.h
   ${PROJECT_SOURCE_DIR}../../network/ImapClient.h
   ${PROJECT_SOURCE_DIR}../../network/SmtpClient.h
   ${PROJECT_SOURCE_DIR}../../network/TcpClient.h
   ${PROJECT_SOURCE_DIR}../../network/Network.h
   ${PROJECT_SOURCE_DIR}/app/MailClientCore.h
)

add_executable(MailClientCore ${SOURCE})

