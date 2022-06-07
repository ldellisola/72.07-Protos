cmake_minimum_required(VERSION 3.16)
project(tcp_test C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        tcp_test.c
)

add_library(tcp_test STATIC ${SOURCES})
target_link_libraries(tcp_test PUBLIC tcp)