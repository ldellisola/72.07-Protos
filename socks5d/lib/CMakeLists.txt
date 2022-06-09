cmake_minimum_required(VERSION 3.16)

include_directories(${CMAKE_SOURCE_DIR}/include/lib)

add_subdirectory(parsers)
add_subdirectory(socks5)
add_subdirectory(tcp)
add_subdirectory(utils)
add_subdirectory(selector)
add_subdirectory(fsm)