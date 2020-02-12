cmake_minimum_required(VERSION 3.1.0)
project(format VERSION 0.1.0)

option(FORMAT_BUILD_TESTS "build tests" OFF)
if(${FORMAT_BUILD_TESTS})
    enable_testing()
    add_subdirectory(test)
endif()

add_library(format INTERFACE)
target_include_directories(format INTERFACE include/)
