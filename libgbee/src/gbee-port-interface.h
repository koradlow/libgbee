/**
 * \page porting Porting
 * \section porting Porting
 *
 * A libgbee port provides access to the serial interface and some platform
 * dependent functions. The libgbee uses macros to access the functions
 * provided by the port. These macros must be defined in the file gbee-port.h
 * for each port. Following macros are mandatory:
 *
 * \subsection gbee_port_uart_connect GBEE_PORT_UART_CONNECT
 * \code
 * int gbeePortConnect(const char *deviceName);
 * \endcode
 * to connect to the serial interface.
 * \param[in] deviceName is the name of the UART (e.g. "/dev/ttyS0") passed by
 * the calling application to the gbeeCreate() function.
 * \return A port-dependent device handle of the UART.
 *
 * \subsection gbee_port_uart_disconnect GBEE_PORT_UART_DISCONNECT
 * \code
 * void gbeePortDisconnect(int deviceIndex);
 * \endcode
 * to disconnect from the serial interface.
 * \param[in] deviceIndex is the device index returned by the call to
 * GBEE_PORT_UART_CONNECT.
 *
 * \subsection gbee_port_uart_receive_byte GBEE_PORT_UART_RECEIVE_BYTE
 * \code
 * GBeeError gbeePortReceiveByte(int       deviceIndex,
 *                               uint8_t  *byte,
 *                               uint32_t  timeout);
 * \endcode
 * to receive a byte from the serial interface.
 * \param[in] deviceIndex is the device index returned by the call to
 * GBEE_PORT_UART_CONNECT.
 * \param[out] byte is the received byte.
 * \param[in] timeout specifies a timeout in milliseconds.
 * \retval GBEE_NO_ERROR if successful.
 * \retval GBEE_TIMEOUT_ERROR if the timeout expired without any data received.
 * \retval GBEE_RS232_ERROR to indicate an error to establish serial
 * communication.
 *
 * \subsection gbee_port_uart_send_buffer GBEE_PORT_UART_SEND_BUFFER
 * \code
 * GBeeError gbeePortSendBuffer(int            deviceIndex,
 *                              const uint8_t *buffer,
 *                              uint32_t       length);
 * \endcode
 * to send a block of data to the serial device.
 * \param[in] deviceIndex is the device index returned by the call to
 * GBEE_PORT_UART_CONNECT.
 * \param[out] buffer points to the data to send.
 * \param[in] length is the number of bytes to send.
 * \retval GBEE_NO_ERROR to indicate success.
 * \retval GBEE_RS232_ERROR to indicate a failure establishing serial
 * communication.
 *
 * \subsection gbee_port_memory_alloc GBEE_PORT_MEMORY_ALLOC
 * \code
 * void *gbeePortMemoryAlloc(size_t size);
 * \endcode
 * to allocate a block of memory (same as malloc).
 * \param[in] size is the number of bytes to allocate.
 * \return A pointer to the allocated memory.
 *
 * \subsection gbee_port_memory_copy GBEE_PORT_MEMORY_COPY
 * \code
 * void *gbeePortMemoryCopy(void       *dest,
 *                          const void *src,
 *                          size_t      size);
 * \endcode
 * to copy a block of memory (like memcpy).
 * \param[out] dest points to the memory to copy to.
 * \param[in] src points to the memory to copy from.
 * \param[in] size is the number of bytes to copy.
 * \return A pointer to the destination memory.
 *
 * \subsection gbee_port_time_get GBEE_PORT_TIME_GET
 * \code
 * unsigned int gbeePortTimeGet(void);
 * \endcode
 * to get a timestamp (in milliseconds).
 * \return The current time in milliseconds.
 *
 * Additionally, there are some optional functions, which may be defined by a
 * port. If you do not want to provide these functions in your port you should
 * undef the corresponding macro. Following optional functions may be defined:
 *
 * \subsection gbee_port_memory_free GBEE_PORT_MEMORY_FREE
 * \code
 * void gbeePortMemoryFree(void *ptr);
 * \endcode
 * to free an allocated block of memory. Note, that this function is optional,
 * as on some embedded systems without heap management it may not be possible
 * to free allocated memory.
 * \param[in] ptr is a pointer to the allocated memory to free.
 *
 * \subsection gbee_port_debug_log GBEE_PORT_DEBUG_LOG
 * \code
 * int gbeePortDebugLog(const char *format, ...);
 * \endcode
 * to log debug messages to the console (like printf).
 * \param[in] format is a pointer to the printf-style format string.
 * \param[in] ... are additional parameters to be printed.
 * \return The number of characters printed.
 *
 * \subsection little_big_endian Little or Big Endian
 * Additionally, each port must either define the symbol GBEE_PORT_BIG_ENDIAN
 * or GBEE_PORT_LITTLE_ENDIAN, depending on the byte-order used by target
 * system.
 *
 * \subsection port_interface Port Interface
 * All types you need for writing an own port are defined in
 * gbee-port-interface.h.
 *
 * \file
 * \author  d264
 * \version $Rev$
 *
 * \section DESCRIPTION
 *
 * This is the libgbee port interface.
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

#ifndef GBEE_PORT_INTERFACE_H_INCLUDED
#define GBEE_PORT_INTERFACE_H_INCLUDED

#include <stdint.h>

/** Ensures that there is no error beyond this point. */
#define GBEE_THROW(e) if ((e) != GBEE_NO_ERROR) return (e)

/** No wait for receive packet. */
#define GBEE_NO_WAIT       0x0
/** Infinite wait for receive packet. */
#define GBEE_INFINITE_WAIT 0xFFFFFFFF

/** Enumeration of possible error codes. */
enum gbeeError {
	/** No error. */
	GBEE_NO_ERROR,
	/** GBee deactivated due to last error. */
	GBEE_INHERITED_ERROR,
	/** Error in RS232 communication. */
	GBEE_RS232_ERROR,
	/** Error in checksum calculation. */
	GBEE_CHECKSUM_ERROR,
	/** Maximum frame size exceeded. */
	GBEE_FRAME_SIZE_ERROR,
	/** Incomplete frame received. */
	GBEE_FRAME_INTEGRITY_ERROR,
	/** Unexpected start delimiter. */
	GBEE_FRAME_START_ERROR,
	/** Invalid mode for requested operation. */
	GBEE_MODE_ERROR,
	/** Unexpected response from GBee. */
	GBEE_RESPONSE_ERROR,
	/** Timeout elapsed. */
	GBEE_TIMEOUT_ERROR
};

/** Type definition for GBee error codes. */
typedef enum gbeeError GBeeError;

#include "gbee-port.h"

#endif /* GBEE_PORT_INTERFACE_H_INCLUDED */

#ifdef __cplusplus
}
#endif
