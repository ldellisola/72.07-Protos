cmake_minimum_required(VERSION 3.16)
project(socks5 C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        socks5_connection.c
        socks5_server.c
        socks5_messages.c
        socks5_hello.c
        socks5_auth.c
        socks5_request.c
        socks5_establish_connection.c
        socks5_connected.c
        socks5_client.c
        socks5_remote.c
)

add_library(socks5 STATIC ${SOURCES})
target_link_libraries(socks5 PUBLIC parsers tcp utils selector fsm)