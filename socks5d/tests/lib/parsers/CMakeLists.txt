cmake_minimum_required(VERSION 3.16)
project(parsers_test C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        socks5/hello_parser_test.c
        socks5/auth_parser_test.c
        socks5/request_parser_test.c
        client_hello_parser_test.c
        client_goodbye_parser_test.c
        client_timeout_parser_test.c
        client_get_timeout_parser_test.c
        client_set_buffersize_parser_test.c
        client_get_buffersize_parser_test.c
        client_metrics_parser_test.c
        client_set_user_parser_test.c
        client_list_users_parser_test.c
        client_del_user_parser_test.c
        pop3/pop3_auth_parser_test.c
)

add_library(parsers_test STATIC ${SOURCES})
target_link_libraries(parsers_test PUBLIC parsers)