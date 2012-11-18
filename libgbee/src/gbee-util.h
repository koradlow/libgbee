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

#ifdef __cplusplus
extern "C"{
#endif 

#ifndef GBEE_UTIL_H_INCLUDED
#define GBEE_UTIL_H_INCLUDED

#include "gbee.h"

/**
 * Converts a GBee error code to ASCII string.
 * 
 * \param[in] code specifies the error code.
 * 
 * \return A pointer to an ASCII string for the specified error code or
 * "GBEE UNKNOWN ERROR" if the error code is invalid.
 */
const char *gbeeUtilCodeToString(GBeeError code);

/**
 * Converts a XBee Tx status code to ASCII string.
 * 
 * \param[in] code specifies the Tx status code.
 * 
 * \return A pointer to an ASCII string for the specified Tx status code
 */
const char *gbeeUtilTxStatusCodeToString(uint8_t code);

/**
 * Converts a XBee status code to ASCII string.
 * 
 * \param[in] code specifies the Status code.
 * 
 * \return A pointer to an ASCII string for the specified Status code
 */
const char *gbeeUtilStatusCodeToString(uint8_t code);

#endif /*GBEE_UTIL_H_INCLUDED*/

#ifdef __cplusplus
}
#endif
