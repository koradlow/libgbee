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
 * Simple queue implementation for receiving data from the XBee.
 */

#include "gbee-usart.h"
#include <string.h>

/** USART1 RXD pin definition. */
#ifndef PIN_USART1_RXD
#define PIN_USART1_RXD  {1 << 5, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#endif /* PIN_USART1_RXD */

/** USART1 TXD pin definition. */
#ifndef PIN_USART1_TXD
#define PIN_USART1_TXD  {1 << 6, AT91C_BASE_PIOA, AT91C_ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#endif /* PIN_USART1_TXD */

/** Length of USART DMA buffer in bytes. */
#define GBEE_USART_DMA_BUFFER_LENGTH 128

/** Number of entries in the USART (device) table. */
#define GBEE_USART_COUNT (sizeof(deviceTable)/sizeof(GBeeUsartDevice))

/** Table of supported USART devices. */
static GBeeUsartDevice deviceTable[] = {
	{ "USART1", AT91C_ID_US1, AT91C_BASE_US1, { PIN_USART1_RXD, PIN_USART1_TXD } }
};

/** Table of supported GBee USARTs. */
static GBeeUsart usartTable[GBEE_USART_COUNT];

/**
 * Calculate the number of bytes written into the USART buffer via DMA.
 *
 * \param[in] deviceIndex is the index of the device in the USART table.
 */
#define NB_BYTE_WRITE(deviceIndex) \
		(GBEE_USART_BUFFER_LENGTH - usartTable[deviceIndex].device->base->US_RCR)

/**
 * Handler for USART receive buffer full interrupt.
 */
static void onBufferFull(void);

/******************************************************************************/

int gbeeUsartEnable(const char *deviceName)
{
	/* Index of the connection to configure. */
	int deviceIndex = 0;
	/* Flag telling if device was found. */
	bool foundDevice = false;
	/* USART mode: 9.6kbps, 8 data bits, no parity, 1 stop bit, no flow control. */
	uint32_t mode = AT91C_US_CLKS_CLOCK
	              | AT91C_US_CHRL_8_BITS
	              | AT91C_US_PAR_NONE
	              | AT91C_US_NBSTOP_1_BIT
	              | AT91C_US_CHMODE_NORMAL;

	/* Lookup the device name in the USART table. */
	while (deviceIndex < GBEE_USART_COUNT)
	{
		if (strcmp(deviceName, deviceTable[deviceIndex].name) == 0)
		{
			foundDevice = true;
			break;
		}
	}

	/* If device was not found in the table, return an error. */
	if (!foundDevice)
	{
		return -1;
	}

	/* Initialize the USART. */
	usartTable[deviceIndex].device = &deviceTable[deviceIndex];
	usartTable[deviceIndex].nbBufferWrite = 0;
	usartTable[deviceIndex].nbBufferRead = 0;
	usartTable[deviceIndex].nbByteRead = 0;

	/* Configure the selected USART pins. */
	PIO_Configure(usartTable[deviceIndex].device->pins,
			PIO_LISTSIZE(usartTable[deviceIndex].device->pins));

	/* Enable and configure selected USART. */
	PMC_EnablePeripheral(usartTable[deviceIndex].device->id);
	USART_Configure(usartTable[deviceIndex].device->base, mode, 9600, BOARD_MCK);
	USART_SetTransmitterEnabled(usartTable[deviceIndex].device->base, 1);
	USART_SetReceiverEnabled(usartTable[deviceIndex].device->base, 1);

	/* Configure the DMA channel for receiving data from the USART. */
	usartTable[deviceIndex].device->base->US_RPR = (uint32_t)usartTable[deviceIndex].buffer;
	usartTable[deviceIndex].device->base->US_RCR = GBEE_USART_BUFFER_LENGTH;
	usartTable[deviceIndex].device->base->US_PTCR = AT91C_PDC_RXTEN;

	/* Configure and enable USART receive interrupt. */
	AIC_ConfigureIT(usartTable[deviceIndex].device->id, 0, onBufferFull);
	AIC_EnableIT(usartTable[deviceIndex].device->id);
	usartTable[deviceIndex].device->base->US_IER = AT91C_US_ENDRX;

	/* Return the connection info. */
	return deviceIndex;
}

/******************************************************************************/

void gbeeUsartDisable(int deviceIndex)
{
	/* Disable USART and interrupt. */
	USART_SetTransmitterEnabled(usartTable[deviceIndex].device->base, 0);
	USART_SetReceiverEnabled(usartTable[deviceIndex].device->base, 0);
	PMC_DisablePeripheral(usartTable[deviceIndex].device->id);
	AIC_DisableIT(usartTable[deviceIndex].device->id);
}

/******************************************************************************/

void gbeeUsartBufferPut(int deviceIndex, const uint8_t *buffer, uint16_t length)
{
	/* Get a shortcut to the USART. */
	GBeeUsart *usart = &usartTable[deviceIndex];

	/* Wait until DMA channel is available. */
	while (usart->device->base->US_TCR > 0)
	{
		/* Keep waiting... */
	}

	/* Copy the data to a temporary buffer for DMA transmission. */
	static uint8_t dmaBuffer[GBEE_USART_DMA_BUFFER_LENGTH];
	memcpy(dmaBuffer, buffer, length);

	/* Start the DMA. */
	usart->device->base->US_TPR  = (uint32_t)buffer;
	usart->device->base->US_TCR  = length;
	usart->device->base->US_PTCR = AT91C_PDC_TXTEN;
}

/******************************************************************************/

bool gbeeUsartByteGet(int deviceIndex, uint8_t *byte)
{
	/* Tells if there is data available. */
	bool available = false;

	/* Check if data is available. */
	if (usartTable[deviceIndex].nbBufferWrite > usartTable[deviceIndex].nbBufferRead)
	{
		available = true;
		/* Check if DMA has overwritten unread data in the buffer. */
		if (usartTable[deviceIndex].nbByteRead <= NB_BYTE_WRITE(deviceIndex))
		{
			usartTable[deviceIndex].nbBufferRead = usartTable[deviceIndex].nbBufferWrite - 1;
			usartTable[deviceIndex].nbByteRead = NB_BYTE_WRITE(deviceIndex) + 1;
			if (usartTable[deviceIndex].nbByteRead >= GBEE_USART_BUFFER_LENGTH)
			{
				usartTable[deviceIndex].nbBufferRead = usartTable[deviceIndex].nbBufferWrite;
				usartTable[deviceIndex].nbByteRead = 0;
			}
		}
	}
	/* Check if data is available. */
	if (usartTable[deviceIndex].nbByteRead < NB_BYTE_WRITE(deviceIndex))
	{
		available = true;
	}

	/* If data is available then read it from the buffer and increment the
	 * number of bytes read. */
	if (available)
	{
		*byte = usartTable[deviceIndex].buffer[usartTable[deviceIndex].nbByteRead];
		usartTable[deviceIndex].nbByteRead++;
		if (usartTable[deviceIndex].nbByteRead >= GBEE_USART_BUFFER_LENGTH)
		{
			usartTable[deviceIndex].nbByteRead = 0;
			usartTable[deviceIndex].nbBufferRead++;
		}
		return true;
	}
	return false;
}

/******************************************************************************/

static void onBufferFull(void)
{
	/* Get the USART by its AT91LIB ID. */
	int tableIndex = 0;
	GBeeUsart *usart = (GBeeUsart *)0;
	while (tableIndex < GBEE_USART_COUNT)
	{
		if (usartTable[tableIndex].device->id == AT91C_BASE_AIC->AIC_ISR)
		{
			usart = &usartTable[tableIndex];
			break;
		}
		tableIndex++;
	}

	if (usart)
	{
		/* Reset the USART receive DMA. */
		usart->nbBufferWrite++;
		usart->device->base->US_RPR = (uint32_t)usart->buffer;
		usart->device->base->US_RCR = GBEE_USART_BUFFER_LENGTH;
		usart->device->base->US_PTCR = AT91C_PDC_RXTEN;
	}

	/* End the interrupt in the AIC. */
	AT91C_BASE_AIC->AIC_EOICR = 0x1;
}
