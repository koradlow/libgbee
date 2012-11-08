# Makefile for LibGBee, the generic XBee library, Copyright (C) 2010, d264
#
# This library is free software; you can redistribute it and/or modify it 
# under the terms of the GNU Lesser General Public License as published by the 
# Free Software Foundation; either version 2.1 of the License, or (at your 
# option) any later version.
# 
# This library is distributed in the hope that it will be useful, but WITHOUT 
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License 
# for more details.
# 
# You should have received a copy of the GNU Lesser General Public License 
# along with this library; if not, write to the Free Software Foundation, 
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

# Name of the project
PROJECT(LibGBee)

# Version of the library
SET(VERSION "0.3.0")

# Minimum CMAKE version required to build
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Target OS and CPU to build the library for
SET(TARGET_OS "" 
    CACHE 
    STRING 
    "Available options: linux, windows, at91")
    
SET(TARGET_CPU "" 
    CACHE 
    STRING 
    "Available options: x86, sam7, mipsel")

# Option for building debug or release version
SET(DO_DEBUG OFF 
    CACHE 
    BOOL
    "Select DEBUG or RELEASE build")

# Library source files
SET(SOURCES "src/gbee.c;src/gbee-util.c")

# Library include directory
INCLUDE_DIRECTORIES(src)

# Select port by target CPU and OS
INCLUDE("src/port/${TARGET_OS}/${TARGET_CPU}/libgbee-${TARGET_OS}-${TARGET_CPU}.cmake")

# Add port sources
SET(SOURCES "${SOURCES};${PORT_SOURCES}")

# Select optimization level
IF(DO_DEBUG)
	SET_SOURCE_FILES_PROPERTIES(${SOURCES}
	                            COMPILE_FLAGS "${PORT_COMPILE_FLAGS} -O0 -g")
	SET(VERSION "${VERSION}-debug")
ELSE(DO_DEBUG)
	SET_SOURCE_FILES_PROPERTIES(${SOURCES} 
	                            COMPILE_FLAGS "${PORT_COMPILE_FLAGS} -Os")
ENDIF(DO_DEBUG)

# Add the library to the project
ADD_LIBRARY(gbee-${TARGET_OS}-${TARGET_CPU} STATIC ${SOURCES})

# Copy Doxfile to build directory.
ADD_CUSTOM_COMMAND(TARGET     gbee-${TARGET_OS}-${TARGET_CPU} 
                   POST_BUILD
                   COMMAND    cmake -E copy ${CMAKE_SOURCE_DIR}/Doxyfile ${CMAKE_BINARY_DIR}
                   COMMENT    "Copying Doxyfile to build directory.")

# Rule for building HTML documentation with doxygen
ADD_CUSTOM_COMMAND(OUTPUT  html
                   COMMAND cmake -E chdir ${CMAKE_SOURCE_DIR} doxygen ARGS Doxyfile
                   COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/html ${CMAKE_BINARY_DIR}/html
                   COMMAND cmake -E remove_directory ${CMAKE_SOURCE_DIR}/html)

# Add "make doc" build target
ADD_CUSTOM_TARGET(doc DEPENDS html)
