#
#   Copyright (C) 2016 National Institute For Space Research (INPE) - Brazil.
#
#  This file is part of WTSS.TL.
#
#  WTSS.TL is free software: you can
#  redistribute it and/or modify it under the terms of the
#  GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License,
#  or (at your option) any later version.
#
#  WTSS.TL is distributed in the hope that
#  it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with WTSS.TL. See LICENSE. If not, write to
#  e-sensing team at <esensing-team@dpi.inpe.br>.
#
#  Description: Main CMake script for WTSS.TL build system.
#
#  Author: Gilberto Ribeiro de Queiroz
#          Matheus Cavassan Zaglia
#

cmake_minimum_required(VERSION 2.8.12)

project(terralib_mod_qt_wtss)

#
# main variables for the build script
#

# variables that control the system versioning
set(WTSS_TL_VERSION_MAJOR 0)
set(WTSS_TL_VERSION_MINOR 1)
set(WTSS_TL_VERSION_PATCH 0)
set(WTSS_TL_VERSION_STATUS "")
set(WTSS_TL_VERSION_STATUS_FULL "")
set(WTSS_TL_VERSION_STRING "${WTSS_TL_VERSION_MAJOR}.${WTSS_TL_VERSION_MINOR}.${WTSS_TL_VERSION_PATCH}${WTSS_TL_VERSION_STATUS_FULL}")
set(WTSS_TL_VERSION "0x000100")

# variables storing absolute paths
set(WTSS_TL_ABSOLUTE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# variables that control paths
set(WTSS_TL_BASE_DESTINATION_DIR "")



# variables that control install locations
if(NOT DEFINED WTSS_TL_DESTINATION_UNITTEST)
  set(WTSS_TL_DESTINATION_UNITTEST "${WTSS_TL_BASE_DESTINATION_DIR}share/wtss-tl/unittest")
endif()

if(NOT DEFINED WTSS_TL_DESTINATION_LIBRARY)
  set(WTSS_TL_DESTINATION_LIBRARY "${WTSS_TL_BASE_DESTINATION_DIR}lib")
endif()

if(NOT DEFINED WTSS_TL_DESTINATION_RUNTIME)
  set(WTSS_TL_DESTINATION_RUNTIME "${WTSS_TL_BASE_DESTINATION_DIR}bin")
endif()

if(NOT DEFINED WTSS_TL_DESTINATION_ARCHIVE)
  set(WTSS_TL_DESTINATION_ARCHIVE "${WTSS_TL_BASE_DESTINATION_DIR}lib")
endif()

if(NOT DEFINED WTSS_TL_DESTINATION_HEADERS)
  set(WTSS_TL_DESTINATION_HEADERS "${WTSS_TL_BASE_DESTINATION_DIR}include")
endif()

if(NOT DEFINED WTSS_TL_DESTINATION_SHARE)
  set(WTSS_TL_DESTINATION_HEADERS "${WTSS_TL_BASE_DESTINATION_DIR}share")
endif()

#
# global definitions, includes and config
#

# Common definitions to all modules
add_definitions(-DBOOST_ALL_NO_LIB -DBOOST_FILESYSTEM_VERSION=3 -DBOOST_UBLAS_TYPE_CHECK=0)

# Specific definitions per platform
#if(APPLE)
#  add_definitions(-ftemplate-depth-1024)
#endif()

# For each CMake version we need to check some policies
# Note: automatically link executables with qtmain.lib on Windows when it has Qt dependency.
if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" VERSION_GREATER  2.8.9)
  cmake_policy(SET CMP0020 NEW)
endif()

if(${CMAKE_MAJOR_VERSION} GREATER 2)
  cmake_policy(SET CMP0026 OLD)
  cmake_policy(SET CMP0042 NEW)
  cmake_policy(SET CMP0043 NEW)
endif()

if(${CMAKE_VERSION} VERSION_GREATER "3.1.2")
  cmake_policy(SET CMP0053 OLD)
endif()

# Build will generate files into a single folder
if((CMAKE_GENERATOR STREQUAL Xcode) OR MSVC)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

# Where to find modules (find procedures)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")

# Compiler options: warning and C++ 11
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER MATCHES ".*clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++11")
endif()


# Enable CTest
enable_testing()

# Build tree directory will be needed in all projects.
include_directories(${CMAKE_BINARY_DIR})


#
# first check of libraries
#

#find_package(Boost 1.54.0 REQUIRED system date_time chrono log timer filesystem program_options thread)
find_package(Boost 1.54.0 REQUIRED system filesystem)

if(NOT Boost_FOUND)
  message(FATAL_ERROR "WTSS_TL: could not find required Boost libraries!")
endif()

find_package(Qt5 5.1 REQUIRED COMPONENTS Core Gui Widgets )

if(NOT Qt5_FOUND)
  message(FATAL_ERROR "WTSS_TL: could not find required Qt5 libraries!")
endif()

find_package(terralib REQUIRED)

find_package(Qt5PrintSupport REQUIRED)

#find_package(RapidJSON REQUIRED)

#find_package(CppNetLib REQUIRED)

#find_package(OpenSSL)

#find_package(Doxygen)


#
# some variables needed in config file
#

#
# add include targets
#
include_directories(${Boost_INCLUDE_DIR})
include_directories(${terralib_INCLUDE_DIRS})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#include_directories(${CPPNETLIB_INCLUDE_DIR})
#include_directories(${RAPIDJSON_INCLUDE_DIR})

#
# build targets
#


# wtss.tl client library
file(GLOB WTSS_TL_SRC_FILES ${WTSS_TL_ABSOLUTE_ROOT_DIR}/src/wtss-tl/*.cpp)
file(GLOB WTSS_TL_HDR_FILES ${WTSS_TL_ABSOLUTE_ROOT_DIR}/src/wtss-tl/*.hpp)

add_library(terralib_mod_qt_wtss_plugin SHARED ${WTSS_TL_SRC_FILES} ${WTSS_TL_HDR_FILES})

target_link_libraries(terralib_mod_qt_wtss_plugin #wtss_cxx_client
                                     terralib_mod_plugin
                                     terralib_mod_qt_apf
                                     ${Boost_SYSTEM_LIBRARY})

qt5_use_modules(terralib_mod_qt_wtss_plugin Widgets Network Core)

# wtss.tl unit-test
include_directories(${WTSS_TL_ABSOLUTE_ROOT_DIR}/src)

file(GLOB WTSS_TL_UTEST_SRC_FILES ${WTSS_TL_ABSOLUTE_ROOT_DIR}/src/unittest/*.cpp)
file(GLOB WTSS_TL_UTEST_HDR_FILES ${WTSS_TL_ABSOLUTE_ROOT_DIR}/src/unittest/*.hpp)

#add_executable(terralib_mod_wtss_unittest ${WTSS_TL_UTEST_HDR_FILES} ${WTSS_TL_UTEST_SRC_FILES})

# target_link_libraries(terralib_mod_wtss_unittest terralib_mod_qt_wtss_plugin Qt5::PrintSupport)
target_link_libraries(terralib_mod_qt_wtss_plugin Qt5::PrintSupport)

#qt5_use_modules(terralib_mod_wtss_unittest Widgets Network Core)

# file organization in some IDEs
source_group("wtss_tl\\Source Files"  FILES ${WTSS_TL_SRC_FILES})
source_group("wtss_tl\\Header Files"  FILES ${WTSS_TL_HDR_FILES})
#source_group("wtss_tl\\ui"            FILES ${WTSS_TL_UI_FILES})

# source_group("wtss_unittest\\Source Files"  FILES ${WTSS_TL_UTEST_SRC_FILES})
# source_group("wtss_unittest\\Header Files"  FILES ${WTSS_TL_UTEST_HDR_FILES})


#
# process WTSS_TL configuration files
#

configure_file(${WTSS_TL_ABSOLUTE_ROOT_DIR}/src/wtss-tl/version.hpp.in
               ${CMAKE_BINARY_DIR}/wtss_tl_version.hpp @ONLY)

#
# install and targets export
#

install(TARGETS terralib_mod_qt_wtss_plugin
        EXPORT wtss_tl-targets
        RUNTIME DESTINATION ${WTSS_TL_DESTINATION_RUNTIME} COMPONENT runtime
        LIBRARY DESTINATION ${WTSS_TL_DESTINATION_LIBRARY} COMPONENT runtime
        ARCHIVE DESTINATION ${WTSS_TL_DESTINATION_ARCHIVE} COMPONENT runtime)

install(FILES ${CMAKE_BINARY_DIR}/wtss_tl_version.hpp
              ${WTSS_TL_HDR_FILES}
        DESTINATION ${WTSS_TL_DESTINATION_HEADERS}/wtss-tl COMPONENT devel)

install(FILES ${WTSS_TL_ABSOLUTE_ROOT_DIR}/share/wtss-tl/plugins/te.qt.wtss.teplg
        DESTINATION ${terralib_DIR}/../../../share/terralib/plugins COMPONENT runtime)

install(DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib
        DESTINATION ${terralib_DIR}/../../.. COMPONENT runtime)

# install(FILES ${WTSS_TL_UTEST_SRC_FILES}
#         DESTINATION ${WTSS_TL_DESTINATION_UNITTEST} COMPONENT devel)

export(TARGETS terralib_mod_qt_wtss_plugin FILE ${CMAKE_BINARY_DIR}/wtss-tl-exports.cmake)
