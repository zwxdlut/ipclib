cmake_minimum_required(VERSION 2.8)
project(ipc)

#
# platform
#
message(STATUS "PLATFORM = $ENV{PLATFORM}")

if("$ENV{PLATFORM}" STREQUAL "raspberry-pi")
else()
    include_directories(
        $ENV{SYS_ROOT}/usr/lib/x86_64-linux-gnu/dbus-1.0/include
        $ENV{SYS_ROOT}/usr/include/dbus-1.0)
endif()

#
# ipc lib
#
add_compile_options(-Wall -O0 -g -std=c++14 -rdynamic)
add_definitions(-D _DEBUG)
include_directories(inc)
aux_source_directory(src SRCS)
add_library(${PROJECT_NAME} STATIC ${SRCS})
target_link_libraries(${PROJECT_NAME} pthread dbus-1)

#
# example
#
add_subdirectory(example)

