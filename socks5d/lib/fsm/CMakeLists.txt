cmake_minimum_required(VERSION 3.16)
project(fsm C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        fsm.c
)

add_library(fsm STATIC ${SOURCES})
target_link_libraries(fsm PUBLIC utils)