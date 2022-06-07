cmake_minimum_required(VERSION 3.16)
project(utils C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        utils.c
        logger.c
)

add_library(utils STATIC ${SOURCES})

