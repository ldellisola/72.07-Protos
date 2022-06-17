cmake_minimum_required(VERSION 3.16)
project(parsers C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        auth_parser.c
        hello_parser.c
        request_parser.c
        client_hello_parser.c
        client_goodbye_parser.c
        client_timeout_parser.c
        client_get_timeout_parser.c
)

add_library(parsers STATIC ${SOURCES})
target_link_libraries(parsers PUBLIC utils)