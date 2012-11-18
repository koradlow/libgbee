/**
 * \file
 * \author  d264
 * \version $Rev$
 *
 * \section DESCRIPTION
 *
 * This is the main source file for the GBee driver. It contains implementation
 * of the GBee low level API.
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

#include "gbee.h"

#ifdef GBEE_PORT_DEBUG_LOG
#define GBEE_DEBUG_LOG GBEE_PORT_DEBUG_LOG
#else
#define GBEE_DEBUG_LOG(args...)
#endif

#ifdef GBEE_PORT_MEMORY_FREE
#define GBEE_MEMORY_FREE GBEE_PORT_MEMORY_FREE
#else
#define GBEE_MEMORY_FREE(p)
#endif

/**
 * Calculates and returns the frame data checksum.
 * 
 * \param[in] frameData points to the data to calculate the checksum for.
 * \param[in] length is the length of the frame data.
 * 
 * \return Frame data checksum.
 */
static uint8_t gbeeCalculateChecksum(const uint8_t *frameData, uint8_t length);

/**
 * Verifies the frame data checksum.
 * 
 * \param[in] frameData points to the data to verify the checksum for.
 * \param[in] length is the length of the frame data.
 * 
 * \return GBEE_NO_ERROR if the checksum is valid and GBEE_CHECKSUM_ERROR 
 * otherwise.
 */
static GBeeError gbeeVerifyChecksum(const uint8_t *frameData, uint8_t length,
		uint8_t checksum);
		
/**
 * Starts buffering data from the GBee, until the given maximum number of byte
 * is buffered, or until the stop character is received, or until no byte is
 * received within the given character timeout.
 * 
 * \param[in] self points to the GBee device.
 * \param[out] response is a pointer where to store the data.
 * \param[out] size is the number of bytes buffered.
 * \param[in] maxSize is the maximum number of bytes to buffer.
 * \param[in] if stop_char is received buffering is stopped.
 * \param[in] if no byte received after char_timeout ms buffering is stopped.
 * 
 * \return GBEE_NO_ERROR if successful, or dedicated error code in case of any
 * error.
 */
static GBeeError gbeeGetResponse(GBee *self, uint8_t *response, uint16_t *size,
		uint16_t maxSize, char stopChar, uint32_t charTimeout);

/**
 * GBee wait routine - delays for the requested number of milliseconds.
 * 
 * \param[in] self is a pointer to the GBee device structure.
 * \param[in] milliseconds specifies the number of milliseconds to delay.
 */
static void gbeeWait(GBee *self, uint32_t milliseconds);

/******************************************************************************/

GBee *gbeeCreate(const char *serialName)
{
	int deviceIndex;

	// Connect to the selected serial interface.
	if  ((deviceIndex = GBEE_PORT_UART_CONNECT(serialName)) < 0)
	{
		GBEE_DEBUG_LOG("%s: Error connecting serial interface \r\n", __func__);
		return NULL;
	}

	// Allocate self.
	GBee *self = (GBee *)GBEE_PORT_MEMORY_ALLOC(sizeof(GBee));
	if (!self)
	{
		GBEE_DEBUG_LOG("%s: Error allocating memory \r\n", __func__);
		return NULL;
	}

	// Initialize self.
	self->serialDevice = deviceIndex;
	self->lastError    = GBEE_NO_ERROR;
	
	return self;
}

/******************************************************************************/

void gbeeDestroy(GBee *self)
{
	GBEE_PORT_UART_DISCONNECT(self->serialDevice);
	GBEE_MEMORY_FREE(self);
}

/******************************************************************************/

GBeeError gbeeReceive(GBee *self, GBeeFrameData *frameData, uint16_t *length, 
		uint32_t *timeout)
{
	// Header of frame.
	GBeeFrameHeader frameHeader;
	// Trailer of frame.
	GBeeFrameTrailer frameTrailer;
	// Pointer to current byte being processed.
	uint8_t *bytePtr;
	// Size of received frame.
	uint8_t frameSize;
	// Elapsed time for reception.
	uint8_t elapsedTime;
	// GBee error code.
	GBeeError error = GBEE_NO_ERROR;
	// GBee read error code.
	GBeeError readError;
	
	// Check some pre-conditions.
	if (self->lastError != GBEE_NO_ERROR)
	{
		error = GBEE_INHERITED_ERROR;
	}
	GBEE_THROW(error);	

	// Prepare.
	*length                    = 0;
	frameSize                  = 0;
	frameHeader.startDelimiter = 0;
	frameHeader.length         = 0;
	elapsedTime                = 0;
	bytePtr                    = (uint8_t*)&frameHeader;
	
	GBEE_DEBUG_LOG("%s: ", __func__);
	
	// Loop to read the frame byte-wise.
	while (error == GBEE_NO_ERROR)
	{
		if ((*timeout != GBEE_NO_WAIT) && (*timeout != GBEE_INFINITE_WAIT))
		{
			// Check if timeout expired.
			if (elapsedTime >= (*timeout))
			{
				error = GBEE_TIMEOUT_ERROR;
				break;
			}
			(*timeout) -= elapsedTime;
		}

		// Get timestamp.
		elapsedTime = GBEE_PORT_TIME_GET();
		// Read a byte from the serial interface.
		readError = GBEE_PORT_UART_RECEIVE_BYTE(self->serialDevice, bytePtr, *timeout);
		// Calculate elapsed time.
		elapsedTime = GBEE_PORT_TIME_GET() - elapsedTime;

		GBEE_DEBUG_LOG("%02x ", *bytePtr);

		// Check for errors.
		if (readError != GBEE_NO_ERROR)
		{
			error = GBEE_FRAME_INTEGRITY_ERROR;
			break;
		}
		
		// First byte must be the start delimiter.
		if ((frameSize > 0) || (frameHeader.startDelimiter == 0x7E))
		{
			// Got a byte, so increment the frame size.
			frameSize++;

			// Got the header? Read the data.
			if (frameSize == sizeof(GBeeFrameHeader))
			{
				if (GBEE_USHORT(frameHeader.length) > GBEE_MAX_FRAME_SIZE)
				{
					error = GBEE_FRAME_SIZE_ERROR;
					break;
				}
				bytePtr = (uint8_t *)frameData;
			}
			// Got the data? Read the trailer.
			else if (frameSize == (sizeof(GBeeFrameHeader) + GBEE_USHORT(frameHeader.length)))
			{
				bytePtr = (uint8_t *)&frameTrailer;
			}
			// Got the trailer? Verify checksum.
			else if (frameSize == (sizeof(GBeeFrameHeader) + sizeof(GBeeFrameTrailer) + GBEE_USHORT(frameHeader.length)))
			{
				// Verify checksum.
				if (gbeeVerifyChecksum((uint8_t *)frameData, GBEE_USHORT(frameHeader.length),
						frameTrailer.checksum) != GBEE_NO_ERROR)
				{
					error = GBEE_CHECKSUM_ERROR;
					break;
				}
				else
				{
					*length = GBEE_USHORT(frameHeader.length);
					error  = GBEE_NO_ERROR;
					break;
				}
			}
			// Keep reading.
			else
			{
				bytePtr++;
			}
		}
	}

	GBEE_DEBUG_LOG("\r\n");
	return error;
}

/******************************************************************************/

GBeeError gbeeSend(GBee *self, GBeeFrameData *frameData, uint16_t length)
{
	// Pointer to header of frame to send.
	GBeeFrameHeader *frameHeader = (GBeeFrameHeader *)&self->scratch[0];
	// Pointer to trailer of frame to send.
	GBeeFrameTrailer *frameTrailer = (GBeeFrameTrailer *)&self->scratch[sizeof(GBeeFrameHeader)+length];
	// GBee error code.
	GBeeError error = GBEE_NO_ERROR;
	// Total frame size.
	uint16_t totalLength;
	
	// Check some pre-conditions.
	if (self->lastError != GBEE_NO_ERROR)
	{
		error = GBEE_INHERITED_ERROR;
	}
	else if (length > GBEE_MAX_FRAME_SIZE)
	{
		error = GBEE_FRAME_SIZE_ERROR;
	}
	GBEE_THROW(error);	
	
	// Create frame to send to the XBee.
	frameHeader->startDelimiter = 0x7E;
	frameHeader->length = GBEE_USHORT(length);
	GBEE_PORT_MEMORY_COPY(&self->scratch[sizeof(GBeeFrameHeader)], frameData, length);
	frameTrailer->checksum = gbeeCalculateChecksum((uint8_t *)frameData, length);
	totalLength = length + sizeof(GBeeFrameHeader) + sizeof(GBeeFrameTrailer);
		
	// Print the GBee message to send if debug logging is enabled.
#ifdef GBEE_PORT_DEBUG_LOG
	{
		uint16_t byteNr;
		printf("%s: ", __func__);
		for (byteNr = 0; byteNr < totalLength; byteNr++)
		{
			printf("%02x ", byteNr);
		}
		printf("\r\n");
	}
#endif // GBEE_PORT_DEBUG_LOG
	
	// Send the frame via the serial interface.
	error = GBEE_PORT_UART_SEND_BUFFER(self->serialDevice, self->scratch, totalLength);
	return error;
}

/******************************************************************************/

GBeeError gbeeSendAtCommand(GBee *self, uint8_t frameId, uint8_t *atCmd, 
		uint8_t *value, uint16_t length)
{
	// GBee error code.
	GBeeError error = GBEE_NO_ERROR;
	// AT command to send to GBee.
	GBeeAtCommand atCommand;

	// Check some pre-conditions.
	if (self->lastError != GBEE_NO_ERROR)
	{
		error = GBEE_INHERITED_ERROR;
	}
	GBEE_THROW(error);	

	// Assemble the AT command frame.
	atCommand.ident        = GBEE_AT_COMMAND;
	atCommand.frameId      = frameId;
	atCommand.atCommand[0] = atCmd[0];
	atCommand.atCommand[1] = atCmd[1];
	GBEE_PORT_MEMORY_COPY(atCommand.value, value, length);

	// Send the AT command frame.
	error = gbeeSend(self, (GBeeFrameData *)&atCommand, length + GBEE_AT_COMMAND_HEADER_LENGTH);
	return error;
}

/******************************************************************************/

GBeeError gbeeSendAtCommandQueue(GBee *self, uint8_t frameId, uint8_t *atCmd, 
		uint8_t *value, uint16_t length)
{
	// GBee error code.
	GBeeError error = GBEE_NO_ERROR;
	// AT command to send to GBee.
	GBeeAtCommandQueue atCommandQueue;

	// Check some pre-conditions.
	if (self->lastError != GBEE_NO_ERROR)
	{
		error = GBEE_INHERITED_ERROR;
	}
	GBEE_THROW(error);	

	// Assemble the AT command frame.
	atCommandQueue.ident        = GBEE_AT_COMMAND_QUEUE;
	atCommandQueue.frameId      = frameId;
	atCommandQueue.atCommand[0] = atCmd[0];
	atCommandQueue.atCommand[1] = atCmd[1];
	GBEE_PORT_MEMORY_COPY(atCommandQueue.value, value, length);

	// Send the AT command frame.
	error = gbeeSend(self, (GBeeFrameData *)&atCommandQueue, length + GBEE_AT_COMMAND_QUEUE_HEADER_LENGTH);
	return error;
}

/******************************************************************************/

GBeeError gbeeSendRemoteAtCommand(GBee *self, uint8_t frameId, uint32_t dstAddr64h,
		uint32_t dstAddr64l, uint16_t dstAddr16, uint8_t *atCmd, uint8_t cmdOpts,
		uint8_t *value, uint16_t length)
{
	// Error code returned by GBee.
	GBeeError error = GBEE_NO_ERROR;
	// Remote AT command parameters.
	GBeeRemoteAtCommand remoteAtCommand;

	// Check some pre-conditions.
	if (self->lastError != GBEE_NO_ERROR)
	{
		error = GBEE_INHERITED_ERROR;
	}
	GBEE_THROW(error);	

	// Assemble the AT command frame.
	remoteAtCommand.ident        = GBEE_REMOTE_AT_COMMAND;
	remoteAtCommand.frameId      = frameId;
	remoteAtCommand.dstAddr64h   = GBEE_ULONG(dstAddr64h);
	remoteAtCommand.dstAddr64l   = GBEE_ULONG(dstAddr64l);
	remoteAtCommand.dstAddr16    = GBEE_USHORT(dstAddr16);
	remoteAtCommand.atCommand[0] = atCmd[0];
	remoteAtCommand.atCommand[1] = atCmd[1];
	remoteAtCommand.cmdOpts      = cmdOpts;
	GBEE_PORT_MEMORY_COPY(remoteAtCommand.value, value, length);

	// Send the AT command frame.
	error = gbeeSend(self, (GBeeFrameData *)&remoteAtCommand,
			length + GBEE_REMOTE_AT_COMMAND_HEADER_LENGTH);
	return error;
}

/******************************************************************************/

GBeeError gbeeSendTxRequest(GBee *self, uint8_t frameId, uint32_t dstAddr64h, 
		uint32_t dstAddr64l, uint16_t dstAddr16, uint8_t bcastRadius,
		uint8_t options, uint8_t *data, uint16_t length) {

	// Error code returned by GBee.
	GBeeError error = GBEE_NO_ERROR;
	// 16bit address Tx request parameters.
	GBeeTxRequest txRequest;

	// Check some pre-conditions.
	if (self->lastError != GBEE_NO_ERROR)
	{
		error = GBEE_INHERITED_ERROR;
	}
	GBEE_THROW(error);

	// Assemble the AT command frame.
	txRequest.ident      = GBEE_TX_REQUEST;
	txRequest.frameId    = frameId;
	txRequest.dstAddr64h = GBEE_ULONG(dstAddr64h);
	txRequest.dstAddr64l = GBEE_ULONG(dstAddr64l);
	txRequest.dstAddr16  = GBEE_USHORT(dstAddr16);
	txRequest.bcastRadius = bcastRadius;
	txRequest.options    = options;
	GBEE_PORT_MEMORY_COPY(txRequest.data, data, length);

	// Send the AT command frame.
	error = gbeeSend(self, (GBeeFrameData *)&txRequest, length + GBEE_TX_REQUEST_HEADER_LENGTH);
	return error;
}



/******************************************************************************/

static GBeeError gbeeGetResponse(GBee *self, uint8_t *response, uint16_t *size,
		uint16_t maxSize, char stopChar, uint32_t charTimeout)
{
	// GBee error code.
	GBeeError error = GBEE_NO_ERROR;
	*size = 0;
	
	while (1)
	{
		// Read a byte.
		error = GBEE_PORT_UART_RECEIVE_BYTE(self->serialDevice, &response[*size],
				charTimeout);
		
		if (error == GBEE_NO_ERROR)
		{
			// Count the byte.
			(*size)++;
			
			// Got a byte. Check if it is the stop character.
			if (response[(*size)-1] == stopChar)
			{
				error = GBEE_NO_ERROR;
				break;
			}
			
			// Check if maximum number of bytes received.
			if (*size >= maxSize)
			{
				error = GBEE_NO_ERROR;
				break;
			}
		}
		else
		{
			// Either timeout or serial error.
			break;
		}
	}

	return error;
}

/******************************************************************************/

static void gbeeWait(GBee *self, uint32_t milliseconds)
{
	uint32_t initial_time = GBEE_PORT_TIME_GET();
	while (GBEE_PORT_TIME_GET() < (initial_time + milliseconds));
}

/******************************************************************************/

static uint8_t gbeeCalculateChecksum(const uint8_t *frameData, uint8_t length)
{
	// GBee 8-bit checksum.
	uint8_t checksum = 0;
	// Pointer to current byte.
	const uint8_t *bytePtr;

	// Add all bytes keeping only the lowest 8 bits of the result and subtract
	// from 0xFF.
	for (bytePtr = frameData; bytePtr < (frameData + length); bytePtr++)
	{
		checksum += *bytePtr;
	}
	return 0xFF - checksum;
}

/******************************************************************************/

static GBeeError gbeeVerifyChecksum(const uint8_t *frameData, uint8_t length, 
		uint8_t checksum)
{
	// Pointer to current byte.
	const uint8_t *bytePtr;

	// Add all bytes (include checksum). If the checksum is correct, the sum
	// will equal 0xFF.
	for (bytePtr = frameData; bytePtr < (frameData + length); bytePtr++)
	{
		checksum += *bytePtr;
	}
	return checksum == 0xFF ? GBEE_NO_ERROR : GBEE_CHECKSUM_ERROR;
}
