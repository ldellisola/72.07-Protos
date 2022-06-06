cmake_minimum_required(VERSION 3.16.3)
set(This Server)
project(${This})

set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(${This} src/main.c src/cli.c headers/cli.h src/tcp.c headers/tcp.h src/utils.c headers/utils.h src/socks.c headers/socks.h src/Logger.c headers/Logger.h src/parsers/hello_parser.c headers/parsers/hello_parser.h src/parsers/req_parser.c headers/parsers/req_parser.h src/parsers/auth_parser.c headers/parsers/auth_parser.h)

