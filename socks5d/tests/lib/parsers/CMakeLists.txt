cmake_minimum_required(VERSION 3.16)
project(parsers_test C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        hello_parser_test.c
        auth_parser_test.c
        request_parser_test.c request_parser_test.h)

add_library(parsers_test STATIC ${SOURCES})
target_link_libraries(parsers_test PUBLIC parsers)