cmake_minimum_required(VERSION 3.22)
project(server C)

set(CMAKE_C_STANDARD 11)

add_executable(server src/main.c src/cli.c headers/cli.h)
