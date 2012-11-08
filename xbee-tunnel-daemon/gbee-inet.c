/**
 * \file
 * \author  d264
 * \version $Rev$
 *
 * \section DESCRIPTION
 *
 * Helper functions and structs for internet protocols we want to use with our
 * XBee.
 *
 * \section LICENSE
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "gbee-inet.h"
#include <netinet/in.h>

/**
 * Calculate IP header checksum.
 *
 * \param[in] ipHeader is the IP header.
 *
 * \return The 16bit checksum.
 */
static uint16_t gbeeInetChecksumGet(const IpHeader *ipHeader);

/*****************************************************************************/

bool gbeeInetDecode(IpHeader *ipHeader, uint32_t packetLength,
		GBeeTxRequest16 *txRequest,	uint16_t *txRequestLength)
{
	/* Ensure this is a UDP packet. */
	if (ipHeader->protocol != IP_PROTOCOL_UDP)
	{
		return false;
	}

	/* Size of the IP header. */
	uint32_t ipHeaderLength = 4 * ipHeader->headerLength;

	/* Get a pointer to the UDP header. */
	UdpHeader *udpHeader = (UdpHeader *)(((uint8_t *)ipHeader) + ipHeaderLength);

	/* Assemble the GBee Tx request. */
	*txRequestLength     = packetLength - ipHeaderLength;
	txRequest->ident     = GBEE_TX_REQUEST_16;
	txRequest->frameId   = 0xAA;
	txRequest->dstAddr16 = GBEE_USHORT(ntohl(ipHeader->destAddress) & 0xFFFF);
	txRequest->options   = 0;
	memcpy(txRequest->data, (uint8_t *)udpHeader, *txRequestLength);

	/* Length of the Tx request includes the header. */
	*txRequestLength += GBEE_TX_REQUEST_16_HEADER_LENGTH;

	/* Decode complete. */
	return true;
}

/*****************************************************************************/

bool gbeeInetEncode(uint32_t destAddr, const GBeeRxPacket16 *gbeePacket,
		uint16_t gbeePacketLength, IpHeader *ipHeader)
{
	/* Create the IP header. */
	ipHeader->version      = 4;
	ipHeader->headerLength = 5;

	/* Calculate length of IP packet. */
	uint16_t ipPacketLength = gbeePacketLength - GBEE_RX_PACKET_16_HEADER_LENGTH
	                        + ipHeader->headerLength * 4;

	/* Continue filling the IP header. */
	ipHeader->typeOfService = 0;
	ipHeader->totalLength   = htons(ipPacketLength);
	ipHeader->id            = 42;
	ipHeader->fragOffset    = 0;
	ipHeader->timeToLive    = 64;
	ipHeader->protocol      = IP_PROTOCOL_UDP;
	ipHeader->checksum      = 0;
	ipHeader->sourceAddress = htonl((destAddr & 0xFFFF0000) |
			                   GBEE_USHORT(gbeePacket->srcAddr16));
	ipHeader->destAddress   = htonl(destAddr);

	/* Get the header checksum. */
	ipHeader->checksum = gbeeInetChecksumGet(ipHeader);

	/* Finally, copy the data and we're done. */
	memcpy(((uint8_t *)ipHeader) + sizeof(IpHeader), gbeePacket->data,
			gbeePacketLength);
	return true;
}

/*****************************************************************************/

static uint16_t gbeeInetChecksumGet(const IpHeader *ipHeader)
{
	register uint32_t sum = 0;
	uint16_t answer = 0;
	register uint16_t *w = (uint16_t *)ipHeader;
	register int nleft = ipHeader->headerLength * 4;

	while (nleft > 1)
	{
		sum += *w++;
		nleft -= 2;
	}

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	answer = ~sum;
	return(answer);
}
