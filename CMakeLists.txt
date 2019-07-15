cmake_minimum_required(VERSION 3.0)

project(MailClient)

set(MailClient_VERSION_MAJOR 1)
set(MailClient_VERSION_MINOR 0)

if(WIN32)
include_directories(./sources/ D:/oss/usr/include/ D:/oss/usr/include/openssl/)
link_directories(D:/oss/usr/lib)
else()
include_directories(./sources/ ./include/ /include/ /usr/include/ /usr/local/include/ /usr/include/openssl/)
link_directories(./lib/ /lib /usr/lib/ /usr/local/lib/ /usr/lib/x86_64-linux-gnu/ /usr/lib/arm-linux-gnu/)
endif()

if(WIN32)
link_libraries(ws2_32 libssl libcrypto libsasl2)
else()
link_libraries(rt pthread dl ssl crypto)
endif()

set(PROJECT_SOURCE_DIR ./sources/)

set(SOURCE
   ${SOURCE}
   ${PROJECT_SOURCE_DIR}/utils/Base64.cpp
   ${PROJECT_SOURCE_DIR}/utils/Directory.cpp
   ${PROJECT_SOURCE_DIR}/utils/StringEx.cpp
   ${PROJECT_SOURCE_DIR}/utils/Logger.cpp
   ${PROJECT_SOURCE_DIR}/utils/Timestamp.cpp
   ${PROJECT_SOURCE_DIR}/app/MailClient.cpp
   ${PROJECT_SOURCE_DIR}/network/ImapClient.cpp
   ${PROJECT_SOURCE_DIR}/network/SmtpClient.cpp
   ${PROJECT_SOURCE_DIR}/network/TcpClient.cpp
   ${PROJECT_SOURCE_DIR}/network/Network.cpp
   ${PROJECT_SOURCE_DIR}/network/Mail.cpp
   ${PROJECT_SOURCE_DIR}/app/main.cpp
)

set(HEADERS
   ${HEADERS}
   ${PROJECT_SOURCE_DIR}/utils/Base64.h
   ${PROJECT_SOURCE_DIR}/utils/Directory.h
   ${PROJECT_SOURCE_DIR}/utils/StringEx.h
   ${PROJECT_SOURCE_DIR}/utils/Logger.h
   ${PROJECT_SOURCE_DIR}/utils/Timestamp.h 
   ${PROJECT_SOURCE_DIR}/app/MailClient.h
   ${PROJECT_SOURCE_DIR}/network/ImapClient.h
   ${PROJECT_SOURCE_DIR}/network/SmtpClient.h
   ${PROJECT_SOURCE_DIR}/network/TcpClient.h
   ${PROJECT_SOURCE_DIR}/network/Mail.h
   ${PROJECT_SOURCE_DIR}/network/SecurityType.h
)

add_executable(MailClient ${SOURCE})

