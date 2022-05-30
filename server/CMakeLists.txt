cmake_minimum_required(VERSION 3.16.3)
set(This Server)
project(${This})

set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(${This} src/main.c src/cli.c headers/cli.h src/tcp.c headers/tcp.h src/utils.c headers/utils.h)

