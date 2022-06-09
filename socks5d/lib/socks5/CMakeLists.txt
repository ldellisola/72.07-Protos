cmake_minimum_required(VERSION 3.16)
project(socks5 C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        socks5_connection.c
        socks5_server.c
        socks5_messages.c
        socks5_hello.c)

add_library(socks5 STATIC ${SOURCES})
target_link_libraries(socks5 PUBLIC parsers tcp utils selector fsm)