# Makefile for xbee-tunnel-daemon, Copyright (C) 2011, d264
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

# Require minimum CMake version.
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Include PkgConfig support
INCLUDE(FindPkgConfig)

# Name of the project.
PROJECT(xbee-tunnel-daemon)

# Version of the program.
SET(PROJECT_VERSION "0.3.0")

# List of source files.
SET(SOURCES "daemon.c"
            "gbee-inet.c"
            "tunnel.c")

# Set compile flags passed via command line.
SET_SOURCE_FILES_PROPERTIES(${SOURCES} COMPILE_FLAGS "-Wall -g -std=c99")
ADD_DEFINITIONS(-DPROJECT_VERSION="${PROJECT_VERSION}")
ADD_DEFINITIONS(-DPROJECT_NAME="${PROJECT_NAME}")

# Rule for building the executable from the source files.
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES})

SET(LIBGBEE_NAME "gbee-linux-x86")
SET(XBEE_TUNNEL_CONTROL_FILENAME "xbee-tunnel-control.sh")

# Find LibGBee.
FIND_PATH(LIBGBEE_INCLUDE_PATH "gbee.h"
          PATHS                "libgbee/src"
                               "../libgbee/src"
                               "../../libgbee/src")

INCLUDE_DIRECTORIES(${LIBGBEE_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${LIBGBEE_INCLUDE_PATH}/port/linux/x86)
INCLUDE_DIRECTORIES(${LIBGBEE_INCLUDE_PATH}/port/linux)

FIND_LIBRARY(LIBGBEE "${LIBGBEE_NAME}"
             PATHS   "${LIBGBEE_INCLUDE_PATH}/..")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBGBEE} pthread rt m)

# Copy the daemonizer scripts to the build directory.
ADD_CUSTOM_COMMAND(TARGET     ${PROJECT_NAME} 
                   POST_BUILD
                   COMMAND    cmake -E copy ${CMAKE_SOURCE_DIR}/${XBEE_TUNNEL_CONTROL_FILENAME} ${CMAKE_BINARY_DIR}
                   COMMENT    "Copying ${XBEE_TUNNEL_CONTROL_FILENAME} to build directory.")

# Rule for building HTML documentation with doxygen
ADD_CUSTOM_COMMAND(OUTPUT  html
                   COMMAND cmake -E chdir ${CMAKE_SOURCE_DIR} doxygen ARGS Doxyfile
                   COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/html ${CMAKE_BINARY_DIR}/html
                   COMMAND cmake -E remove_directory ${CMAKE_SOURCE_DIR}/html)

# Add "make doc" build target
ADD_CUSTOM_TARGET(doc DEPENDS html)
