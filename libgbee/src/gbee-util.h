/**
 * \page utility_functions Utility Functions
 * \section utility_functions Utility Functions
 *
 * For convenience the libgbee provides some utility functions, e.g. for
 * accessing XBee registers or setting the XBee addresses. The following utility
 * functions are declared in gbee-util.h.
 *
 * \subsection get_set_xbee_address Get/Set XBee Address
 *
 * You can set the 16-bit address of the XBee module by calling the function
 * gbeeUtilSetAddress16(). To query the current 16-bit address of the XBee
 * module you can call the function gbeeUtilGetAddress16().
 *
 * \subsection xbee_register_access XBee Register Access
 *
 * To set any register of the XBee module you can call the function
 * gbeeUtilWriteRegister(). To query the value of a register of the XBee module
 * you can call the function gbeeUtilReadRegister().
 *
 * \subsection xbee_error_string_conversion XBee Error to String Conversion
 *
 * The utility function gbeeUtilCodeToString() converts an of the ::GBeeError
 * codes into a human readable string.
 *
 * \subsection xbee_udp_tunneling XBee UDP Tunneling
 *
 * The GBee utilities provide a very basic UDP implementation, allowing to
 * tunnel UDP packets over your XBee PAN. This is useful if you use the libgbee
 * on a small embedded system and want to exchange data with a remote XBee
 * module operated by the XBee Tunnel Daemon.
 *
 * To encode a XBee data frame into an UDP packet the GBee utilities provide
 * the function gbeeUtilEncodeUdp(). To decode a received UDP packet containing
 * an XBee frame, there is the function gbeeUtilDecodeUdp().
 *
 * For more information how to do UDP tunneling with the libgbee, please refer
 * to the "XBee Tunnel Daemon Reference Manual" located in
 * xbee-tunnel-daemon/doc. If you need an example how to do UDP tunneling with
 * the libgbee you can look at the source code of the XBee Echo Server, see
 * xbee-echo-server/server.c.
 *
 * \file
 * \author  d264
 * \version $Rev$
 *
 * \section DESCRIPTION
 *
 * The gbee-util module provides various utility functions for the libgbee.
 *
 * \section TODO
 *
 * Add Functions to get and set the 64-bit address of the XBee module.
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

#ifndef GBEE_UTIL_H_INCLUDED
#define GBEE_UTIL_H_INCLUDED

#include "gbee.h"

/** GBee UDP socket address. */
struct __attribute__((__packed__)) GBeeSockAddr {
	uint16_t port; /**< UDP port number. */
	uint16_t addr; /**< 16bit XBee address. */
};

/** Type definition for GBee UDP socket address. */
typedef struct GBeeSockAddr GBeeSockAddr;

/**
 * Sets the XBee's 16-bit address and PAN ID.
 * The XBee has to be in API mode when this function is called!
 * 
 * \param[in] gbee is a pointer to the GBee driver object.
 * \param[in] addr is the 16-bit address to set.
 * \param[in] pan is the 16-bit PAN ID to set.
 * 
 * \return GBEE_NO_ERROR if successfull or dedicated errror code in case of an
 * error.
 */
GBeeError gbeeUtilSetAddress16(GBee *gbee, uint16_t addr, uint16_t pan);

/**-
 * Gets the XBee's 16-bit address and PAN ID. 
 * The XBee has to be in API mode when this function is called!
 * 
 * \param[in] gbee is a pointer to the GBee driver object.
 * \param[out] addr contains the GBee's 16-bit address.
 * \param[out] pan contains the GBee's 16-bit PAN ID.
 * 
 * \return GBEE_NO_ERROR if successfull or dedicated errror code in case of an
 * error.
 */
GBeeError gbeeUtilGetAddress16(GBee *gbee, uint16_t *addr, uint16_t *pan);

/**
 * Writes data to a XBee register.
 * The XBee has to be in API mode when this function is called!
 * 
 * \param[in] gbee is a pointer to the GBee driver object.
 * \param[in] regName specifies the name of the register.
 * \param[in] value points to the data to write to the register. Remember to
 *            convert endianess if you write a multi-byte value.
 * \param[in] length specifies the length of the data to write.
 * 
 * \return GBEE_NO_ERROR if successful, or dedicated error code in case of an
 * error.
 */
GBeeError gbeeUtilWriteRegister(GBee *gbee, const char *regName, const uint8_t *value,
		uint16_t length);

/**
 * Reads data from a XBee register.
 * The XBee has to be in API mode when this function is called!
 * 
 * \param[in] gbee is a pointer to the GBee driver object.
 * \param[in] regName specifies the name of the register.
 * \param[in] value points to the data to write to the register. Remember to
 *            convert endianess if you read a multi-byte value.
 * \param[out] length specifies the length of the data read from the register.
 * \param[in] maxLength specifies the maximum number of bytes to read.
 * 
 * \return GBEE_NO_ERROR if successful, or dedicated error code in case of an
 * error.
 */
GBeeError gbeeUtilReadRegister(GBee *gbee, const char *regName, uint8_t *value,
		uint16_t *length, uint16_t maxLength);

/**
 * Encode the given payload into a GBee UDP frame.
 * 
 * \param[in] payload is a pointer to the UDP payload.
 * \param[in] payloadLength is the length of the payload.
 * \param[in] fromPort is the originating port number.
 * \param[in] toAddr is the port and 16bit address of the remote host.
 * \param[out] txRequest is the encoded UDP frame.
 * \param[out] totalLength is the total frame length.
 * 
 * \return true if successful, false if payload length exceeds 92 bytes.
 */
bool gbeeUtilEncodeUdp(const uint8_t *payload, uint16_t payloadLength,
		uint16_t fromPort, const GBeeSockAddr *toAddr,
		GBeeTxRequest16 *txRequest, uint16_t *totalLength);

/**
 * Check if the given GBee frame data contains a UDP packet and decode the UDP
 * and payload data.
 * 
 * \param[in] frame is a pointer to the raw frame received from the XBee.
 * \param[in] length is the length of the frame in bytes.
 * \param[out] payload is set to the start of the UDP payload.
 * \param[out] payloadLength is set to the length of the UDP payload.
 * \param[out] fromAddr is set to the port and 16bit address the frame is from.
 *
 * \return true if the frame contains a valid UDP packet, false if not.
 */
bool gbeeUtilDecodeUdp(GBeeRxPacket16 *frame, uint16_t length, uint8_t **payload,
		uint16_t *payloadLength, GBeeSockAddr *fromAddr);

/**
 * Converts a GBee error code to ASCII string.
 * 
 * \param[in] code specifies the error code.
 * 
 * \return A pointer to an ASCII string for the specified error code or
 * "GBEE UNKNOWN ERROR" if the error code is invalid.
 */
const char *gbeeUtilCodeToString(GBeeError code);

#endif /*GBEE_UTIL_H_INCLUDED*/
