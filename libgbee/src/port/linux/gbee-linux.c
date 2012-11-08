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

#include "gbee-port-interface.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

/******************************************************************************/

int gbeePortTTYConnect(const char *deviceName)
{
	// Device index used to identifiy the TTY connected to the GBee.
	int deviceIndex =  0;
	// TTY options.
	struct termios options;

	// Open the serial device with the given name.
	deviceIndex = open(deviceName, O_RDWR|O_NOCTTY|O_NDELAY);
	if (deviceIndex < 0)
	{
		return -1;
	}

	// Configure the serial device.
	tcgetattr(deviceIndex, &options);
	
	options.c_cflag  = B9600;
	options.c_cflag |= CS8;
	options.c_cflag |= CREAD;
	options.c_cflag |= CLOCAL;
	options.c_cflag |= CRTSCTS;
	options.c_iflag  = IGNPAR | IGNBRK;
	
	options.c_oflag     = 0;
	options.c_lflag     = 0;
	options.c_cc[VTIME] = 0;
	options.c_cc[VMIN]  = 1;
	
	tcflush(deviceIndex, TCIOFLUSH);
	tcsetattr(deviceIndex, TCSANOW, &options);

	return deviceIndex;
}

/******************************************************************************/

void gbeePortTTYDisconnect(int deviceIndex)
{
	if (deviceIndex >= 0)
	{
	    // Close the serial port.
		close(deviceIndex);
	}
}

/******************************************************************************/

GBeeError gbeePortTTYSendBuffer(int deviceIndex, const uint8_t *buffer, uint32_t length)
{
	int result = write(deviceIndex, buffer, length);
	return (uint32_t)result != length ? GBEE_RS232_ERROR : GBEE_NO_ERROR;
}

/******************************************************************************/

GBeeError gbeePortTTYReceiveByte(int deviceIndex, uint8_t *byte, uint32_t timeout)
{
	// File descriptor set used for select call.
	fd_set readSet;
	// Timeval for timeout calculation.
	struct timeval timeVal;
	// POSIX result.
	int result;

	// Watch the serial device to see when it has input.
	FD_ZERO(&readSet);
	FD_SET(deviceIndex, &readSet);

	// Calculate the timeout in seconds and microseconds.
	timeVal.tv_sec  = timeout / 1000;
	timeVal.tv_usec = (timeout % 1000) * 1000;

    // Wait for the character.
	result = select(deviceIndex+1, &readSet, NULL, NULL, &timeVal);
	if (result < 0)
	{
		return GBEE_RS232_ERROR;
	}
	else if (result > 0)
	{
		result = read(deviceIndex, byte, 1);
		if (result == 1)
		{
			return GBEE_NO_ERROR;
		}
		else
		{
			return GBEE_RS232_ERROR;
		}
	}
	else
	{
		return GBEE_TIMEOUT_ERROR;
	}
}

/******************************************************************************/

unsigned int gbeePortTimeGet(void)
{
	struct timeval timeVal;
	gettimeofday(&timeVal, NULL);
	return timeVal.tv_sec * 1000 + timeVal.tv_usec / 1000;
}
