/**
 * \file
 * \author  d264
 * \version $Rev$
 *
 * \section DESCRIPTION
 *
 * This is the Linux-specific implementation of the XBee tunnel device.
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

#include "tunnel.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <syslog.h>

/*****************************************************************************/

Tunnel *tunnelInit(const char *serialDevice, const char* inetAddr)
{
	/* This is our tunnel instance. */
	static Tunnel tunnel;
    /* Mode Xbee is operating in. */
	GBeeMode mode;
    /* Error code returned by Xbee. */
	GBeeError error;

	/* Create the Xbee driver instance. */
	tunnel.gbeeDevice = gbeeCreate(serialDevice);
	if (tunnel.gbeeDevice == NULL)
	{
		syslog(LOG_ERR, "XBee error: failed to connect to XBee");
		return NULL;
	}

	/* Get Xbee mode. */
	error = gbeeGetMode(tunnel.gbeeDevice, &mode);
	if (error != GBEE_NO_ERROR)
	{
		syslog(LOG_ERR, "XBee error: failed to get mode");
		return NULL;
	}

	/* If XBee is not operating in API, then set API mode. */
	if (mode != GBEE_MODE_API)
	{
		error = gbeeSetMode(tunnel.gbeeDevice, GBEE_MODE_API);
		if (error != GBEE_NO_ERROR)
		{
			syslog(LOG_ERR, "XBee error: failed to set mode");
			return NULL;
		}

	}

	/* Set the XBee 16bit address. */
	tunnel.inetAddr = ntohl(inet_addr(inetAddr));
	tunnel.gbeeAddr = tunnel.inetAddr & 0xFFFF;
	tunnel.gbeePan  = tunnel.inetAddr >> 16;
	error = gbeeUtilSetAddress16(tunnel.gbeeDevice, tunnel.gbeeAddr, tunnel.gbeePan);
	if (error != GBEE_NO_ERROR)
	{
		syslog(LOG_ERR, "XBee error: failed to set address");
		return NULL;
	}
	
	/* For configuring the TUN device. */
	struct ifreq request;
	/* System command for configuring the network interface. */
	char shellCommand[80];

	/* Get a handle for the TUN device. */
	tunnel.tunDevice = open("/dev/net/tun", O_RDWR);
	if (tunnel.tunDevice == -1)
	{
		syslog(LOG_ERR, "TUN/TAP error: failed to open device /dev/net/tun");
		return NULL;
	}

	/* Configure the TUN device. */
	memset(&request, 0, sizeof(request));
	request.ifr_flags = IFF_TUN | IFF_NO_PI;
	if (ioctl(tunnel.tunDevice, TUNSETIFF, (void *)&request) < 0)
	{
		syslog(LOG_ERR, "TUN/TAP error: failed to configure /dev/net/tun");
		return NULL;
	}

	/* Configure the network interface. */
	sprintf(shellCommand, "ifconfig tun0 inet %s netmask 255.255.0.0", inetAddr);
	system(shellCommand);

	/* Create the semaphore for getting transmission status. */
	if (sem_init(&tunnel.txStatusLock, 0, 0) != 0)
	{
		syslog(LOG_ERR, "Internal error: failed to create semaphore");
		return NULL;
	}

	return &tunnel;
}

/*****************************************************************************/

void tunnelExit(Tunnel *self)
{
	/* Destroy the GBee device. */
	gbeeDestroy(self->gbeeDevice);
	/* Close the TAP/TUN device. */
	close(self->tunDevice);
}

/*****************************************************************************/

bool tunnelInetReceive(Tunnel *self, GBeeTxRequest16 *txRequest, uint16_t *txRequestLength)
{
	/* The IP packet. */
	static uint8_t buffer[576];

	/* Wait for data from the TUN device. */
	int result = read(self->tunDevice, buffer, sizeof(buffer));
	if (result == -1)
	{
		syslog(LOG_ERR, "TUN/TAP error: failed to read from /dev/net/tun");
		return false;
	}

	/* Decode the IP packet. */
	if (!gbeeInetDecode((IpHeader *)buffer, result, txRequest, txRequestLength))
	{
		syslog(LOG_ERR, "Protocol error: failed to decode packet");
		return false;
	}

	return true;
}

/*****************************************************************************/

bool tunnelInetSend(Tunnel *self, GBeeRxPacket16 *rxPacket, uint16_t rxPacketLength)
{
	/* The IP packet. */
	static uint8_t buffer[576];
	IpHeader *ipHeader = (IpHeader *)buffer;

	/* Encode the IP packet. */
	gbeeInetEncode(self->inetAddr, rxPacket, rxPacketLength, ipHeader);

	/* Forward the packet data to the TUN device. */
	ssize_t result = write(self->tunDevice, ipHeader, ntohs(ipHeader->totalLength));
	if (result < 0)
	{
		syslog(LOG_ERR, "TUN/TAP error: failed to write to /dev/net/tun");
		return false;
	}
	return true;
}

/*****************************************************************************/

bool tunnelGBeeReceive(Tunnel *self, GBeeFrameData *frame, uint16_t *frameLength)
{
	/* Infinite loop for receiving data. */
	while (1)
	{
		/* Wait until we receive a packet from the GBee device driver. */
		uint32_t timeout = GBEE_INFINITE_WAIT;
		GBeeError error = gbeeReceive(self->gbeeDevice, frame, frameLength, &timeout);
		if (error != GBEE_NO_ERROR)
		{
			syslog(LOG_ERR, "XBee error: failed to receive data from XBee");
			return false;
		}

		/* Process the packet. */
		if (frame->ident == GBEE_RX_PACKET_16)
		{
			return true;
		}
		else if (frame->ident == GBEE_TX_STATUS)
		{
			/* Received a XBee transmission status packet. Signal "Tx Status
			 * Received" to the transmitter. */
			sem_post(&self->txStatusLock);
		}
		else
		{
			/* Received any other packet -> ignore it. */
			syslog(LOG_WARNING, "Discarded XBee packet (ident=%d)", frame->ident);
		}
	}
	return true;
}

/*****************************************************************************/

bool tunnelGBeeSend(Tunnel *self, GBeeTxRequest16 *txRequest, uint16_t txRequestLength)
{
	/* Send the IP packet to the remote XBee. */
	uint32_t error = gbeeSend(self->gbeeDevice, (GBeeFrameData *)txRequest,	txRequestLength);
	if (error != GBEE_NO_ERROR)
	{
		syslog(LOG_ERR, "XBee error: failed to send data to XBee");
		return false;
	}

	/* Wait for the transmission status. */
	sem_wait(&self->txStatusLock);
	return true;
}
