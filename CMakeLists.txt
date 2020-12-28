cmake_minimum_required(VERSION 2.8)
project(ipc)

message(STATUS "------Build Environment------")
message(STATUS "PLATFORM = $ENV{PLATFORM}")
message(STATUS "IPC_SCHEME = $ENV{IPC_SCHEME}")
message(STATUS "------Build Environment------")

#
# platform and ipc scheme dependencies
#
if("$ENV{IPC_SCHEME}" STREQUAL "rpclib")
    add_definitions(-D _RPCLIB_)
else()
    if("$ENV{PLATFORM}" STREQUAL "raspberry-pi")
    else()
        include_directories($ENV{SYS_ROOT}/usr/lib/x86_64-linux-gnu/dbus-1.0/include
                            $ENV{SYS_ROOT}/usr/include/dbus-1.0)
    endif()
    
    add_definitions(-D _DBUS_)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/dbus DIR_SRCS)
    set(LINK dbus-1)
endif()

#
# ipclib
#
add_compile_options(-Wall -O0 -g -std=c++14 -rdynamic)
add_definitions(-D _DEBUG)
include_directories(inc)
aux_source_directory(src DIR_SRCS)
add_library(${PROJECT_NAME} STATIC ${DIR_SRCS})
target_link_libraries(${PROJECT_NAME} pthread ${LINK})

# demo
add_subdirectory(test)

