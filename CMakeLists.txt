cmake_minimum_required(VERSION 2.8)

project(chunx LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Werror")
include_directories(${chunx_SOURCE_DIR}/include)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(libchunx 
            src/FixedSizeChunker.cpp include/chunx/FixedSizeChunker.h
            src/OutputStream.cpp include/chunx/OutputStream.h
            src/InputStream.cpp  include/chunx/InputStream.h)
set_target_properties(libchunx PROPERTIES OUTPUT_NAME chunx)

add_subdirectory(external/gtest)

target_include_directories(libchunx INTERFACE "${chunx_SOURCE_DIR}/include")

enable_testing()
add_executable(TestChunx 
               test/main.cpp
               test/FixedSizeChunker.cpp
               test/OutputStream.cpp
               test/InputStream.cpp)

target_link_libraries(TestChunx gtest libchunx)
set_target_properties(TestChunx PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_CURRENT_BINARY_DIR}/test)
add_test(NAME TestChunx
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test/TestChunx)
