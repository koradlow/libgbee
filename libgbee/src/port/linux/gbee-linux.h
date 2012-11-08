/**
 * \file
 * \author  d264
 * \version $Rev$
 *
 * \section DESCRIPTION
 *
 * This is a port of the GBee driver for Linux-based systems.
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

#ifdef __cplusplus
extern "C"{
#endif 

#ifndef GBEE_LINUX_H_INCLUDED
#define GBEE_LINUX_H_INCLUDED

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/**
 * Connect the GBee with the given TTY interface and perform port-specific
 * initialization.
 * 
 * \param[in] deviceName is the name of the TTY interface to open.
 * 
 * \return A handle to the TTY interface if successful and -1 in case of an 
 * error.
 */
int gbeePortTTYConnect(const char *deviceName);

/**
 * Close the specified serial interface.
 * 
 * \param[in] deviceIndex is the GBee/TTY connection index.
 */
void gbeePortTTYDisconnect(int deviceIndex);

/**
 * Write the given byte buffer to the TTY interface.
 * 
 * \param[in] deviceIndex is the GBee/TTY connection index.
 * \param[in] buffer is the byte buffer to send.
 * \param[in] length is the number of bytes to send.
 * 
 * \retval GBEE_NO_ERROR to indicate success.
 * \retval GBEE_RS232_ERROR to indicate an error.
 */
GBeeError gbeePortTTYSendBuffer(int deviceIndex, const uint8_t *buffer, 
		uint32_t length);

/**
 * Read a byte from the serial buffer.
 * 
 * \param[in] deviceIndex is the handle returned by serialOpen.
 * \param[out] byte is the byte received from the serial interface.
 * \param[in] timeout specifies the timeout in milliseconds.
 * 
 * \retval GBEE_NO_ERROR to indicate success.
 * \retval GBEE_TIMEOUT_ERROR to indicate timeout expired wihtou any data being
 * received.
 * \retval GBEE_RS232_ERROR to indicate a serial communication error.
 */
GBeeError gbeePortTTYReceiveByte(int deviceIndex, uint8_t *byte, uint32_t timeout);

/**
 * Return the current time in milliseconds.
 *
 * \return The current timestamp in milliseconds.
 */
uint32_t gbeePortTimeGet(void);

/** This macro is used by the GBee driver to connect to the UART. */
#define GBEE_PORT_UART_CONNECT gbeePortTTYConnect
/** This macro is used by the GBee driver to disconnect from the UART. */
#define GBEE_PORT_UART_DISCONNECT gbeePortTTYDisconnect
/** This macro is used by the GBee driver to send a buffer via the UART. */
#define GBEE_PORT_UART_RECEIVE_BYTE gbeePortTTYReceiveByte
/** This macro is used by the GBee driver to receive a byte from the UART. */
#define GBEE_PORT_UART_SEND_BUFFER gbeePortTTYSendBuffer
/** This macro is used by the GBee driver to allocate a block of memory. */
#define GBEE_PORT_MEMORY_ALLOC malloc
/** This macro is used by the GBee driver to free an allocated block of memory.
 * If this macro is undefined, the GBee driver will not try to free allocated
 * memory (which might be necessary on small embedded systems.
 */
#undef GBEE_PORT_MEMORY_FREE
/** This macro is used by the GBee driver to copy a block of memory. */
#define GBEE_PORT_MEMORY_COPY memcpy
/** This macro is used by the GBee driver to get current system time. */
#define GBEE_PORT_TIME_GET gbeePortTimeGet
/** This macro is used by the GBee driver to print debug messages.
 * If this macro is undefined, the GBee driver will not try to print debug
 * messages.
 */
#undef GBEE_PORT_DEBUG_LOG /* you can use printf here */

#endif /* GBEE_LINUX_H_INCLUDED */

#ifdef __cplusplus
}
#endif
