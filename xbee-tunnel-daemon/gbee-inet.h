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

#ifndef INET_H_INCLUDED
#define INET_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include "gbee.h"
#include "gbee-util.h"

/** Defines an IP header. */
struct __attribute__((__packed__)) IpHeader {
	uint8_t  headerLength : 4; /**< IP header length */
	uint8_t  version      : 4; /**< IP version */
	uint8_t  typeOfService;    /**< Type of service */
	uint16_t totalLength;      /**< Total length of packet */
	uint16_t id;               /**< Unique identifier */
	uint16_t fragOffset;       /**< Fragment offset */
	uint8_t  timeToLive;       /**< IP packet time-to-live value */
	uint8_t  protocol;         /**< Higher-level protocol */
	uint16_t checksum;         /**< IP checksum */
	uint32_t sourceAddress;    /**< IP source address */
	uint32_t destAddress;      /**< IP destination address */
};

/** Type definition for IP header. */
typedef struct IpHeader IpHeader;

#define IP_PROTOCOL_UDP 17 /**< UDP protocol */

/** Defines an UDP header. */
struct __attribute__((__packed__)) UdpHeader {
	uint16_t sourcePort;  /**< Source port number */
	uint16_t destPort;    /**< Destination port number */
	uint16_t length;      /**< Length of UDP packet */
	uint16_t checksum;    /**< Checksum of UDP packet */
};

/** Type definition for UDP header. */
typedef struct UdpHeader UdpHeader;

/**
 * Decode the UDP/IP packet and create an appropriate GBee Tx request.
 *
 * \param[in] ipHeader is a pointer to the IP header of the packet.
 * \param[in] packetLength indicated by the TUN device.
 * \param[out] txRequest is the GBee Tx request.
 * \param[out] txRequestLength is the length of the created Tx request.
 *
 * \return true if successful, or false in case of any error.
 */
bool gbeeInetDecode(IpHeader *ipHeader, uint32_t packetLength,
		GBeeTxRequest16 *txRequest,	uint16_t *txRequestLength);

/**
 * Encode an UDP/IP packet using the data given by the GBee Rx packet.
 *
 * \param[in] destAddr is the host where to send the packet to.
 * \param[in] gbeePacket is the GBee Rx packet.
 * \param[in] gbeePacketLength is the length of the packet in bytes.
 * \param[out] ipHeader points to a memory location where to store the IP
 * 		packet.
 *
 * \return true if successfull, false in case of any error.
 */
bool gbeeInetEncode(uint32_t destAddr, const GBeeRxPacket16 *gbeePacket,
		uint16_t gbeePacketLength, IpHeader *ipHeader);

#endif /* INET_H_INCLUDED */
