cmake_minimum_required(VERSION 3.16)
project(lulu_client_test C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        lulu_client_test.c
)

add_library(lulu_client_test STATIC ${SOURCES})
target_link_libraries(lulu_client_test PUBLIC lulu_client_lib)