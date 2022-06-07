cmake_minimum_required(VERSION 3.16)

include_directories(${CMAKE_SOURCE_DIR}/include/src)

add_subdirectory(server)
add_subdirectory(lulu_client)