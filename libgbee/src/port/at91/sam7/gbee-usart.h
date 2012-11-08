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
 * USART wrapper for GBee port.
 */

#ifndef GBEE_USART_H_INCLUDED
#define GBEE_USART_H_INCLUDED

#include "board.h"
#include "peripherals/aic/aic.h"
#include "peripherals/pio/pio.h"
#include "peripherals/pmc/pmc.h"
#include "peripherals/usart/usart.h"
#include <stdint.h>
#include <stdbool.h>

/** USART device information structure. */
struct gbeeUsartDevice {
	/** Name to identify the USART. */
	const char name[8];
	/** USART identifier. */
	uint32_t id;
	/** USART base address. */
	AT91PS_USART base;
	/** USART pins. */
	Pin pins[2];
};

/** Type definition for USART information structure. */
typedef struct gbeeUsartDevice GBeeUsartDevice;

/** Length of a USART's receive buffer. */
#define GBEE_USART_BUFFER_LENGTH 256

/** This is a simple queue with a read and a write pointer. */
struct gbeeUsart {
	/** The queue buffer memory. */
	uint8_t buffer[GBEE_USART_BUFFER_LENGTH];
	/** Number of buffer write wrap-arounds. */
	uint32_t nbBufferWrite;
	/** Number of buffer read wrap-arounds. */
	uint32_t nbBufferRead;
	/** Number of bytes read from buffer. */
	uint32_t nbByteRead;
	/** Pointer to the USART filling this queue. */
	GBeeUsartDevice *device;
};

/** Type definition for GBee queue. */
typedef struct gbeeUsart GBeeUsart;

/**
 * Initialize and enable the GBee USART.
 *
 * \param[in] deviceName is the name of the USART to enable.
 *
 * \return A device index identifying the queue.
 */
int gbeeUsartEnable(const char *deviceName);

/**
 * Disable the given USART.
 *
 * \param[in] deviceIndex is the USART to disable.
 */
void gbeeUsartDisable(int deviceIndex);

/**
 * Sets up a DMA transfer to transmit the given buffer via the USART. Returns
 * immediately if the DMA channel is available, otherwise the function waits
 * until the DMA channel gets available.
 *
 * \param[in] deviceIndex is the device index returned by gbeeUsartInit.
 * \param[in] buffer is a pointer to the data buffer to send.
 * \param[in] length is the number of bytes to send.
 */
void gbeeUsartBufferPut(int deviceIndex, const uint8_t *buffer, uint16_t length);

/**
 * Gets a byte from the USART.
 *
 * \param[in] deviceIndex is the device index returned by gbeeUsartInit.
 * \param[out] byte is the byte from the queue.
 *
 * \return true to indicate success, false to indicate no data.
 */
bool gbeeUsartByteGet(int deviceIndex, uint8_t *byte);

#endif /* GBEE_USART_H_INCLUDED */
