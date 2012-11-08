/**
 * \file
 * \author  d264
 * \version $Rev$
 *
 * \section DESCRIPTION
 *
 * Source file with GBee utility functions.
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

#include "gbee-util.h"

/** Default Xbee frame ID. */
#define GBEE_UTIL_DEFAULT_FRAME_ID 0xCF

/** UDP header. */
struct __attribute__((__packed__)) udpHeader {
	uint16_t fromPort; /**< UDP packet source port. */
	uint16_t toPort;   /**< UDP packet destination port. */
	uint16_t length;   /**< UDP packet length in bytes. */
	uint16_t checksum; /**< UDP packet checksum (0 = don't care). */
};

/** UDP header type definition. */
typedef struct udpHeader UdpHeader;

/******************************************************************************/

GBeeError gbeeUtilSetAddress16(GBee *gbee, uint16_t addr, uint16_t pan)
{
	/* GBee 16-bit address (register "MY"). */
	uint16_t my;
	/* GBee 16-bit PAN-ID (register "ID"). */
	uint16_t id;
	/* Error code returned by the GBee driver. */
	GBeeError error;
	
	/* Set the GBee's 16-bit address. */
	my    = GBEE_USHORT(addr);
	error = gbeeUtilWriteRegister(gbee, "MY", (uint8_t*)&my, sizeof(my));
	GBEE_THROW(error);		                         

	/* Set GBee's PAN ID. */
	id    = GBEE_USHORT(pan);
	error = gbeeUtilWriteRegister(gbee, "ID", (uint8_t*)&id, sizeof(id));
	GBEE_THROW(error);

	return GBEE_NO_ERROR;
}

/******************************************************************************/

GBeeError gbeeUtilGetAddress16(GBee *gbee, uint16_t *addr, uint16_t *pan)
{
	/* Error code returned by the GBee driver. */
	GBeeError error;
	/* Length of data read from register. */
	uint16_t length;
	
	/* Read GBee's 16-bit address. */
	error = gbeeUtilReadRegister(gbee, "MY", (uint8_t *)addr, &length, sizeof(uint16_t));
	GBEE_THROW(error);	

	/* Convert endianess. */
	(*addr) = GBEE_USHORT(*addr);	

	/* Read GBee's PAN-ID. */
	error = gbeeUtilReadRegister(gbee, "MY", (uint8_t*)pan, &length, sizeof(uint16_t));
	GBEE_THROW(error);	

	/* Convert endianess. */
	(*pan) = GBEE_USHORT(*pan);	

	return GBEE_NO_ERROR;
}

/******************************************************************************/

GBeeError gbeeUtilWriteRegister(GBee *gbee, const char *regName, const uint8_t *value,
		uint16_t length)
{
	/* GBee error code. */
	GBeeError error;
	/* GBee response to AT commands. */
	GBeeAtCommandResponse atCommandResponse;
	/* Length of AT command response. */
	uint16_t responseLength;
	/* Timeout in milliseconds. */
	uint32_t timeout;
	
	/* Send command to write the register. */
	error = gbeeSendAtCommand(gbee, GBEE_UTIL_DEFAULT_FRAME_ID, (uint8_t*)regName,
			(uint8_t*)value, length);
	GBEE_THROW(error);
	
	/* Receive the response. */
	timeout = 1000;
	do
	{
		error = gbeeReceive(gbee, (GBeeFrameData *)&atCommandResponse, &responseLength, 
				&timeout);
	}
	while ((error == GBEE_NO_ERROR) && 
	       (atCommandResponse.ident != GBEE_AT_COMMAND_RESPONSE));
	GBEE_THROW(error);
	
	/* Check the response. */
	if ((atCommandResponse.atCommand[0] != regName[0]) || 
	    (atCommandResponse.atCommand[1] != regName[1]) ||
	    (atCommandResponse.status != GBEE_AT_COMMAND_STATUS_OK))
	{
		GBEE_THROW(GBEE_RESPONSE_ERROR);
	}
	
	return GBEE_NO_ERROR;
}

/******************************************************************************/

GBeeError gbeeUtilReadRegister(GBee *gbee, const char *regName, uint8_t *value,
		uint16_t *length, uint16_t maxLength)
{
	/* Error code returned by the GBee driver. */
	GBeeError error;
	/* GBee response to AT commands. */
	GBeeAtCommandResponse atCommandResponse;
	/* Length of AT command response. */
	uint16_t responseLength;
	/* Timeout in milliseconds. */
	uint32_t timeout;
	
	/* Query the GBee for the given register. */
	error = gbeeSendAtCommand(gbee, GBEE_UTIL_DEFAULT_FRAME_ID, (uint8_t *)regName, 
			NULL, 0);
	GBEE_THROW(error);

	/* Receive the response. */
	timeout = 1000;
	do
	{
		error = gbeeReceive(gbee, (GBeeFrameData *)&atCommandResponse, &responseLength, 
				&timeout);
	}
	while ((error == GBEE_NO_ERROR) 
			&& (atCommandResponse.ident != GBEE_AT_COMMAND_RESPONSE));
	GBEE_THROW(error);
	
	responseLength -= GBEE_AT_COMMAND_RESPONSE_HEADER_LENGTH;
	
	/* Check the response. */
	if ((atCommandResponse.atCommand[0] != regName[0]) 
			|| (atCommandResponse.atCommand[1] != regName[1]) 
			|| (atCommandResponse.status != GBEE_AT_COMMAND_STATUS_OK) 
			|| (maxLength < responseLength))
	{
		GBEE_THROW(GBEE_RESPONSE_ERROR);
	}
	
	GBEE_PORT_MEMORY_COPY(value, atCommandResponse.value, responseLength);
	return GBEE_NO_ERROR;
}

/******************************************************************************/

bool gbeeUtilEncodeUdp(const uint8_t *payload, uint16_t payloadLength,
		uint16_t fromPort, const GBeeSockAddr *toAddr,
		GBeeTxRequest16 *txRequest, uint16_t *totalLength)
{
	/* Check the length of the payload. */
	if ((payloadLength + sizeof(UdpHeader)) > GBEE_MAX_PAYLOAD_LENGTH)
	{
		return false;
	}

	/* Create the UDP header. */
	UdpHeader *udpHeader = (UdpHeader *)txRequest->data;
	udpHeader->fromPort  = GBEE_USHORT(fromPort);
	udpHeader->toPort    = GBEE_USHORT(toAddr->port);
	udpHeader->length    = GBEE_USHORT(payloadLength + sizeof(UdpHeader));
	udpHeader->checksum  = 0;

	/* Get the payload. */
	GBEE_PORT_MEMORY_COPY(txRequest->data + sizeof(UdpHeader), payload, payloadLength);

	/* Create the Tx Request header. */
	txRequest->ident     = GBEE_TX_REQUEST_16;
	txRequest->frameId   = GBEE_UTIL_DEFAULT_FRAME_ID;
	txRequest->dstAddr16 = GBEE_USHORT(toAddr->addr);
	txRequest->options   = 0;

	/* Calculate total frame length and we're done. */
	*totalLength = payloadLength + sizeof(UdpHeader) + GBEE_TX_REQUEST_16_HEADER_LENGTH;
	return true;
}

/******************************************************************************/

bool gbeeUtilDecodeUdp(GBeeRxPacket16 *frame, uint16_t length, uint8_t **payload,
		uint16_t *payloadLength, GBeeSockAddr *fromAddr)
{
	/* Check if the frame contains data with 16 bit address. */
	if (frame->ident != GBEE_RX_PACKET_16)
	{
		/* Drop any packet, except Rx data with 16 bit address. */
		return false;
	}

	/* Get the UDP header. */
	UdpHeader *udpHeader = (UdpHeader *)(frame->data);

	/* Check if the UDP header is valid. */
	if (length < (sizeof(UdpHeader) + GBEE_RX_PACKET_16_HEADER_LENGTH))
	{
		return false;
	}
	if ((GBEE_USHORT(udpHeader->length) + GBEE_RX_PACKET_16_HEADER_LENGTH) != length)
	{
		return false;
	}
	
	/* Get payload and payload length. */
	*payload = frame->data + sizeof(UdpHeader);
	*payloadLength = length - (sizeof(UdpHeader) + GBEE_RX_PACKET_16_HEADER_LENGTH);

	/* Get the address packet is received from. */
	fromAddr->port = GBEE_USHORT(udpHeader->fromPort);
	fromAddr->addr = GBEE_USHORT(frame->srcAddr16);
	return true;
}

/******************************************************************************/

const char *gbeeUtilCodeToString(GBeeError code)
{
	switch (code)
	{
		case GBEE_NO_ERROR:
			return "OK";
		case GBEE_INHERITED_ERROR:
			return "INHERITED ERROR";
		case GBEE_RS232_ERROR:
			return "RS232 FAILURE";
		case GBEE_CHECKSUM_ERROR:
			return "CHECKSUM FAILURE";
		case GBEE_FRAME_SIZE_ERROR:
			return "MAX FRAME SIZE EXCEEDED";
		case GBEE_FRAME_INTEGRITY_ERROR:
			return "INCOMPLETE FRAME";
		case GBEE_FRAME_START_ERROR:
			return "UNEXPECTED START DELIMITER";
		case GBEE_MODE_ERROR:
			return "INVALID MODE";
		case GBEE_RESPONSE_ERROR:
			return "UNEXPECTED RESPONSE";
		case GBEE_TIMEOUT_ERROR:
			return "TIMEOUT";
		default:
			return "UNKNOWN ERROR";
	};
}
