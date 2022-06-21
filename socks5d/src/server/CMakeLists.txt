cmake_minimum_required(VERSION 3.16)
project(server C)
set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        main.c
        cli.c
)

add_executable(socks5d ${SOURCES})
add_library(server_lib ${SOURCES})

target_link_libraries(socks5d PUBLIC socks5 tcp utils selector lulu)