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
 * Simple heap manager for GBee port.
 */

#include "gbee-heap.h"

/** This is our heap. */
static uint8_t heap[GBEE_HEAP_LENGTH];

/** Pointer to tail of allocated heap. */
static uint8_t *heapPtr = heap;

/*****************************************************************************/

void *gbeeHeapAllocate(uint32_t numberOfBytes)
{
	/* Pointer to allocated memory block. */
	uint8_t *allocPtr = heapPtr;

	/* Check if requested number of bytes available. */
	if ((heapPtr + numberOfBytes) > (heap + GBEE_HEAP_LENGTH))
	{
		return (void *)0;
	}

	/* Allocate the memory block. */
	heapPtr += numberOfBytes;
	return (void *)allocPtr;
}

