cmake_minimum_required(VERSION 2.8)
project(example)

#
# Example lib
#
include_directories(inc)
aux_source_directory(src EXAMPLE_SRCS)
add_library(example STATIC ${EXAMPLE_SRCS})
target_link_libraries(example ipc)

#
# Example server
#
add_executable(example_server example_server.cc)
target_link_libraries(example_server example)
#set_target_properties(example_server PROPERTIES OUTPUT_NAME "example_server")

#
# example client
#
add_executable(example_client example_client.cc)
target_link_libraries(example_client example)

