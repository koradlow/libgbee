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

# Add source files for the port
SET(PORT_SOURCES "src/port/at91/sam7/gbee-port.c"
                 "src/port/at91/sam7/gbee-heap.c"
                 "src/port/at91/sam7/gbee-tick.c"
                 "src/port/at91/sam7/gbee-usart.c")

# Set include directory for the port
INCLUDE_DIRECTORIES(src/port/at91/sam7)

# Find AT91LIB include path
SET(AT91LIB_BOARD_NAME "at91sam7x-ek" 
    CACHE 
    STRING 
    "Exact name of the board, e.g. at91sam7x-ek")
		
SET(AT91LIB_CHIP_NAME "at91sam7x256" 
    CACHE 
    STRING
    "Exact name of the MCU, e.g. at91sam7x256")
		
FIND_PATH(AT91LIB_INCLUDE_PATH "board.h"
          PATHS                ".."
                               "../.."
          PATH_SUFFIXES        "at91lib/boards/${AT91LIB_BOARD_NAME}")

# Set AT91LIB include path
INCLUDE_DIRECTORIES(${AT91LIB_INCLUDE_PATH})
INCLUDE_DIRECTORIES("${AT91LIB_INCLUDE_PATH}/../..")

# Set AT91LIB-specific compiler settings
SET(PORT_COMPILE_FLAGS "-mlong-calls -mcpu=arm7tdmi -fomit-frame-pointer -fno-strict-aliasing -mthumb-interwork")
ADD_DEFINITIONS("-DTHUMB_INTERWORK -D${AT91LIB_CHIP_NAME}")
