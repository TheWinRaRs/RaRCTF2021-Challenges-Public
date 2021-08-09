cmake_minimum_required(VERSION 3.16)
project(ConsoleGame)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

ADD_SUBDIRECTORY(proto)

include_directories(include proto)
include_directories(C:\\vcpkg\\vcpkg\\packages\\protobuf_x86-windows\\include)
link_directories(C:\\vcpkg\\vcpkg\\packages\\protobuf_x86-windows\\lib)
link_directories(C:\\vcpkg\\vcpkg\\packages\\protobuf_x86-windows\\debug\\lib)

if (WIN32)
    find_library(CURSES pdcurses lib)
    find_library(WINMM winmm)
    find_library(WS2_32 ws2_32)
elseif (UNIX)
    find_library(CURSES curses)
    set(CMAKE_C_COMPILER "/usr/bin/clang")
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
endif (WIN32)

find_library(ENET enet lib)

add_executable(client client.cpp ENetHost.cpp)
add_executable(server server.cpp ENetHost.cpp)


if (WIN32)
    target_link_libraries(client proto ${CURSES} ${ENET} ${WINMM} ${WS2_32})
    target_link_libraries(server proto ${ENET} ${WINMM} ${WS2_32})
    target_link_libraries(client debug libprotobufd)
    target_link_libraries(server debug libprotobufd)
    target_link_libraries(client optimized ${PROTOBUF})
    target_link_libraries(server optimized  ${PROTOBUF})
elseif (UNIX)
    Include(FindProtobuf)
    target_link_libraries(client proto ${CURSES} ${ENET})
    target_link_libraries(server proto ${ENET})
    target_link_libraries(client ${Protobuf_LIBRARIES})
    target_link_libraries(server ${Protobuf_LIBRARIES})
endif (WIN32)

