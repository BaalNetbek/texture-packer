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
set( CMAKE_CXX_FLAGS "-std=c++14" )

find_package(PNG)
# find_package(JPEG)

include_directories(
    ${PNG_INCLUDE_DIR}
    # ${JPEG_INCLUDE_DIR}
    )

link_directories(
    ${PNG_LIBRARY_DIRS}
    # ${JPEG_LIBRARY_DIRS}
    )

file( GLOB_RECURSE SOURCES "src/*.cpp" )

link_libraries (
    ${PNG_LIBRARY}
    # ${JPEG_LIBRARY}
    )

add_executable( packimages
    WIN32 # Only if you don't want the DOS prompt to appear in the background in Windows
    # MACOSX_BUNDLE
    ${SOURCES}
    )

