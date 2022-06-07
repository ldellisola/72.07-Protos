cmake_minimum_required(VERSION 3.16)
project(server_test C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        cli_test.c
)

add_library(server_test STATIC ${SOURCES})
target_link_libraries(server_test PUBLIC server_lib)