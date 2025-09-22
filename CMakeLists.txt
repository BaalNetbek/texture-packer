cmake_minimum_required(VERSION 3.22)

set(APPLICATION_NAME "texpacker")

project(${APPLICATION_NAME})

include(FindPkgConfig)
include(CheckLibraryExists)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "=== RELEASE MODE ===")
    add_definitions("-DNDEBUG")
    add_definitions("-Wall -O3")
else()
    message(STATUS "=== DEBUG MODE ===")
    add_definitions("-DDEBUG")
    add_definitions("-Wall -Wextra -pedantic -O0 -g")
endif()

# additional compiler flags
set(CMAKE_COLOR_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/third-party/fmtlib/include
    )

# link_directories(
# )

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/fmtlib)

link_libraries(
    FmtLib
    )

add_executable(${APPLICATION_NAME}
    ${SOURCES}
    )

