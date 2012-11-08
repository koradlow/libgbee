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
 * microcontroller. This port uses the AT91LIB for interrupt and USART
 * handling.
 * 
 * For more information on the AT91LIB, please refer to http://www.at91.com.
 */

#include "gbee-port-interface.h"
#include <stdbool.h>

/******************************************************************************/

GBeeError gbeePortUsartSendBuffer(int deviceIndex, const uint8_t *buffer, 
		uint16_t length)
{
	gbeeUsartBufferPut(deviceIndex, buffer, length);
	return GBEE_NO_ERROR;
}

/******************************************************************************/

GBeeError gbeePortUsartReceiveByte(int deviceIndex, uint8_t *byte, uint32_t timeout)
{
	/* Wait until there is a byte available, or timeout elapsed. */
	uint32_t absTimeout = gbeeTickTimeoutCalculate(timeout);
	do
	{
		if (gbeeUsartByteGet(deviceIndex, byte))
		{
			return GBEE_NO_ERROR;
		}
	}
	while (!gbeeTickTimeoutExpired(absTimeout) || (timeout == GBEE_INFINITE_WAIT));
	return GBEE_TIMEOUT_ERROR;
}
