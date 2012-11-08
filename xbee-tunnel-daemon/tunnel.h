/**
 * \file
 * \author  d264
 * \version $Rev$
 *
 * \section DESCRIPTION
 *
 * This defines the interface for the OS-dependant implementation of the tunnel
 * daemon.
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

#ifndef TUNNEL_H_INCLUDED
#define TUNNEL_H_INCLUDED

#include "gbee-inet.h"
#include <semaphore.h>
#include <stdint.h>
#include <stdbool.h>

/** This is our tunnel. */
struct Tunnel {
	GBee      *gbeeDevice;   /**< The GBee device driver instance. */
	uint16_t   gbeeAddr;     /**< XBee address of the tunnel. */
	uint16_t   gbeePan;      /**< XBee PAN identifier. */
	sem_t      txStatusLock; /**< XBee transmission status flag. */
	int        tunDevice;    /**< TUN device file descriptor. */
	uint32_t   inetAddr;     /**< IP address of the tunnel. */
};

/** Tunnel type definition. */
typedef struct Tunnel Tunnel;

/**
 * Initializes the Xbee device driver and sets the Xbee into API mode.
 * Configures the TUN device with the desired IP address.
 *
 * \param[in] serialDevice is the name of the serial device the Xbee is
 * 		connected to, e.g. ``/dev/ttyS0''.
 * \param[in] inetAddr is a string with the IP address.
 *
 * \return A pointer to the Tunnel if successful, NULL in case of any error.
 */
Tunnel *tunnelInit(const char* serialDevice, const char* inetAddr);

/**
 * Closes the tunnel by destroying the GBee device and closing the TUN/TAP 
 * device.
 *
 * \param[in] self is a pointer to the tunnel to exit.
 */
void tunnelExit(Tunnel *self);

/**
 * Receives an UDP/IP packet from the TUN device and decodes it into an
 * appropriate GBee Tx request.
 *
 * \param[in] self is a pointer to the tunnel.
 * \param[out] txRequest is the GBee transmit request.
 * \param[out] txRequestLength is the length of the transmit request in bytes.
 *
 * \return true to indicate success, false in case of any error.
 */
bool tunnelInetReceive(Tunnel *self, GBeeTxRequest16 *txRequest, uint16_t *txRequestLength);

/**
 * Encodes a GBee Rx packet into an appropriate UDP/IP packet and writes it to
 * the TUN device.
 *
 * \param[in] self is a pointer to the tunnel.
 * \param[in] rxPacket is a pointer to the GBee Rx packet.
 * \param[in] rxPacketLength is the length of the packet in bytes.
 *
 * \return true to indicate success, false in case of any error.
 */
bool tunnelInetSend(Tunnel *self, GBeeRxPacket16 *rxPacket, uint16_t rxPacketLength);

/**
 * Receives data from the GBee device driver. Blocks until a GBee Rx packet
 * with 16bit address is available.
 *
 * \param[in] self is a pointer to the tunnel.
 * \param[out] frame is the received packet.
 * \param[out] frameLength is the length of the received frame in bytes.
 *
 * \return true to indicate success, false in case of any error.
 */
bool tunnelGBeeReceive(Tunnel *self, GBeeFrameData *frame, uint16_t *frameLength);

/**
 * Sends the given GBee Tx request to the XBee device.
 *
 * \param[in] self is a pointer to the tunnel device.
 * \param[in] txRequest is a pointer to the Tx request.
 * \param[in] txRequestLength is the length of the Tx request in bytes.
 *
 * \return true to indicate success, false in case of any error.
 */
bool tunnelGBeeSend(Tunnel *self, GBeeTxRequest16 *txRequest, uint16_t txRequestLength);

#endif /* TUNNEL_H_INCLUDED */
