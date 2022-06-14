cmake_minimum_required(VERSION 3.16)
project(lulu C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        lulu.c
)

add_library(lulu STATIC ${SOURCES})
target_link_libraries(lulu PUBLIC utils tcp parsers)