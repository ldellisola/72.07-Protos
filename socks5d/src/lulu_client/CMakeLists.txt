cmake_minimum_required(VERSION 3.16)
project(lulu_client C)
set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        main.c
)

add_executable(lulu_client ${SOURCES})

target_link_libraries(lulu_client PUBLIC parsers tcp utils)