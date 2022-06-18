cmake_minimum_required(VERSION 3.16)
project(server C)
set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        main.c
        cli.c
)

add_executable(server ${SOURCES})
add_library(server_lib ${SOURCES})

target_link_libraries(server PUBLIC socks5 tcp utils selector)