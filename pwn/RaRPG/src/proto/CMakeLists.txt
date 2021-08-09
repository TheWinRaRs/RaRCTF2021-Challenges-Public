if (WIN32)
INCLUDE_DIRECTORIES(C:\\vcpkg\\vcpkg\\packages\\protobuf_x86-windows\\include)
add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/game.pb.cc ${CMAKE_SOURCE_DIR}/game.pb.h
        COMMAND C:\\vcpkg\\vcpkg\\packages\\protobuf_x86-windows\\tools\\protobuf\\protoc.exe game.proto --cpp_out=..
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/proto
        DEPENDS game.proto
)
elseif (UNIX)
add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/game.pb.cc ${CMAKE_SOURCE_DIR}/game.pb.h
        COMMAND protoc game.proto --cpp_out=..
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/proto
        DEPENDS game.proto
)
endif (WIN32)

ADD_LIBRARY(proto STATIC ${CMAKE_SOURCE_DIR}/game.pb.cc)
