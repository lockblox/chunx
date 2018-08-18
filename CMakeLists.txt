cmake_minimum_required(VERSION 2.8)

project(chunx LANGUAGES CXX)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(Boost_USE_STATIC_LIBS ON)
find_library(Boost REQUIRED COMPONENTS utility)
include_directories(${CMAKE_BINARY_DIR}/include)
link_directories(${CMAKE_BINARY_DIR}/lib)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Werror")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(libchunx
        chunx/FixedSizeChunker.cpp chunx/FixedSizeChunker.h
        chunx/OutputStreamBuffer.cpp chunx/OutputStreamBuffer.h
        chunx/InputStreamBuffer.cpp chunx/InputStreamBuffer.h)
set_target_properties(libchunx PROPERTIES OUTPUT_NAME chunx)

if (NOT TARGET gtest)
    add_subdirectory(external/googletest)
endif()

target_include_directories(libchunx PUBLIC "${chunx_SOURCE_DIR}")

install(TARGETS libchunx
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)
install(DIRECTORY chunx DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        PATTERN "chunx/test/*" EXCLUDE
        PATTERN "chunx/test" EXCLUDE)

enable_testing()
add_executable(TestChunx
        chunx/test/main.cpp
        chunx/test/FixedSizeChunker.cpp
        chunx/test/OutputStream.cpp
        chunx/test/InputStream.cpp)

target_link_libraries(TestChunx gtest libchunx)
set_target_properties(TestChunx PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_CURRENT_BINARY_DIR}/test)
add_test(NAME TestChunx
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test/TestChunx)
