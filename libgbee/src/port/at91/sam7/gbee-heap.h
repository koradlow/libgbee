/**
 * \file
 * \author  d264
 * \version $Rev$
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
 *
 * \section DESCRIPTION
 *
 * This is a port of the GBee driver for embedded systems based on a SAM7
 * microcontroller. This port uses the AT91LIB for USART handling and FreeRTOS
 * for interrupt and message queue control.
 *
 * For more information on the AT91LIB and FreeRTOS, please refer to
 * http://www.at91.com and http://www.FreeRTOS.org, respectively.
 */

#ifndef GBEE_HEAP_H_INCLUDED
#define GBEE_HEAP_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

/** The size of the heap in bytes. */
#define GBEE_HEAP_LENGTH 256

/**
 * Allocate a block of memory from the heap.
 *
 * \param[in] numberOfBytes is the number of bytes to allocated.
 *
 * \return A pointer to the allocated memory block if successful, or NULL if
 * requested number of bytes are not available.
 */
void *gbeeHeapAllocate(uint32_t numberOfBytes);

#endif /* GBEE_HEAP_H_INCLUDED */
