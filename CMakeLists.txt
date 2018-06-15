cmake_minimum_required(VERSION 2.8)

project(chunx LANGUAGES CXX)

include_directories(${CMAKE_BINARY_DIR}/include)
link_directories(${CMAKE_BINARY_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Werror")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(libchunx
        chunx/FixedSizeChunker.cpp chunx/FixedSizeChunker.h
        chunx/OutputStream.cpp chunx/OutputStream.h
        chunx/InputStream.cpp chunx/InputStream.h)
set_target_properties(libchunx PROPERTIES OUTPUT_NAME chunx)

add_subdirectory(external/gtest)

target_include_directories(libchunx INTERFACE "${chunx_SOURCE_DIR}")

install(TARGETS libchunx
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)
install(DIRECTORY chunx DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        PATTERN "chunx/test/*" EXCLUDE
        PATTERN "chunx/test" EXCLUDE)

SET(GTEST_LIBRARIES gtest)
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    SET(GTEST_LIBRARIES gtestd)
endif ()

enable_testing()
add_executable(TestChunx
        chunx/test/main.cpp
        chunx/test/FixedSizeChunker.cpp
        chunx/test/OutputStream.cpp
        chunx/test/InputStream.cpp)
add_dependencies(TestChunx googletest)

target_link_libraries(TestChunx ${GTEST_LIBRARIES} libchunx)
set_target_properties(TestChunx PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_CURRENT_BINARY_DIR}/test)
add_test(NAME TestChunx
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test/TestChunx)
