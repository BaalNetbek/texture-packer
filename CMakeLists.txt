project(packimages)
cmake_minimum_required(VERSION 2.6)

include(FindPkgConfig)
include(CheckLibraryExists)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if( CMAKE_BUILD_TYPE STREQUAL "Release" )
    message(STATUS "=== RELEASE MODE ===")
    add_definitions( "-DNDEBUG" )
    add_definitions( "-Wall -O2" )
else()
    message(STATUS "=== DEBUG MODE ===")
    add_definitions( "-DDEBUG" )
    add_definitions( "-Wall -Wextra -pedantic -O0 -g" )
endif()

# additional compiler flags
set( CMAKE_CXX_FLAGS "-std=c++11" )

include_directories(
    ${PROJECT_SOURCE_DIR}/src
    )

# link_directories(
    # )

file( GLOB_RECURSE SOURCES "src/*.cpp" )

# link_libraries (
    # )

add_executable( packimages
    WIN32 # Only if you don't want the DOS prompt to appear in the background in Windows
    # MACOSX_BUNDLE
    ${SOURCES}
    )

