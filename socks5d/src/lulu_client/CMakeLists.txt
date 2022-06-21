cmake_minimum_required(VERSION 3.16)
project(lulu_client C)
set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        main.c
        cli.c
        client_data.c
        fsm/client_auth.c
        fsm/client_wait_connection.c
        fsm/client_transaction.c
        fsm/client_timeout.c
        fsm/client_buffersize.c
        fsm/client_metrics.c
        fsm/client_goodbye.c)

add_executable(client ${SOURCES} fsm/client_users.c)
add_library(lulu_client_lib ${SOURCES})

target_link_libraries(client PUBLIC parsers tcp utils fsm)