/**
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

#include "gbee-port-interface.h"

/** Macro for printing debug messages. */
#ifdef GBEE_PORT_DEBUG_LOG
#define GBEE_PORT_TRACE GBEE_PORT_DEBUG_LOG
#else
#define GBEE_PORT_TRACE(args...)
#endif /* GBEE_PORT_DEBUG_LOG */

/******************************************************************************/

int gbeePortComConnect(const char *deviceName)
{
	/* COM port handle - used as device index. */
	HANDLE deviceHandle;
	/* COM port device control block. */
	DCB deviceControl;
	
	/* Open the COM port. */
	deviceHandle = CreateFile(deviceName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING,
			0, 0);
	if (deviceHandle == INVALID_HANDLE_VALUE)
	{
		GBEE_PORT_TRACE("gbeePortComConnect: Error creating device %s: %d \r\n",
				deviceName, GetLastError());
		return -1;
	}

	/* Set COM port event notification. */
	if (!SetCommMask(deviceHandle, EV_RXCHAR | EV_TXEMPTY))
	{
		GBEE_PORT_TRACE("gbeePortComConnect: Error setting COM port event notification: %d \r\n",
				GetLastError());
		CloseHandle(deviceHandle);
		return -1;
	}

	/* Configure baud rate and other COM port parameters. */
	if (!GetCommState(deviceHandle, &deviceControl))
	{
		GBEE_PORT_TRACE("gbeePortComConnect: Error getting COM port configuration: %d \r\n",
				GetLastError());
		CloseHandle(deviceHandle);
		return -1;
	}

	deviceControl.DCBlength       = sizeof(DCB);
	deviceControl.BaudRate        = CBR_9600;
	deviceControl.ByteSize        = 8;
	deviceControl.Parity          = NOPARITY;
	deviceControl.StopBits        = ONESTOPBIT;
	deviceControl.fDsrSensitivity = FALSE;
	deviceControl.fDtrControl     = DTR_CONTROL_ENABLE;
	deviceControl.fOutxDsrFlow    = FALSE;

	if (!SetCommState(deviceHandle, &deviceControl))
	{
		GBEE_PORT_TRACE("gbeePortComConnect: Error setting COM port configuration: %d \r\n",
				GetLastError());
		CloseHandle(deviceHandle);
		return -1;
	}

	/* The device handle specifies the device index. */
	return (int)deviceHandle;
}

/******************************************************************************/

void gbeePortComDisconnect(int deviceIndex)
{
	if (deviceIndex >= 0)
	{
	    /* Close the COM port. */
		CloseHandle((HANDLE)deviceIndex);
	}
}

/******************************************************************************/

GBeeError gbeePortComSendBuffer(int deviceIndex, const uint8_t *buffer, uint32_t length)
{
	/* Write result. */
	BOOL result;
	/* Number of bytes written. */
	DWORD bytesWritten;
	/* Event received. */
	DWORD event;

	/* Send the buffer to the COM port. */
	result = WriteFile((HANDLE)deviceIndex, buffer, length, &bytesWritten, NULL);
	if ((result != TRUE) || (bytesWritten != length))
	{
		return GBEE_RS232_ERROR;
	}

	/* Wait until bytes have been transmitted. */
	do
	{
		WaitCommEvent((HANDLE)deviceIndex, &event, NULL);
	}
	while ((event & EV_TXEMPTY) == 0);
	return GBEE_NO_ERROR;
}

/******************************************************************************/

GBeeError gbeePortComReceiveByte(int deviceIndex, uint8_t *byte, uint32_t timeout)
{
	/* COM port timeout structure. */
	COMMTIMEOUTS comTimeout;
	/* Number of bytes read from COM port. */
	DWORD bytesRead;

	/* Set the timeout. */
	comTimeout.ReadIntervalTimeout         = MAXDWORD;
	comTimeout.ReadTotalTimeoutMultiplier  = MAXDWORD;
	comTimeout.ReadTotalTimeoutConstant    = timeout;
	comTimeout.WriteTotalTimeoutMultiplier = MAXDWORD;
	comTimeout.WriteTotalTimeoutConstant   = MAXDWORD;
	if (!SetCommTimeouts((HANDLE)deviceIndex, &comTimeout))
	{
		return GBEE_RS232_ERROR;
	}

	/* Wait for character received from COM port. */
	if (!ReadFile((HANDLE)deviceIndex, byte, 1, &bytesRead, NULL))
	{
		return GBEE_RS232_ERROR;
	}

	if (bytesRead == 1)
	{
		return GBEE_NO_ERROR;
	}
	else
	{
		return GBEE_TIMEOUT_ERROR;
	}
}
