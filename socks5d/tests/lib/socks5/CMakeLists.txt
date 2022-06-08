cmake_minimum_required(VERSION 3.16)
project(socks5_test C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        socks5_connection_test.c
        socks5_messages_test.c socks5_messages_test.h)

add_library(socks5_test STATIC ${SOURCES})
target_link_libraries(socks5_test PUBLIC socks5)