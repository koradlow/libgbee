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

# Name of the project.
PROJECT("xbee-echo-server")

# Version of the program.
SET(VERSION "0.3.0")

# Minimum CMake version required to build this project.
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Link the program for running from flash by default.
SET(PROJECT_TARGET_MEMORY "flash" CACHE STRING
    "Select target memory for code: flash, sram or loader")

# Find LibGBee.
FIND_PATH(LIBGBEE_INCLUDE_PATH "gbee.h"
          PATHS                "libgbee/src"
                               "../libgbee/src"
                               "../../libgbee/src")

FIND_LIBRARY(LIBGBEE "gbee-at91-sam7"
             PATHS   "${LIBGBEE_INCLUDE_PATH}/..")

LINK_LIBRARIES("xbee-echo-server" "${LIBGBEE}")

INCLUDE_DIRECTORIES("${LIBGBEE_INCLUDE_PATH}")
INCLUDE_DIRECTORIES("${LIBGBEE_INCLUDE_PATH}/port/at91/sam7")

# Set the name of the board for AT91LIB.
SET(AT91LIB_BOARD_NAME "ICSwift" CACHE STRING
    "Exact name of the board for AT91LIB, e.g. ICSwift")

# Set the name of the processor for AT91LIB.
SET(AT91LIB_CHIP_NAME "at91sam7x256" CACHE STRING
    "Exact name of the processor for AT91LIB, e.g. at91sam7x256")

# Add local directory to include path.
INCLUDE_DIRECTORIES(.)

# Find the AT91LIB include path.
FIND_PATH(AT91LIB_INCLUDE_PATH "board.h"
          PATHS                "at91lib"
                               "../at91lib"
                               "../../at91lib"
          PATH_SUFFIXES        "boards/${AT91LIB_BOARD_NAME}")

# Set AT91LIB include paths.
INCLUDE_DIRECTORIES(${AT91LIB_INCLUDE_PATH})
INCLUDE_DIRECTORIES("${AT91LIB_INCLUDE_PATH}/../..")
INCLUDE_DIRECTORIES("${AT91LIB_INCLUDE_PATH}/../../peripherals")

ADD_DEFINITIONS(-D VERSION=\\"${VERSION}\\"
                -D THUMB_INTERWORK
                -D ${AT91LIB_CHIP_NAME})

# Set the source files.
SET(SOURCES_C "server.c"
              "${AT91LIB_INCLUDE_PATH}/board_lowlevel.c"
              "${AT91LIB_INCLUDE_PATH}/board_memories.c"
              "${AT91LIB_INCLUDE_PATH}/../../peripherals/aic/aic.c"
              "${AT91LIB_INCLUDE_PATH}/../../peripherals/dbgu/dbgu.c"
              "${AT91LIB_INCLUDE_PATH}/../../peripherals/pio/pio.c"
              "${AT91LIB_INCLUDE_PATH}/../../peripherals/pio/pio_it.c"
              "${AT91LIB_INCLUDE_PATH}/../../peripherals/pmc/pmc.c"
              "${AT91LIB_INCLUDE_PATH}/../../peripherals/usart/usart.c"
              "${AT91LIB_INCLUDE_PATH}/../../utility/led.c"
              "${AT91LIB_INCLUDE_PATH}/../../utility/stdio.c"
              "${AT91LIB_INCLUDE_PATH}/../../utility/string.c")

SET(SOURCES_S "${AT91LIB_INCLUDE_PATH}/board_cstartup.S")

# Set compile flags.
SET_SOURCE_FILES_PROPERTIES(${SOURCES_C}
                            COMPILE_FLAGS "-Wall -mlong-calls -ffunction-sections -Os")

SET_SOURCE_FILES_PROPERTIES(${SOURCES_S} PROPERTIES
                            COMPILE_FLAGS "-Wall -D__ASSEMBLY__ -c"
                            LANGUAGE      C)

# Add the sources to the executable.
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES_C} ${SOURCES_S})

# Set AT91LIB linker settings.
SET(PROJECT_LDSCRIPT ${AT91LIB_INCLUDE_PATH}/${AT91LIB_CHIP_NAME}/${PROJECT_TARGET_MEMORY}.lds)
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES 
                      LINK_FLAGS "-T\"${PROJECT_LDSCRIPT}\" -nostartfiles -mthumb-interwork -Wl,-Map,${PROJECT_NAME}.map")

# Convert ELF to BIN.
ADD_CUSTOM_COMMAND(TARGET     ${PROJECT_NAME} 
                   POST_BUILD
                   COMMAND    ${ARM_ELF_GCC_ROOT}/bin/${_CMAKE_TOOLCHAIN_PREFIX}objcopy -O binary ${PROJECT_NAME} ${PROJECT_NAME}.bin
                   COMMENT    "Converting ELF to BIN")

# Rule for building HTML documentation with doxygen.
ADD_CUSTOM_COMMAND(TARGET  xbee-echo-server
                   POST_BUILD
                   COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/Doxyfile ${CMAKE_BINARY_DIR}
                   COMMENT "Copying Doxyfile to build directory.")

# Rule for building HTML documentation with doxygen
ADD_CUSTOM_COMMAND(OUTPUT  html
                   COMMAND cmake -E chdir ${CMAKE_SOURCE_DIR} doxygen ARGS Doxyfile
                   COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/html ${CMAKE_BINARY_DIR}/html
                   COMMAND cmake -E remove_directory ${CMAKE_SOURCE_DIR}/html)
                   
# Add "make doc" build target.
ADD_CUSTOM_TARGET(doc
		          DEPENDS html)
