cmake_minimum_required(VERSION 2.8)

project(test)

# myservice lib
include_directories(
    ${CMAKE_SOURCE_DIR}/inc
    lib/inc)
aux_source_directory(lib/src DIR_SRCS)
add_library(myservice STATIC ${DIR_SRCS})
target_link_libraries(myservice ipc)

# myservice application
add_executable(service main_service.cc)
target_link_libraries(service myservice)
set_target_properties(service PROPERTIES OUTPUT_NAME "myservice")

# myclient application
add_executable(myclient main_client.cc)
target_link_libraries(myclient myservice)

