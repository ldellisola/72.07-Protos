cmake_minimum_required(VERSION 3.16)

include_directories(${CMAKE_SOURCE_DIR}/include/lib)

add_subdirectory(parsers)
add_subdirectory(socks5)
