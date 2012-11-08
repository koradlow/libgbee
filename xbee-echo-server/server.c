/**
 * \mainpage
 *
 * This is an example program showing how to use the libgbee on an AT91 SAM
 * microcontroller.
 *
 * The XBee-Echo-Server waits for an echo request. When an echo request is
 * received, the echo server sends the data back to the originator.
 *
 * Note, that the echo server will only process UDP packets. To generate the
 * appropriate UDP packets you can use the XBee-Echo-Client in combination with
 * the XBee-Tunnel-Daemon on your host system.
 *
 * An alternative way to generate the echo requests is to use the \a echoping
 * tool, also in combination with the XBee-Tunnel-Daemon, e.g. use
 * \code
 * echoping -v -u -s 92 10.10.10.10
 * \endcode
 *
 * to start echoping. This tells echoping to use the UDP protocol (-u) and to
 * set the payload size to 92 bytes (-s 92). Ensure that the XBee-Tunnel-Daemon
 * is running before starting echoping.
 *
 * The XBee-Echo-Server is listening on the XBee 16bit address \a 0x0A0A with
 * the PAN ID \a 0x0A0A. This corresponds to the IP address \a 10.10.10.10.
 *
 * See \ref build_instructions for building the XBee-Echo-Server.
 *
 * \page build_instructions Build Instructions
 *
 * The Xbee Echo Server's build system is also based on CMake, although the
 * only platform supported so far is an AT91-SAM7 micro controller. Before
 * starting CMake you will have to create the directory where you want to
 * make the build, e.g.
 * \code
 * ~/xbee/build/at91/sam7/xbee-echo-server
 * \endcode
 * Browse to this directory in a terminal and run \a cmake-gui. When being
 * asked for the generator to use for this build, select <i>Specify Toolchain
 * for cross-compiling</i>.
 *
 * In the next step, CMake asks for a toolchain file. The toolchain file
 * configures the toolchain used for the build (i.e. compiler, linker, etc.). I
 * have provided some toolchain files which may be useful for building the XBee
 * Echo Server. You will find them in the <i>libgbee/src/port/at91/sam7</i>
 * directory.
 *
 * The following options are available for building the XBee Echo Server:
 *
 * <table>
 * <tr>
 * <td>AT91LIB_BOARD_NAME</td>
 * <td>Set to the name of the AT91LIB source directory which is suitable for
 * your board. The board-specific sources can be found in at91lib/boards. For
 * instance, if you have an AT91SAM7X Evalutation Kit the suitable board
 * package would be \a at91sam7x-ek.</td>
 * </tr>
 * <tr>
 * <td>AT91LIB_CHIP_NAME</td>
 * <td>Set to the name of the AT91LIB source directory which is suitable for
 * your MCU. The MCU-specific sources can be found in
 * at91lib/boards/$AT91LIB_BOARD_NAME. For instance, if you have an AT91SAM7X
 * Evaluation Kit, the suitable MCU package would be \a at91sam7x256.</td>
 * </tr>
 * <tr>
 * <td>AT91LIB_INCLUDE_PATH</td>
 * <td>Specifies the main include path of your AT91LIB. This is the path where
 * the board.h file is located. CCMake will try to automatically locate the
 * AT91LIB in all default paths.</td>
 * </tr>
 * <tr>
 * <td>LIBGBEE</td>
 * <td>The path to your LibGBee built for at91/sam7.</td>
 * </tr>
 * <tr>
 * <td>LIBGBEE_INCLUDE_PATH</td>
 * <td>Specify the path to the LibGBee include files (i.e. the path to gbee.h),
 * e.g. \a ~/xbee/libgbee/src/.</td>
 * </tr>
 * <tr>
 * <td>PROJECT_TARGET_MEMORY</td>
 * <td>Specify if you want to build for Flash (\a flash) or SRAM (\a sram)
 * here, default value is \a flash.</td>
 * </tr>
 * </table>
 *
 * After configuration of the build system, you can build the XBee Echo Server
 * by running \a make in your build directory. Use SAM-BA to download the
 * \a xbee-echo-server.bin to your AT91 SAM
 * microcontroller.
 *
 * \file
 * \author  d264
 * \version $Rev$
 *
 * \section DESCRIPTION
 *
 * Implementation of the XBee Echo Server for AT91 SAM7.
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

#include "board.h"
#include "pio/pio.h"
#include "dbgu/dbgu.h"
#include "usart/usart.h"
#include "aic/aic.h"
#include "utility/led.h"
#include "gbee.h"
#include "gbee-util.h"

#include <stdio.h>
#include <string.h>

/** 16bit address of echo server. */
#define ECHO_SERVER_ADDR 0x0A0A
/** 16bit PAN identifier of echo server. */
#define ECHO_SERVER_PAN  0x0A0A

/** UDP port number for echo service. */
#define ECHO_PORT_UDP 7

/** Calculate PIT period in milliseconds. */
#define PIT_MSEC(msec) ((BOARD_MCK / 16 / 1000) * (msec))

/** PIO pins to configure. */
static const Pin pins[] = { PINS_DBGU };

/**
 * Runs the echo server.
 *
 * \param[in] gbee is a pointer to the GBee driver instance.
 */
static void serverStart(GBee *gbee);

/**
 * Stops the execution of the echo server in case of an error.
 */
static void serverStop(void);

/**
 * PIT interrupt handler.
 */
static void serverTick(void);

/**
 * Simple wait routine - delays for the requested number of milliseconds.
 *
 * \param[in] milliseconds specifies the number of milliseconds to delay.
 */
static void serverWait(uint32_t milliseconds);
                      
/**
 * Application entry point.
 * Configures the peripherals and starts the echo server.
 */
int	main(void)
{
	/* This is our GBee device. */
	GBee *gbee;

	/* Configure pins. */
	PIO_Configure(pins, PIO_LISTSIZE(pins));

	/* Configure and enable LEDs. */
	LED_Configure(LED_DS0);
	LED_Configure(LED_DS1);
	LED_Set(LED_DS0);

	/* Configure DBGU and print welcome message. */
	DBGU_Configure(DBGU_STANDARD, 115200, BOARD_MCK);
	
	/* Configure AIC for PIT interrupt. */
	AIC_ConfigureIT(AT91C_ID_SYS, AT91C_AIC_PRIOR_LOWEST, serverTick);
	/* Configure the PIT period. */
	AT91C_BASE_PITC->PITC_PIMR = AT91C_PITC_PITEN | AT91C_PITC_PITIEN | PIT_MSEC(1);
	/* Enable the PIT interrupt. */
	AIC_EnableIT(AT91C_ID_SYS);

	/* Initialize the Xbee */
	gbee = gbeeCreate("USART1");
	if (!gbee)
	{
		printf("Error creating Xbee driver instance \r\n");
		return -1;
	}

	/* Start the echo server. */
	serverStart(gbee);
	serverStop();

	return 0;
}

/******************************************************************************/

void serverTick(void)
{
	gbeeTickCount();
	/* Signal end of interrupt to the AIC. */
	AT91C_BASE_AIC->AIC_EOICR = AT91C_BASE_PITC->PITC_PIVR;
}

/******************************************************************************/

static void serverStart(GBee *gbee)
{
	/* Xbee mode. */
	GBeeMode mode;
	/* Xbee error code. */
	GBeeError error;
	/* Data packet using 16bit short address. */
	GBeeRxPacket16 rxPacket16;
	/* Length of received packet. */
	uint16_t packetLength;
	/* Xbee receive timeout. */
	uint32_t timeout;

	/* Print banner. */
	serverWait(1000);
	printf("\r\n *** XBee-Echo-Server (%s) *** \r\n", VERSION);
	
	/* Get Xbee mode. */
	error = gbeeGetMode(gbee, &mode);
	if (error != GBEE_NO_ERROR)
	{
		printf("Error getting Xbee mode: %s \r\n", gbeeUtilCodeToString(error));
		serverStop();
	}
	
	printf("Current Xbee mode is %s \r\n", 
			mode == GBEE_MODE_TRANSPARENT ? "Transparent" : "API");

	/* If current mode is 'Transparent', set 'API' mode. */
	if (mode != GBEE_MODE_API)
	{
		error = gbeeSetMode(gbee, GBEE_MODE_API);
		if (error != GBEE_NO_ERROR)
		{
			printf("Error setting Xbee mode: %s \r\n", gbeeUtilCodeToString(error));
			serverStop();
		}
		
		printf("Set Xbee to API mode \r\n");
	}

	/* Set Xbee's 16-bit address. */
	error = gbeeUtilSetAddress16(gbee, ECHO_SERVER_ADDR, ECHO_SERVER_PAN);
	if (error != GBEE_NO_ERROR)
	{
		printf("Error setting Xbee address to 0x%04x (PAN 0x%04x): %s \r\n",
				ECHO_SERVER_ADDR, ECHO_SERVER_PAN, gbeeUtilCodeToString(error));
		serverStop();
	}
	
	printf("Set Xbee address to 0x%04x (PAN 0x%04x) \r\n",
			ECHO_SERVER_ADDR, ECHO_SERVER_PAN);
	printf("Ready \r\n");

	while (1)
	{
		/* Payload of received packet. */
		uint8_t *payload;
		/* Payload length of received packet. */
		uint16_t payloadLength;
		/* Address of echo client. */
		GBeeSockAddr clientAddr;
		/* GBee transmission request. */
		GBeeTxRequest16 txRequest16;
		/* GBee transmission length. */
		uint16_t txRequestLength;

		LED_Clear(LED_DS1);

		/* Wait for echo requests. */
		timeout = GBEE_INFINITE_WAIT;
		do
		{
			error = gbeeReceive(gbee, (GBeeFrameData *)&rxPacket16, &packetLength,
					&timeout);
		}
		while ((error == GBEE_NO_ERROR) 
				&& (rxPacket16.ident != GBEE_RX_PACKET_16));

		LED_Set(LED_DS1);

		if (error != GBEE_NO_ERROR)
		{
			printf("Error receiving data: %s \r\n", gbeeUtilCodeToString(error));
			continue;
		}
		else
		{
			printf("Received %d bytes from 0x%04x, signal strength = -%ddBm \r\n",
					packetLength, GBEE_USHORT(rxPacket16.srcAddr16), rxPacket16.rssi);
		}

		/* Check if received packet contains UDP data. */
		if (gbeeUtilDecodeUdp(&rxPacket16, packetLength, &payload, &payloadLength,
				&clientAddr))
		{
			/* Encode the UDP packet containing the echo. */
			gbeeUtilEncodeUdp(payload, payloadLength, ECHO_PORT_UDP, &clientAddr,
					&txRequest16, &txRequestLength);

			/* Send back the echo. */
			error = gbeeSend(gbee, (GBeeFrameData *)&txRequest16, txRequestLength);
			if (error != GBEE_NO_ERROR)
			{
				printf("Error sending echo: %s \r\n", gbeeUtilCodeToString(error));
			}
		}
	}

	serverStop();
}

/******************************************************************************/

static void serverStop(void)
{
	printf("STOP \r\n");
	while (1);
}

/******************************************************************************/

static void serverWait(uint32_t milliseconds)
{
	uint32_t timeout = gbeeTickTimeoutCalculate(milliseconds);
	while (gbeeTickTimeoutExpired(timeout) == false);
}
