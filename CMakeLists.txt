# WFDB Software Package - CMake Configuration
# Based on original configure script and Makefiles
# Modern CMake build system for WFDB

cmake_minimum_required(VERSION 3.10)
project(WFDB VERSION 10.7.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(ENABLE_NETFILES "Enable network file support" ON)
option(ENABLE_FLAC "Enable FLAC support" ON)
option(ENABLE_WAVE "Enable WAVE support" OFF)
option(BUILD_APPS "Build applications" ON)
option(BUILD_EXAMPLES "Build examples" ON)

# Installation directories
include(GNUInstallDirs)

# Platform detection and configuration
if(WIN32)
    add_definitions(-DMSDOS)
    add_definitions(-DNOMKSTEMP)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/dist" CACHE PATH "Default install path" FORCE)
endif()

if(NOT WIN32)
    add_definitions(-DWFDB_LARGETIME)
endif()

add_definitions(-DNOVALUES_H)

# Version definitions
add_definitions(-DWFDB_MAJOR=10)
add_definitions(-DWFDB_MINOR=7)
add_definitions(-DWFDB_RELEASE=0)

# Find packages
if(ENABLE_NETFILES)
    find_package(CURL)
    if(CURL_FOUND)
        add_definitions(-DNETFILES)
        add_definitions(-DNETFILES_LIBCURL)
        include_directories(${CURL_INCLUDE_DIRS})
    else()
        message(WARNING "libcurl not found, NETFILES support disabled")
        set(ENABLE_NETFILES OFF)
    endif()
endif()

if(ENABLE_FLAC)
    find_package(PkgConfig)
    if(PkgConfig_FOUND)
        pkg_check_modules(FLAC flac)
        if(FLAC_FOUND)
            include_directories(${FLAC_INCLUDE_DIRS})
        else()
            message(WARNING "libflac not found, FLAC support disabled")
            set(ENABLE_FLAC OFF)
        endif()
    endif()
endif()

# Compiler flags
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wno-implicit -Wformat)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O2)
    endif()
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)

# Subdirectories
add_subdirectory(lib)

if(BUILD_APPS)
    add_subdirectory(app)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Optional subdirectories
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/psd)
    add_subdirectory(psd)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/xml)
    add_subdirectory(xml)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/convert)
    add_subdirectory(convert)
endif()

# Configuration summary
message(STATUS "")
message(STATUS "WFDB Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build shared libraries: ${BUILD_SHARED_LIBS}")
message(STATUS "  Network files support: ${ENABLE_NETFILES}")
message(STATUS "  FLAC support: ${ENABLE_FLAC}")
message(STATUS "  WAVE support: ${ENABLE_WAVE}")
message(STATUS "  Build applications: ${BUILD_APPS}")
message(STATUS "  Build examples: ${BUILD_EXAMPLES}")
message(STATUS "  Installation prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")

# CPack configuration
set(CPACK_PACKAGE_NAME "WFDB")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "WFDB Software Package")
set(CPACK_PACKAGE_VENDOR "PhysioNet")
set(CPACK_PACKAGE_CONTACT "wfdb@physionet.org")
include(CPack)