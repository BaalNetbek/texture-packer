PROJECT(packimages)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(FindPkgConfig)
INCLUDE(CheckLibraryExists)

SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

IF( CMAKE_BUILD_TYPE STREQUAL "Release" )
    MESSAGE(STATUS "=== RELEASE MODE ===")
    ADD_DEFINITIONS( "-DNDEBUG" )
    ADD_DEFINITIONS( "-Wall -O2" )
ELSE()
    MESSAGE(STATUS "=== DEBUG MODE ===")
    ADD_DEFINITIONS( "-DDEBUG" )
    ADD_DEFINITIONS( "-Wall -Wextra -pedantic -O0 -g" )
ENDIF()

# additional compiler flags
SET( CMAKE_CXX_FLAGS "-std=c++14" )

FIND_PACKAGE(PNG)
# FIND_PACKAGE(JPEG)

INCLUDE_DIRECTORIES(
    ${PNG_INCLUDE_DIR}
    # ${JPEG_INCLUDE_DIR}
    )

LINK_DIRECTORIES(
    ${PNG_LIBRARY_DIRS}
    # ${JPEG_LIBRARY_DIRS}
    )

SET( SOURCES
    src/file.cpp
    src/image.cpp
    src/imagesaver.cpp
    src/main.cpp
    src/packer.cpp
    src/rect.cpp
    src/trim.cpp
    )

LINK_LIBRARIES (
    ${PNG_LIBRARY}
    # ${JPEG_LIBRARY}
    )

ADD_EXECUTABLE( packimages
    WIN32 # Only if you don't want the DOS prompt to appear in the background in Windows
    # MACOSX_BUNDLE
    ${SOURCES}
    )

