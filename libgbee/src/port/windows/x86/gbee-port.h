/**
 * \page building_for_windows_x86 Building for Windows on X86
 * \section  building_for_windows_x86 Building for Windows on X86
 *
 * You will need MinGW for building the libgbee for Microsoft Windows.
 *
 * In CMake-GUI specify the source directory of the libgbee and the build
 * directory where you want to build your binaries, e.g.
 * \code
 * C:\xbee\build\windows\x86\libgbee
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
 * \a windows.
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
 * \a C:\\Program Files.
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
 * This is a port of the GBee driver for Windows-based PC systems.
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <windows.h>

/** Building for a little endian machine. */
#define GBEE_PORT_LITTLE_ENDIAN
#undef  GBEE_PORT_BIG_ENDIAN

/**
 * Connects the GBee with the given COM port and performs port-specific
 * initialization.
 * 
 * \param[in] deviceName is the name of the COM port to open.
 * 
 * \return A handle to the TTY interface if successful and -1 in case of an 
 * error.
 */
int gbeePortComConnect(const char *deviceName);

/**
 * Closes the specified COM port.
 * 
 * \param[in] deviceIndex is the GBee/COM port connection index.
 */
void gbeePortComDisconnect(int deviceIndex);

/**
 * Writes the given byte buffer to the COM port.
 * 
 * \param[in] deviceIndex is the GBee/COM port connection index.
 * \param[in] buffer is the byte buffer to send.
 * \param[in] length is the number of bytes to send.
 * 
 * \return GBEE_NO_ERROR if successful; GBEE_RS232_ERROR in case of an error.
 */
GBeeError gbeePortComSendBuffer(int deviceIndex, const uint8_t *buffer,
		uint32_t length);

/**
 * Reads a byte from the COM port.
 * 
 * \param[in] deviceIndex is the handle returned by gbeePortComConnect.
 * \param[out] byte is the byte received from the serial interface.
 * \param[in] timeout specifies the timeout in milliseconds.
 * 
 * \returns GBEE_NO_ERROR if a byte was received, GBEE_TIMEOUT_ERROR if timed
 * out, or GBEE_RS232_ERROR in case of an error.
 */
GBeeError gbeePortComReceiveByte(int deviceIndex, uint8_t *byte, uint32_t timeout);

/** This macro is used by the GBee driver to connect to the UART. */
#define GBEE_PORT_UART_CONNECT gbeePortComConnect
/** This macro is used by the GBee driver to disconnect from the UART. */
#define GBEE_PORT_UART_DISCONNECT gbeePortComDisconnect
/** This macro is used by the GBee driver to send a buffer via the UART. */
#define GBEE_PORT_UART_RECEIVE_BYTE gbeePortComReceiveByte
/** This macro is used by the GBee driver to receive a byte from the UART. */
#define GBEE_PORT_UART_SEND_BUFFER gbeePortComSendBuffer
/** This macro is used by the GBee driver to allocate a block of memory. */
#define GBEE_PORT_MEMORY_ALLOC malloc
/** 
 * This macro is used by the GBee driver to free an allocated block of memory.
 * If this macro is undefined, the GBee driver will not try to free allocated
 * memory (which might be necessary on small embedded systems.
 */
#define GBEE_PORT_MEMORY_FREE free
/** This macro is used by the GBee driver to copy a block of memory. */
#define GBEE_PORT_MEMORY_COPY memcpy
/** This macro is used by the GBee driver to get current system time. */
#define GBEE_PORT_TIME_GET GetTickCount
/** 
 * This macro is used by the GBee driver to print debug messages.
 * If this macro is undefined, the GBee driver will not try to print debug
 * messages.
 */
#undef GBEE_PORT_DEBUG_LOG /*printf*/

#endif // GBEE_PORT_H_INCLUDED
