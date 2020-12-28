cmake_minimum_required(VERSION 2.8)
project(ipc)

message(STATUS "------Build Environment------")
message(STATUS "PLATFORM = $ENV{PLATFORM}")
message(STATUS "IPC_SCHEME = $ENV{IPC_SCHEME}")
message(STATUS "------Build Environment------")

#
# platform and ipc scheme dependencies
#
if("$ENV{IPC_SCHEME}" STREQUAL "rpc")
    add_definitions(-D _RPC_)
else()
    if("$ENV{PLATFORM}" STREQUAL "raspberry-pi")
    else()
        include_directories(
            $ENV{SYS_ROOT}/usr/lib/x86_64-linux-gnu/dbus-1.0/include
            $ENV{SYS_ROOT}/usr/include/dbus-1.0)
    endif()

    add_definitions(-D _DBUS_)
    include_directories(inc/dbus)
    aux_source_directory(src/dbus SRCS)
    set(LINK dbus-1)
endif()

#
# ipclib
#
add_compile_options(-Wall -O0 -g -std=c++14 -rdynamic)
add_definitions(-D _DEBUG)
include_directories(inc)
aux_source_directory(src SRCS)
add_library(${PROJECT_NAME} STATIC ${SRCS})
target_link_libraries(${PROJECT_NAME} pthread ${LINK})

# example
add_subdirectory(example)

