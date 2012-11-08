/**
 * \mainpage
 *
 * The XBee-Tunnel-Daemon is a user space background daemon, that creates a
 * virtual network interface (or more precisely: a TUN device) allowing to
 * transmit UDP/IP packets over the XBee module.
 *
 * This allows to do XBee communication using a normal socket interface.
 *
 * \section invoking_the_xbee_tunnel_daemon Invoking the XBee-Tunnel-Daemon
 *
 * Do not call the XBee-Tunnel-Daemon directly. Instead use the
 * \a xbee-tunnel-control script with the \a start option to start the daemon
 * or with the \a stop option to stop the daemon. Note, that you will need
 * \a root privileges to start the XBee-Tunnel-Daemon.
 *
 * The XBee-Tunnel-Daemon needs to be configured with the following parameters:
 *
 * <table>
 * <tr>
 * <td>-i, --inet</td>
 * <td>IP address to assign to the network interface, e.g. \a 10.10.123.123.
 * Note, that the XBee-Tunnel-Daemon uses a /16 network (netmask 255.255.0.0).
 * The network part of the IP address is used as PAN ID of the XBee module and
 * the host part of the IP address is used as the XBee module's 16-bit address.
 * </td>
 * </tr>
 * <tr>
 * <td>-s, --serial</td>
 * <td>Name of the serial interface the XBee module is connected to, e.g.
 * \a /dev/ttyUSB0. </td>
 * </tr>
 * <tr>
 * <td>-v, --verbose</td>
 * <td>Optional. Enables verbose output; note, that the XBee-Tunnel-Daemon logs
 * all output to the syslog.</td>
 * </tr>
 * </table>
 *
 * You can write these parameters into a configuration file named
 * \a xbee-tunnel-rc and put this configuration file either in your \a home
 * directory or into the XBee-Tunnel-Daemon's working directory. You can create
 * the configuration file with the following command:
 * \code
 * echo "--inet=$IP_ADDRESS --serial=$SERIAL_DEVICE_NAME" > .xbee-tunnel-rc
 * \endcode
 *
 * For more information on UDP/IP tunneling through your XBee module, please
 * refer to \ref udp_ip_tunneling_details.
 *
 * For instructions to build the XBee-Tunnel-Daemon, please refer to
 * \ref build_instructions.
 *
 * \section sending_and_receiving_data Sending and Receiving Data
 *
 * The XBee-Tunnel-Daemon allows to use a normal socket interface to send and
 * receive data in your application. So first, create a socket with the
 * following parameters:
 * \code
 * s = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
 * \endcode
 *
 * Then initialize a socket address structure with the address of your local
 * host:
 * \code
 * struct sockaddr_in host;
 * localHost.sin_family = AF_INET;
 * localHost.sin_addr.s_addr = $YOUR_IP_ADDRESS
 * localHost.sin_port = $DESTINATION_PORT_NUMBER
 * \endcode
 *
 * Now send the data to the remote host with:
 * \code
 * sendto(s, dataPtr, dataLength, 0, &localHost, sizeof(localHost));
 * \endcode
 *
 * To receive data from the XBee module, call:
 * \code
 * length = recvfrom(s, dataPtr, maxLength, 0, &remoteHost, &hostLength);
 * \endcode
 *
 * Note, as of yet there are still some limitations regarding UDP/IP tunneling
 * through your XBee. Refer to \ref limitations to get an overview.
 *
 * \page udp_ip_tunneling_details UDP/IP Tunneling Details
 *
 * The XBee-Tunnel-Daemon creates a virtual network device, a \a TUN device,
 * which allows to receive IP packets from or send IP packets to the operating
 * system's TCP/IP stack. When a data packet is received from the XBee (serial
 * interface), the XBee-Tunnel-Daemon generates an appropriate IP packet and
 * forwards it to the TCP/IP stack. If an IP packet is received from the TCP/IP
 * stack, the XBee-Tunnel-Daemon generates an appropriate XBee Tx Request and
 * sends the data to the remote XBee.
 *
 * \section data_transport Data Transport
 * \subsection from_host_to_xbee From Host to XBee
 *
 * When the XBee-Tunnel-Daemon receives an IP packet from the operating system's
 * TCP/IP stack, it first checks if the IP packet contains UDP data. If the
 * packet does not contain UDP data, then the packet is discarded.
 *
 * If the IP packet contains UDP data, the XBee-Tunnel-Daemon extracts the UDP
 * payload from the IP packet and sends it to the remote XBee. So only the UDP
 * payload is send over the air, without the IP header.
 *
 * \subsection from_xbee_to_host From XBee to Host
 *
 * When the XBee-Tunnel-Daemon receives a packet from a remote XBee, it first
 * checks if the packet contains UDP payload. If the packet does not contain
 * UDP payload, then the packet is discarded.
 *
 * If the packet contains UDP payload, the XBee-Tunnel-Daemon creates an
 * appropriate IP packet for this payload and forwards it to the operating
 * system's TCP/IP stack.
 *
 * \section addressing Addressing
 *
 * The XBee-Tunnel-Daemon creates a network interface operating a /16
 * network (this is netmask 255.255.0.0).
 *
 * If a packet is read from the TCP/IP stack and transmitted to a remote XBee,
 * the XBee-Tunnel-Daemon converts the IP address as follows:
 * <ul>
 * <li>the network part of the IP address gives the PAN ID, and
 * <li>the host part of the IP address gives the remote XBee's 16-bit address.
 * </ul>
 *
 * If a packet is received from a remote XBee and forwarded to the TCP/IP stack,
 * the XBee-Tunnel-Daemon generates the IP address as follows:
 * <ul>
 * <li>the PAN-ID gives the network part of the IP address, and
 * <li>the 16-bit destination address gives the host part of the IP address.
 * </ul>
 *
 * \page build_instructions Build Instructions
 *
 * The XBee-Tunnel-Daemon's build system is also based on CMake, although the
 * only platform supported so far is GNU/Linux. Before starting CMake first
 * create the directory where you want to make the build, e.g.
 * \code
 * ~/xbee/build/linux/x86/xbee-tunnel-daemon
 * \endcode
 *
 * Browse to this directory in a terminal and run cmake-gui. When being asked
 * for the generator to use for this build, select <i>Use default native
 * compilers</i>. The following options are available for building the XBee-
 * Tunnel-Daemon:
 *
 * <table>
 * <tr>
 * <td>LIBGBEE</td>
 * <td>The XBee-Tunnel-Daemon uses the LibGBee, so specify the path to your
 * LibGBee here, e.g. \a ~/xbee/build/linux/x86/libgbee/libgbee-x86-linux.a.
 * </td>
 * </tr>
 * <tr>
 * <td>LIBGBEE_INCLUDE_PATH</td>
 * <td>Specify the path to the LibGBee include files (i.e. the path to gbee.h),
 * e.g. \a ~/xbee/libgbee/src/ </td>
 * </tr>
 * <tr>
 * <td>CMAKE_INSTALL_PREFIX</td>
 * <td>Selects the installation prefix for the library. The default value is
 * \a /usr/local which is suitable for a Linux / x86 build. </td>
 * </tr>
 * </table>
 *
 * After configuration of the build system, you can build the XBee-Tunnel-
 * Daemon by running \a make in your build directory.
 *
 * \page limitations Limitations
 *
 * Currently, the XBee-Tunnel-Daemon has some limitations:
 * <ul>
 * <li>The maximum UDP payload size is limited to <i>92 bytes</i>.
 * <li>Can't remember of anything else at the moment, but there surely are more
 * limitations...
 * </ul>
 *
 * \file
 * \author  d264
 * \version $Rev$
 *
 * \section DESCRIPTION
 *
 * Initializes and runs the XBee-Tunnel-Daemon. Note, that the daemon is run as
 * a regular user application in fact, so use some daemonizer (e.g.
 * start-stop-daemon with the --background option) to run the daemon as a real
 * background process.
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
#include <pthread.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

/**
 * Initialize the tunnel daemon by initializing the tunnel and creating the
 * transmitter and receiver threads.
 * Note, this function does not return before the transmitter and receiver
 * threads are terminated.
 *
 * \param[in] serialDevice is the name of the serial device the XBee is
 * connected to.
 * \param[in] inetAddr is the IP address of the local device.
 *
 * \return true if successful, false in case of any error.
 */
static bool daemonInit(const char *serialDevice, const char *inetAddr);

/**
 * The transmitter sends data received from the TUN device via the Xbee.
 *
 * \param[in] data is not used.
 *
 * \return Always NULL.
 */
static void *daemonTransmit(void *data);

/**
 * The Receiver receives data from the Xbee and sends it to the TUN device.
 *
 * \param[in] data is not used.
 *
 * \return Always NULL.
 */
static void *daemonReceive(void *data);

/**
 * Signal handler for the daemon.
 *
 * \param[in] sig specifies the signal.
 */
static void daemonSignalCatch(int sig);

/** This is our tunnel. */
static Tunnel *theTunnel = NULL;

/**
 * Application entry point. Creates the tunnel and starts the daemon.
 *
 * \return 0 if successful, -1 in case of any error.
 */
int main(int argc, char* argv[])
{
	/* Name of serial device the XBee is connected to. */
	static char serialDeviceName[80];
	/* IP address for the Xbee */
	static char inetAddrString[16];
	/** VERBOSE flag, set to 1 to enable verbose mode */
	static bool verbose = false;

	/* Configure the signals we want to catch. */
	signal(SIGHUP,  daemonSignalCatch);
	signal(SIGTERM, daemonSignalCatch);
	signal(SIGINT,  daemonSignalCatch);
	signal(SIGQUIT, daemonSignalCatch);

	/* Parse command line options. */
	while (1)
	{
		/* These are the available tunnel daemon options. */
		static struct option options[] = {
			{ "inet"   , required_argument, 0, 'i' },
			{ "serial" , required_argument, 0, 's' },
			{ "verbose", no_argument      , 0, 'v' },
			{ 0        , 0                , 0, 0   }
		};
		int index, result;

		result = getopt_long(argc, argv, "i:s:v", options, &index);
		if (result == -1)
		{
			break;	/* done */
		}

		switch (result)
		{
		case 's':	/* name of the serial device the XBee is connected to */
			strncpy(serialDeviceName, optarg, sizeof(serialDeviceName) - 1);
			break;
		case 'i':	/* IP address to use for the XBee */
			strncpy(inetAddrString, optarg, sizeof(inetAddrString) - 1);
			break;
		case 'v':	/* Enable VERBOSE mode */
			verbose = true;
			break;
		}
	}
	
	/* Open the syslog, depending if verbose mode has been selected. */
	if (verbose)
	{
		setlogmask(LOG_UPTO(LOG_DEBUG));
		openlog(PROJECT_NAME, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);
		syslog(LOG_INFO, "Verbose mode");
	}
	else
	{
		setlogmask(LOG_UPTO(LOG_INFO));
		openlog(PROJECT_NAME, LOG_CONS, LOG_USER);
		syslog(LOG_INFO, "Silent mode");
	}

	/* Ensure all required options are set. */
	if ((strlen(serialDeviceName) == 0) || (strlen(inetAddrString) == 0))
	{
		syslog(LOG_WARNING, "Don't know IP address or serial device name to use");
		syslog(LOG_INFO, "Usage: %s --inet <internet address> --serial <serial device> [--verbose]",
				PROJECT_NAME);
		exit(EXIT_FAILURE);
	}
	
	syslog(LOG_INFO, "%s version %s, Copyright (C) 2011, d264",
			PROJECT_NAME, PROJECT_VERSION);


	/* Initialize the Tunnel. */
	if (!daemonInit(serialDeviceName, inetAddrString))
	{
		syslog(LOG_ERR, "Error initializing the daemon");
		exit(EXIT_FAILURE);
	}

	syslog(LOG_INFO, "%s is shutting down", PROJECT_NAME);
	exit(EXIT_SUCCESS);
}

/*************************************************************************/

static bool daemonInit(const char *serialDevice, const char *inetAddr)
{
	/* Initialize the tunnel. */
	theTunnel = tunnelInit(serialDevice, inetAddr);
	if (!theTunnel)
	{
		syslog(LOG_ERR, "Error creating the tunnel");
		return false;
	}
	syslog(LOG_DEBUG, "Tunnel successfully initialized");

	/* Create the transmitter task. */
	pthread_t transmitter;
	if (pthread_create(&transmitter, NULL, daemonTransmit, NULL) != 0)
	{
		syslog(LOG_ERR, "Error starting the transmitter");
		return false;
	}

	syslog(LOG_DEBUG, "Transmitter successfully started");

	/* Create the receiver thread. */
	pthread_t receiver;
	if (pthread_create(&receiver, NULL, daemonReceive, NULL) != 0)
	{
		syslog(LOG_ERR, "Error starting the receiver");
		return false;
	}

	syslog(LOG_DEBUG, "Receiver successfully started");
	syslog(LOG_INFO, "%s up and running", PROJECT_NAME);

	pthread_join(transmitter, NULL);
	pthread_join(receiver, NULL);
	return true;
}

/*****************************************************************************/

static void *daemonTransmit(void *data)
{
	/* XBee transmission request, 16bit address. */
	GBeeTxRequest16 txRequest;
	/* Length of the XBee data frame. */
	uint16_t txRequestLength;

	/* Infinite loop for transmitting data. */
	while (1)
	{
		/* Receive the IP packets from the TUN device. */
		if (!tunnelInetReceive(theTunnel, &txRequest, &txRequestLength))
		{
			syslog(LOG_WARNING, "Error reading IP packet");
			continue;
		}

		syslog(LOG_DEBUG, "---> %d bytes to 0x%04x",
				txRequestLength, GBEE_USHORT(txRequest.dstAddr16));

		/* Send the data to the PAN. */
		if (!tunnelGBeeSend(theTunnel, &txRequest, txRequestLength))
		{
			syslog(LOG_WARNING, "ERROR sending XBee frame");
			continue;
		}
	}

	pthread_exit(0);
	return NULL;
}

/*************************************************************************/

static void *daemonReceive(void *data)
{
	/* The packet received from the PAN. */
	GBeeRxPacket16 rxPacket;
	/* Length of the packet received from the PAN. */
	uint16_t rxPacketLength;

	while (1)
	{
		/* Receive the packets from the PAN. */
		if (!tunnelGBeeReceive(theTunnel, (GBeeFrameData *)&rxPacket, &rxPacketLength))
		{
			syslog(LOG_WARNING, "Error reading XBee frame");
			continue;
		}

		/* Dump the packet data if in verbose mode. */
		syslog(LOG_DEBUG, "<--- %d bytes from 0x%04x at -%ddBm",
				rxPacketLength, GBEE_USHORT(rxPacket.srcAddr16), rxPacket.rssi);

		/* Write the packets to the TUN device. */
		if (!tunnelInetSend(theTunnel, &rxPacket, rxPacketLength))
		{
			syslog(LOG_WARNING, "Error sending IP packet");
			continue;
		}
	}

	pthread_exit(0);
	return NULL;
}

/*************************************************************************/

static void daemonSignalCatch(int sig)
{
	/* Check the signal we caught. */
    switch (sig)
    {
    case SIGHUP:
    	syslog(LOG_INFO, "Caught the HUP signal - terminating");
    	break;

    case SIGTERM:
    	syslog(LOG_INFO, "Caught the TERM signal - terminating");
    	break;

    case SIGINT:
    	syslog(LOG_INFO, "Caught the INT signal - terminating");
    	break;

    case SIGQUIT:
    	syslog(LOG_INFO, "Caught the QUIT signal - terminating");
    	break;

    default:
    	syslog(LOG_WARNING, "Caught unhandled signal #%d", sig);
    	return;
    }

    /* If we get here, then the daemon will be terminated. */
    tunnelExit(theTunnel);
    exit(EXIT_SUCCESS);
}
