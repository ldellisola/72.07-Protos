cmake_minimum_required(VERSION 3.16)
project(socks5 C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        socks5_connection.c
        socks5_server.c
        socks5_messages.c
        fsm_handlers/socks5_hello.c
        fsm_handlers/socks5_auth.c
        fsm_handlers/socks5_request.c
        fsm_handlers/socks5_establish_connection.c
        fsm_handlers/socks5_connected.c
        fsm_handlers/socks5_client.c
        fsm_handlers/socks5_remote.c
        fsm_handlers/socks5_dns.c
        socks5_users.c
)

add_library(socks5 STATIC ${SOURCES})
target_link_libraries(socks5 PUBLIC parsers tcp utils selector fsm)