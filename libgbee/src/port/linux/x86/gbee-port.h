/**
 * \page building_for_linux_x86 Building for Linux on X86
 * \section  building_for_linux_x86 Building for Linux on X86
 *
 * In CMake-GUI specify the source directory of the libgbee and the build
 * directory where you want to build your binaries, e.g.
 * \code
 * ~/xbee/build/linux/x86/libgbee
 * \endcode
 *
 * Click on the \a Configure button and in the dialog select <i>Use default
 * native compilers</i>.
 *
 * In the next step you will have to configure the build options according to
 * the following table:
 * <table>
 * <tr>
 * <td>TARGET_OS</td>
 * <td>
 * Selects the operating system that is running on the target system; set to
 * \a linux.
 * </td>
 * </tr>
 * <tr>
 * <td>TARGET_CPU</td>
 * <td>
 * Selects the CPU (architecture) of the target system; set to \a x86.
 * </td>
 * </tr>
 * <tr>
 * <td>CMAKE_INSTALL_PREFIX</td>
 * <td>
 * Selects the installation prefix for the library. The default is
 * \a /usr/local.
 * </td>
 * </tr>
 * <tr>
 * <td>DO_DEBUG</td>
 * <td>Set to \a ON to enable debugging, or \a OFF to disable debugging.</td>
 * </tr>
 * </table>
 *
 * After configuration if the build options clock on the button \a Configure
 * again, and then \a Generate to generate the Makefiles. In a console window
 * browse to your build directory and type \a make.
 *
 * \file
 * \author  d264
 * \version $Rev$
 *
 * \section DESCRIPTION
 *
 * This is a port of the GBee driver for x86 Linux systems.
 *
 * \section LICENSE
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef GBEE_PORT_H_INCLUDED
#define GBEE_PORT_H_INCLUDED

/** Building for a little endian machine. */
#define GBEE_PORT_LITTLE_ENDIAN
#undef  GBEE_PORT_BIG_ENDIAN

/** Include Linux common part. */
#include "gbee-linux.h"

#endif // GBEE_PORT_H_INCLUDED
