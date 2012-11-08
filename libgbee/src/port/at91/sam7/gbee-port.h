/**
 * \page building_for_at91_sam7 Building for AT91-SAM7
 * \section building_for_at91_sam7 Building for AT91-SAM7
 *
 * You will need ATMEL's AT91LIB for building the LibGBee for this target
 * platform. The AT91LIB is a complete API for AT91 microcontrollers, including
 * the AT91SAM7 family. The libgbee uses this API for accessing the USART and
 * AIC. The complete AT91LIB can be downloaded here:
 * http://www.at91.com/samphpbb/viewtopic.php?f=4&t=5420
 *
 * In CMake-GUI specify the source directory of the libgbee and the build
 * directory where you want to build your binaries, e.g.
 * \code
 * ~/xbee/build/at91/sam7/libgbee
 * \endcode
 *
 * Click on the \a Configure button and in the dialog select <i>Specify
 * toolchain file for cross-compiling</i>. In the next step, cmake-gui asks for
 * a toolchain file. The toolchain file configures the toolchain used for the
 * build (i.e. compiler, linker, etc.). I have provided toolchain files for
 * using the arm-elf-gcc toolchain or the YAGARTO toolchain in the
 * <i>libgbee/src/port/at91/sam7</i> directory.
 *
 * In the next step you will have to configure the build options according to
 * the following table:
 * <table>
 * <tr>
 * <td>TARGET_OS</td>
 * <td>
 * Selects the operating system that is running on the target system; set to
 * \a at91.
 * </td>
 * </tr>
 * <tr>
 * <td>TARGET_CPU</td>
 * <td>
 * Selects the CPU (architecture) of the target system; set to \a sam7.
 * </td>
 * </tr>
 * <tr>
 * <td>CMAKE_INSTALL_PREFIX</td>
 * <td>
 * Selects the installation prefix for the library. The default is
 * \a /usr/local/arm.
 * </td>
 * </tr>
 * <tr>
 * <td>DO_DEBUG</td>
 * <td>Set to \a ON to enable debugging, or \a OFF to disable debugging.</td>
 * </tr>
 * <tr>
 * <td>AT91LIB_BOARD_NAME</td>
 * <td>
 * Set to the name of the AT91LIB source directory which is suitable for your
 * board. The board-specific sources can be found in at91lib/boards. For
 * instance, if you have an AT91SAM7X Evalutation Kit the suitable board
 * package would be \a at91sam7x-ek.
 * </td>
 * </tr>
 * <tr>
 * <td>AT91LIB_CHIP_NAME</td>
 * <td>
 * Set to the name of the AT91LIB source directory which is suitable for your
 * MCU. The MCU-specific sources can be found in
 * at91lib/boards/$AT91LIB_BOARD_NAME. For instance, if you have an AT91SAM7X
 * Evalutation Kit the suitable MCU package would be \a at91sam7x256.
 * </td>
 * </tr>
 * <tr>
 * <td>AT91LIB_INCLUDE_PATH</td>
 * <td>
 * Specifies the main include path of your AT91LIB. This is the path where the
 * board.h file is located. CMake will try to automatically locate the AT91LIB
 * in all default paths.
 * </td>
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
 * This is a port of the GBee driver for embedded systems based on a SAM7
 * microcontroller. This port uses the AT91LIB for USART handling and interrupt
 * control. For more information on the AT91LIB, please refer to
 * http://www.at91.com.
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

#include "gbee-heap.h"
#include "gbee-tick.h"
#include "gbee-usart.h"
#include <stdio.h>
#include <string.h>

/** By default, SAM7 is a little endian machine. */
#define GBEE_PORT_LITTLE_ENDIAN
#undef  GBEE_PORT_BIG_ENDIAN

/**
 * Transmits the given buffer on the USART specified by the device index.
 * 
 * \param[in] deviceIndex is the GBee/USART connection index.
 * \param[in] buffer is a pointer to the data to send.
 * \param[in] length is the number of bytes to send.
 * 
 * \return XB_STATUS_OK.
 */
GBeeError gbeePortUsartSendBuffer(int deviceIndex, const uint8_t *buffer, 
		uint16_t length);

/**
 * Receives a byte from the USART specified by the device index.
 * 
 * \param[in] deviceIndex is the GBee/USART connection index.
 * \param[in] byte points to the received byte.
 * \param[in] timeout is a timeout in milliseconds.
 * 
 * \return GBEE_NO_ERROR or GBEE_TIMEOUT_ERROR.
 */
GBeeError gbeePortUsartReceiveByte(int deviceIndex, uint8_t *byte, uint32_t timeout);

/** This macro is used by the GBee driver to connect to the UART. */
#define GBEE_PORT_UART_CONNECT gbeeUsartEnable
/** This macro is used by the GBee driver to disconnect from the UART. */
#define GBEE_PORT_UART_DISCONNECT gbeeUsartDisable
/** This macro is used by the GBee driver to send a buffer via the UART. */
#define GBEE_PORT_UART_SEND_BUFFER gbeePortUsartSendBuffer
/** This macro is used by the GBee driver to receive a byte from the UART. */
#define GBEE_PORT_UART_RECEIVE_BYTE gbeePortUsartReceiveByte
/** This macro is used by the GBee driver to allocate a block of memory. */
#define GBEE_PORT_MEMORY_ALLOC gbeeHeapAllocate
/** This macro is used by the GBee driver to free an allocated block of memory.
 * If this macro is undefined, the GBee driver will not try to free allocated
 * memory (which might be necessary on small embedded systems. */
#undef  GBEE_PORT_MEMORY_FREE
/** This macro is used by the GBee driver to copy a block of memory. */
#define GBEE_PORT_MEMORY_COPY memcpy
/** This macro is used by the GBee driver to get current system time. */
#define GBEE_PORT_TIME_GET gbeeTickGet
/** This macro is used by the GBee driver to print debug messages.
 * If this macro is undefined, the GBee driver will not try to print debug
 * messages. */
#undef  GBEE_PORT_DEBUG_LOG /*printf*/

#endif /* GBEE_PORT_H_INCLUDED */
