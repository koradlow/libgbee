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

/******************************************************************************/

const char *gbeeUtilTxStatusCodeToString(uint8_t code)
{
	switch (code)
	{
		case 0x00:
			return "Success";
		case 0x01:
			return "MAC ACK Failure";
		case 0x02:
			return "CCA Failure";
		case 0x15:
			return "Invalid destination endpoint";
		case 0x21:
			return "Network ACK Failure";
		case 0x22:
			return "Not Joined to Network";
		case 0x23:
			return "Self-addressed";
		case 0x24:
			return "Address Not Found";
		case 0x25:
			return "Route Not Found";
		case 0x26:
			return "Failed to hear neighbor relay";
		case 0x2B:
			return "Invalid binding table index";
		case 0x2C:
			return "Resource error";
		case 0x2E:
			return "Attempted Bcast with APS transm.";
		case 0x32:
			return "Resource error";
		case 0x74:
			return "Payload too large";
		case 0x75:
			return "Indirect message unrequested";		
		default:
			return "Unknown Tx Status";
	};
}

/******************************************************************************/

const char *gbeeUtilStatusCodeToString(uint8_t code)
{
	switch (code)
	{
		case 0x00:
			return "Successfully formed or joined a network";
		case 0x21:
			return "Scan found no PANs";
		case 0x22:
			return "Scan found no valid PANs based on current SC and ID settings";
		case 0x23:
			return "Valid Coordinator or Routers found, but NJ expired";
		case 0x24:
			return "No joinable beacons were found";
		case 0x25:
			return "Unexpected state, node should not be attempting to join at this time";
		case 0x27:
			return "Node Joining attempt failed (security settings?)";
		case 0x2A:
			return "Coordinator Start attempt failed";
		case 0x2B:
			return "Checking for an existing coordinator";
		case 0x2C:
			return "Attempt to leave the network failed";
		case 0xAB:
			return "Attempted to join a device that did not respond";
		case 0xAC:
			return "Secure join error - network security key received unsecured";
		case 0xAD:
			return "Secure join error - network security key not received";
		case 0xAF:
			return "Secure join error - joining device does not have the right preconfigured link key";
		case 0xFF:
			return "Scanning for a ZigBee network";
		default:
			return "Unknown Status";
	};
}
