cmake_minimum_required(VERSION 2.8)
project(SocketApps)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wall -Wextra -pedantic-errors -Werror=return-type")
include_directories("${CMAKE_SOURCE_DIR}/common")

if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX /usr/local)
endif()

set(server_SOURCES
    server/main.cpp
    server/listeners.cpp
    server/echoserver.cpp
    common/globals.h
    common/utils.h
)

set(client_SOURCES
    client/main.cpp
    client/echoclient.cpp
    common/globals.h
    common/utils.h
)

add_executable(echoServer ${server_SOURCES})
install(TARGETS echoServer DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/")

add_executable(echoClient ${client_SOURCES})
install(TARGETS echoClient DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/")
