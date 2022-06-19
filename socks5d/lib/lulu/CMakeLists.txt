cmake_minimum_required(VERSION 3.16)
project(lulu C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        lulu_server.c
        lulu_connection.c
        lulu_users.c fsm_handlers/lulu_auth.c ../../include/lib/lulu/fsm_handlers/lulu_auth.h)

add_library(lulu STATIC ${SOURCES})
target_link_libraries(lulu PUBLIC utils tcp parsers)