find_package(Check REQUIRED)

include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)

add_subdirectory(lib)
add_subdirectory(src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(LIBS_TO_TEST parsers_test socks5_test tcp_test lulu_client_test server_test)

# Since Check uses Threads to paralelize the tests, it'Selector mandatory
# add pthread as a dependency, alongside the Check libraries.
add_executable(run_tests run_tests.c lib/utils/object_pool_tests.c lib/utils/object_pool_tests.h)
target_link_libraries(run_tests ${LIBS_TO_TEST} ${CHECK_LIBRARIES} pthread rt m subunit)

# Create testing target and redirect its output to `Testing` folder
add_test(NAME run_tests COMMAND run_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Testing)
set_tests_properties(run_tests PROPERTIES TIMEOUT 30)

