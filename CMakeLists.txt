cmake_minimum_required(VERSION 2.8)

project(ipclib)

message(STATUS "Build environment:")
message(STATUS "PLATFORM = $ENV{PLATFORM}")
message(STATUS "IPC_SCHEME = $ENV{IPC_SCHEME}")

add_compile_options(-Wall -O0 -g -std=c++14 -rdynamic)
add_definitions(-D _DEBUG)

# platform lib directory
if("$ENV{PLATFORM}" STREQUAL "0")
    set(DBUS_INCS $ENV{SYS_ROOT}/usr/lib/x86_64-linux-gnu/dbus-1.0/include)
elseif("$ENV{IPC_SCHEME}" STREQUAL "1")
else()
    set(DBUS_INCS $ENV{SYS_ROOT}/usr/lib/x86_64-linux-gnu/dbus-1.0/include)
endif()

# ipc lib
if("$ENV{IPC_SCHEME}" STREQUAL "0")
    add_definitions(-D _IPC_DBUS_)
    include_directories(
        ${DBUS_INCS}
        $ENV{SYS_ROOT}/usr/include/dbus-1.0)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/dbus DIR_SRCS)
    set(LINK dbus-1)
elseif("$ENV{IPC_SCHEME}" STREQUAL "1")
    add_definitions(-D _IPC_SOCKET_)
else()
    add_definitions(-D _IPC_DBUS_)
    include_directories(
        ${DBUS_INCS}
        $ENV{SYS_ROOT}/usr/include/dbus-1.0)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/dbus DIR_SRCS)
    set(LINK dbus-1)
endif()

include_directories(inc)
aux_source_directory(src DIR_SRCS)
add_library(ipc STATIC ${DIR_SRCS})
target_link_libraries(ipc pthread ${LINK})

# demo
add_subdirectory(test test)

