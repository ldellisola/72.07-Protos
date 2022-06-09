cmake_minimum_required(VERSION 3.16)
project(selector C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        selector.c
)

add_library(selector STATIC ${SOURCES})
target_link_libraries(selector PUBLIC utils)