# Makefile for XBee-Echo-Client, Copyright (C) 2010, d264
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

# Name of the project.
PROJECT(xbee-echo-client)

# Version of the program.
SET(PROJECT_VERSION "0.3.0")

# List of source files.
SET(SOURCES "client.c")

# Set compile flags passed via command line.
SET_SOURCE_FILES_PROPERTIES(${SOURCES}
                            COMPILE_FLAGS "-Wall -g")

ADD_DEFINITIONS(-DPROJECT_VERSION="${PROJECT_VERSION}")

# Rule for building the executable from the source files.
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES})

# Rule for building HTML documentation with doxygen.
ADD_CUSTOM_COMMAND(TARGET  xbee-echo-client
                   POST_BUILD
                   COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/Doxyfile ${CMAKE_BINARY_DIR}
                   COMMENT "Copying Doxyfile to build directory.")

# Rule for building HTML documentation with doxygen
ADD_CUSTOM_COMMAND(OUTPUT  html
                   COMMAND cmake -E chdir ${CMAKE_SOURCE_DIR} doxygen ARGS Doxyfile
                   COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/html ${CMAKE_BINARY_DIR}/html
                   COMMAND cmake -E remove_directory ${CMAKE_SOURCE_DIR}/html)
                   
# Add "make doc" build target.
ADD_CUSTOM_TARGET(doc DEPENDS html)

# Rule for installing the XBee Echo Client.
INSTALL(PROGRAMS    xbee-echo-client
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
