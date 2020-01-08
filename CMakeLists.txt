cmake_minimum_required(VERSION 3.10)

project(chunx VERSION 1.0 LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTING "Build unit tests" ON)

find_package(Boost REQUIRED)

add_library(libchunx
        chunx/length_predicate.cpp
        chunx/length_predicate.h
        #chunx/OutputStreamBuffer.cpp
        #chunx/OutputStreamBuffer.h
        #chunx/InputStreamBuffer.cpp
        #chunx/InputStreamBuffer.h
        chunx/chunk_if.cpp
        chunx/chunk_if.h
        chunx/detail/join_iterator.cpp
        chunx/detail/join_iterator.h
        chunx/join_range.cpp
        chunx/join_range.h)

set_target_properties(libchunx PROPERTIES OUTPUT_NAME chunx)

target_include_directories(libchunx PUBLIC
        $<BUILD_INTERFACE:${chunx_SOURCE_DIR}>
        $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
        $<INSTALL_INTERFACE:include>)

if (MSVC)
    target_compile_options(libchunx PRIVATE /W4 /WX /MP)
else ()
    target_compile_options(libchunx PRIVATE -Wall -Wextra -pedantic -Werror)
endif ()

if (BUILD_TESTING)
    enable_testing()
    find_package(GTest MODULE REQUIRED)
    add_executable(ChunxTest
            chunx/test/main.cpp
            chunx/test/length_predicate_test.cpp
            #chunx/test/OutputStream.cpp
            #chunx/test/InputStream.cpp)
            )
    target_link_libraries(ChunxTest PRIVATE GTest::GTest GTest::Main libchunx)
    gtest_discover_tests(ChunxTest)
    if (MSVC)
        target_compile_options(ChunxTest PRIVATE /W4 /WX /MP)
    else ()
        target_compile_options(ChunxTest PRIVATE -Wall -Wextra -pedantic -Werror)
    endif ()
endif ()


include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${chunx_BINARY_DIR}/chunxConfigVersion.cmake"
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY AnyNewerVersion)

install(TARGETS libchunx
        EXPORT chunxTargets
        INCLUDES DESTINATION include
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(DIRECTORY chunx DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        PATTERN "chunx/test/*" EXCLUDE
        PATTERN "chunx/test" EXCLUDE)

include(CMakePackageConfigHelpers)
configure_package_config_file(
        "${chunx_SOURCE_DIR}/cmake/chunxConfig.cmake"
        "${chunx_BINARY_DIR}/chunxConfig.cmake"
        INSTALL_DESTINATION share/cmake/chunx
)

install(EXPORT chunxTargets DESTINATION share/cmake/chunx)
install(FILES "${chunx_BINARY_DIR}/chunxConfigVersion.cmake"
        "${chunx_BINARY_DIR}/chunxConfig.cmake"
        DESTINATION share/cmake/chunx)
