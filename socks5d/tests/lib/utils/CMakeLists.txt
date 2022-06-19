cmake_minimum_required(VERSION 3.16)
project(utils_test C)

set(CMAKE_C_STANDARD 11)

set(
        SOURCES
        object_pool_tests.c
)

add_library(utils_test STATIC ${SOURCES})
target_link_libraries(utils_test PUBLIC utils)