cmake_minimum_required(VERSION 3.16)
project(tcp C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        tcp.c
)

add_library(tcp STATIC ${SOURCES})
target_link_libraries(tcp PUBLIC utils)