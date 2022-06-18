cmake_minimum_required(VERSION 3.16)
project(socks5d C)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_scripts")

# Determine minimum C
set(CMAKE_C_STANDARD 11)

# Enable debugging
set(CMAKE_BUILD_TYPE Debug)

# Show all warning messages
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wno-unused-parameter -Wno-implicit-fallthrough -pedantic -pedantic-errors")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_POSIX_C_SOURCE=200112L ")

add_subdirectory(lib)
add_subdirectory(src)

#enable_testing()
#
#add_subdirectory(tests)

add_custom_target(graphviz ALL
        COMMAND ${CMAKE_COMMAND} "--graphviz=foo.dot" .
        COMMAND dot -Tpng foo.dot -o foo.png
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
