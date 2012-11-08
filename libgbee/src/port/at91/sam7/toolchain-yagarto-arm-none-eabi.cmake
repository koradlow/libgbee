# CMake Toolchain file for YAGARTO, Copyright (C) 2010, d264
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

INCLUDE(CMakeForceCompiler)

# Name of the target system (we use ``Generic'' here to indicate non-Linux and
# non-Windows target system).
SET(CMAKE_CROSSCOMPILING TRUE)
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)

# Specify the cross compiler.
CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ GNU)
SET(_CMAKE_TOOLCHAIN_PREFIX "arm-none-eabi-")

# YAGARTO root path.
SET(YAGARTO_ROOT_PATH
    "C:/Programme/YAGARTO" 
    CACHE
    STRING 
    "Root path of your YAGARTO installation")

# Specify directory search paths.
SET(CMAKE_FIND_ROOT_PATH ${YAGARTO_ROOT_PATH})

# Search for programs, libraries and headers in the target directories.
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
