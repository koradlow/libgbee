/**
 * \mainpage
 *
 * This is an example program showing how to use the libgbee in combination
 * with the xbee-tunnel-daemon. Make sure the xbee-tunnel-daemon is running
 * before starting the xbee-echo-client.
 *
 * The xbee-echo-client will open a UDP socket and send random data to UDP
 * port 7 (echo protocol) of the remote host specified by the provided IP
 * address. Then the xbee-echo-client waits for the same data being returned by
 * the remote host.
 * 
 * <BR>
 * Usage: xbee-echo-client [OPTIONS]<BR>
 * <BR>
 * Mandatory options are:<BR>
 * <TABLE>
 * <TR><TD>-a, --address IP_ADDRESS</TD><TD>specifies the IP address of the
 * echo server, e.g. 10.10.10.10.</TD></TR>
 * <TR><TD>-l, --length LENGTH_IN_BYTES</TD><TD>specifies the length of the
 * echo message, up to 92 bytes.</TD></TR>
 * </TABLE>
 * <BR>
 * Additional options are:<BR>
 * <TABLE>
 * <TR><TD>-i, --iterate NUMBER_OF_ITERATIONS</TD><TD>specifies the number of
 * iterations, default is 1.</TD></TR>
 * <TR><TD>-f, --fork NUMBER_OF_FORKS</TD><TD>specifies the number forks of the
 * echo client to run simultaneously.</TD></TR>
 * <TR><TD>-h, --help</TD><TD>shows this help text.</TD></TR>
 * <TR><TD>-t, --terms</TD><TD>shows license terms and conditions.</TD></TR>
 * </TABLE>
 *
 * See \ref build_instructions for a description how to build the XBee Echo
 * Client from source.
 *
 * \page build_instructions Build Instructions
 *
 * The XBee Echo Client's build system is also based on CMake. However, since
 * the XBee Echo Client uses the XBee Tunnel Daemon, the only platform
 * supported so far is GNU/Linux.
 *
 * Before starting CMake you will have to create the directory where you want
 * to make the build, e.g.
 * \code
 * ~/xbee/build/linux/x86/xbee-echo-client
 * \endcode
 *
 * Open a terminal, browse into the build directory and run \a cmake-gui. When
 * being asked for the generator to use for this build, make sure to specify
 * <i>Use default native compilers</i>.
 *
 * The following options are available for configuring the XBee Echo Client
 * build:
 *
 * <table>
 * <tr>
 * <td>CMAKE_INSTALL_PREFIX</td>
 * <td>Selects the installation prefix for the library. The default value is
 * \a /usr/local.</td>
 * </tr>
 * </table>
 *
 * After configuration of the build system you can build the XBee Echo Client
 * by running \a make in your build directory.
 *
 * \file
 * \author  d264
 * \version $Rev$
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
 *
 * \section DESCRIPTION
 *
 * XBee-Echo-Client main source file.
 */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <signal.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

/** Contains the client parameters. */
struct ClientParams {
	char      serverAddrString[16]; /**< IP address of echo server as string. */
	in_addr_t serverAddr;           /**< IP address of echo server. */
	uint32_t  echoLength;           /**< Length of the echo message. */
	uint32_t  numIterations;        /**< Number of loop iterations. */
	uint32_t  numForks;             /**< Number of forks to take. */
	bool      showHelp;             /**< Flag telling if to show help. */
	bool      showLicense;          /**< Flag telling if to show license. */
};

/** Type definition for client parameters. */
typedef struct ClientParams ClientParams;

/** Client statistics. */
struct ClientStats {
	uint32_t *elapsedTime; /**< Elapsed time for each iteration. */
	uint32_t  averageTime; /**< Average elapsed time. */
	uint32_t  minTime;     /**< Minimum elapsed time. */
	uint32_t  maxTime;     /**< Maximum elapsed time. */
	uint32_t  numErrors;   /**< Number of errors detected. */
};

/** Type definition for client statistics. */
typedef struct ClientStats ClientStats;

/**
 * Prints usage information.
 */
static void printUsage(void);

/**
 * Prints the program banner.
 */
static void printBanner(void);

/**
 * Prints license information.
 */
static void printLicense(void);

/**
 * Parse the program arguments and get the values.
 *
 * \param[in] numArgs is the number of arguments.
 * \param[in] args is a pointer to the arguments.
 * \param[out] params contains the parsed arguments.
 *
 * \return true if successful, false in case of any error.
 */
static bool parseUserInput(int32_t numArgs, char *args[], ClientParams *params);

/**
 * Calculates the elapsed time.
 *
 * \param[in] time1 is the first time stamp (see gettimeofday).
 * \param[in] time2 is the second time stamp (see gettimeofday).
 *
 * \return The elapsed time in milliseconds.
 */
static uint32_t getElapsedTime(struct timeval *time1, struct timeval *time2);

/**
 * Generates a pseudo-random message with the specified length, but not longer
 * than 92 bytes.
 *
 * \param[in] length is the length of the message (<= 92).
 *
 * \return A pointer to the statically allocated message.
 */
static uint8_t *getEchoRequest(uint32_t length);

/**
 * Handler for echo receive timeout.
 */
static void receiveTimeoutHandler(int signalNr);

/** Receive timeout value in seconds. */
#define ECHO_RECEIVE_TIMEOUT 10
/** Receive timeout flag. */
static sigjmp_buf receiveTimeout;

/**
 * Application entry point. Configures the peripherals and starts the echo 
 * server.
 * 
 * For a description of parameters, please refer to the file description.
 * 
 * \return 0 if successful, -1 in case of any error.
 */
int	main(int argc, char *argv[])
{
	/* The client parameters specified by the user. */
	static ClientParams clientParams;
	/* The client statistics. */
	static ClientStats clientStats;

	/* The socket for sending/receiving the echo. */
	int echoSocket;
	/* Address of the echo server. */
	struct sockaddr_in echoServer;
	/* Address received by the echo client. */
	struct sockaddr_in echoClient;
	/* Length of the address received by echo client. */
	uint32_t clientLength = sizeof(struct sockaddr_in);

	/* Time stamp before sending the echo request. */
	struct timeval timeBeforeSend;
	/* Time stamp after receiving the echo response. */
	struct timeval timeAfterReceive;

	/* ID of the current fork. */
	uint32_t forkNr = 0;
	/* Macro for logging debug messages to the console. */
#define LOGMSG(format,args...) printf("[%d] " format "\n", forkNr, ##args)

	printBanner();
	
	/* Parse program options. */
	if (!parseUserInput(argc, argv, &clientParams))
	{
		printUsage();
		return -1;
	}
	if (clientParams.showHelp)
	{
		printUsage();
		return 0;
	}
	if (clientParams.showLicense)
	{
		printLicense();
		return 0;
	}

	/* Create the requested number of forks. */
	if (clientParams.numForks)
	{
		pid_t pid;
		do
		{
			if ((pid = fork()) == 0)
			{
				break;
			}
			forkNr++;
			clientParams.numForks--;
		}
		while (clientParams.numForks);
	}

	/* Create the echo request and response buffers. */
	uint8_t *echoRequest;
	uint8_t *echoResponse = malloc(clientParams.echoLength);
	if (!echoResponse)
	{
		LOGMSG("*** Error allocating echo response buffer.");
		return -1;
	}

	/* Create the UDP socket. */
	echoSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (echoSocket < 0)
	{
		LOGMSG("*** Error creating echo socket.");
		return -1;
	}

	/* Create the echo server address. */
	memset(&echoServer, 0, sizeof(echoServer));
	echoServer.sin_family      = AF_INET;
	echoServer.sin_addr.s_addr = clientParams.serverAddr;
	echoServer.sin_port        = htons(7);

	/* Prepare the stats. */
	clientStats.numErrors   = 0;
	clientStats.maxTime     = 0;
	clientStats.minTime     = 0xFFFFFFFF;
	clientStats.averageTime = 0;
	clientStats.elapsedTime = malloc(sizeof(uint32_t) * clientParams.numIterations);
	if (!clientStats.elapsedTime)
	{
		LOGMSG("*** Error allocating memory.");
		return -1;
	}

	/* Loop through the number of selected iterations. */
	uint32_t iter; for (iter = 0; iter < clientParams.numIterations; iter++)
	{
		LOGMSG("Iteration %d/%d.", iter + 1, clientParams.numIterations);

		/* Prepare the echo request message. */
		echoRequest = getEchoRequest(clientParams.echoLength);
		clientStats.elapsedTime[iter] = 0;

		/* Send the data to the echo server. */
		LOGMSG("Sending %d characters to %s...",
				clientParams.echoLength, clientParams.serverAddrString);

		gettimeofday(&timeBeforeSend, NULL);

		int32_t requestLength = sendto(echoSocket, echoRequest,
				clientParams.echoLength, 0, (struct sockaddr *)&echoServer,
				sizeof(echoServer));

		if (requestLength != clientParams.echoLength)
		{
			LOGMSG("*** Error sending echo request.");
			clientStats.numErrors++;
			continue;
		}

		/* Set timeout for echo reception. */
		if (sigsetjmp(receiveTimeout, 1))
		{
			LOGMSG("*** Timeout while waiting for echo.");
			clientStats.numErrors++;
			continue;
		}

		signal(SIGALRM, receiveTimeoutHandler);
		alarm(ECHO_RECEIVE_TIMEOUT);

		/* Wait for the echo response. */
		int32_t responseLength = recvfrom(echoSocket, echoResponse,
				clientParams.echoLength, 0,	(struct sockaddr *)&echoClient,
				&clientLength);

		/* Clear the timeout alarm and remember the time. */
		alarm(0);
		signal(SIGALRM, SIG_DFL);
		gettimeofday(&timeAfterReceive, NULL);

		/* Check the echo response. */
		if (responseLength == -1)
		{
			LOGMSG("*** Echo receive error: %s", strerror(errno));
			continue;
		}
		if (responseLength != clientParams.echoLength)
		{
			LOGMSG("*** Echo size mismatch: received %d bytes, expected %d.",
					responseLength, clientParams.echoLength);
			clientStats.numErrors++;
			continue;
		}
		if (echoServer.sin_addr.s_addr != echoClient.sin_addr.s_addr)
		{
			LOGMSG("*** Received unexpected echo from %s, expected %s.",
					inet_ntoa(echoClient.sin_addr), inet_ntoa(echoServer.sin_addr));
			clientStats.numErrors++;
			continue;
		}
		if (memcmp(echoRequest, echoResponse, responseLength) != 0)
		{
			LOGMSG("*** Echo data mismatch.");
			clientStats.numErrors++;
			continue;
		}

		/* Remember the echo duration. */
		clientStats.elapsedTime[iter] = getElapsedTime(&timeBeforeSend,
				&timeAfterReceive);
		LOGMSG("Received echo after %u milliseconds.",
				clientStats.elapsedTime[iter]);

		/* Calculate the statistics. */
		clientStats.averageTime += clientStats.elapsedTime[iter];
		if (clientStats.elapsedTime[iter] > clientStats.maxTime)
		{
			clientStats.maxTime = clientStats.elapsedTime[iter];
		}
		if (clientStats.elapsedTime[iter] < clientStats.minTime)
		{
			clientStats.minTime = clientStats.elapsedTime[iter];
		}
	}

	/* Calculate the statistics. */
	if (clientParams.numIterations > clientStats.numErrors)
	{
		clientStats.averageTime = clientStats.averageTime /
				(clientParams.numIterations - clientStats.numErrors);
	}
	else
	{
		clientStats.averageTime = clientStats.maxTime;
	}

	/* Print the statistics. */
	printf("\n");
	LOGMSG("Finished with %d errors.", clientStats.numErrors);
	LOGMSG("Echo length = %d bytes: Avg = %d ms, Min = %d ms, Max = %d ms.",
			clientParams.echoLength, clientStats.averageTime, clientStats.minTime,
			clientStats.maxTime);
	printf("\n");

	return 0;
}

/******************************************************************************/

static void printUsage(void)
{
	printf("\n");
	printf("Usage: xbee-echo-client [OPTIONS]\n");
	printf("\n");
	printf("Mandatory options starting echo client:\n");
	printf("-a, --address IP_ADDRESS            specifies the IP address of the echo\n");
	printf("                                    server, e.g. 10.10.10.10.\n");
	printf("-l, --length LENGTH_IN_BYTES        specifies the length of the echo\n");
	printf("                                    message, up to 92 bytes.\n");
	printf("\n");
	printf("Additional options:\n");
	printf("-i, --iterate NUMBER_OF_ITERATIONS  specifies the number of iterations,\n");
	printf("                                    default is 1.\n");
	printf("-f, --fork NUMBER_OF_FORKS          specifies the number forks of the\n");
	printf("                                    echo client to run simultaneously.\n");
	printf("-h, --help                          shows this help text.\n");
	printf("-t, --terms                         shows license terms and conditions.\n");
	printf("\n");
}

/******************************************************************************/

static void printBanner(void)
{
	printf("\n");
	printf("XBee-Echo-Client version %s, Copyright (C) 2010, d264\n", PROJECT_VERSION);
    printf("The XBee-Echo-Client comes with ABSOLUTELY NO WARRANTY.\n");
    printf("\n");
}

/******************************************************************************/

static void printLicense(void)
{
	printf("\n");
	printf("This program is free software; you can redistribute it and/or modify\n");
	printf("it under the terms of the GNU General Public License as published by\n");
	printf("the Free Software Foundation; either version 2 of the License, or\n");
	printf("(at your option) any later version.\n");
	printf("\n");
	printf("This program is distributed in the hope that it will be useful,\n");
	printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
	printf("GNU General Public License for more details.\n");
	printf("\n");
	printf("You should have received a copy of the GNU General Public License along\n");
	printf("with this program; if not, write to the Free Software Foundation, Inc.,\n");
	printf("51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.\n");
	printf("\n");
}

/******************************************************************************/

static bool parseUserInput(int32_t numArgs, char *args[], ClientParams *params)
{
	/* Program options. */
	static struct option options[] = {
		{ "address", required_argument, 0, 'a' },
		{ "length ", required_argument, 0, 'l' },
		{ "fork"   , required_argument, 0, 'f' },
		{ "iterate", required_argument, 0, 'i' },
		{ "help"   , no_argument      , 0, 'h' },
		{ "terms"  , no_argument      , 0, 't' },
		{ 0        , 0                , 0, 0   }
	};

	/* Result and index of getopt function call. */
	int result, index = 0;

	/* Prepare command line options. */
	params->echoLength          = 8;
	params->serverAddr          = 0;
	params->serverAddrString[0] = 0;
	params->numIterations       = 1;
	params->numForks            = 0;
	params->showHelp            = false;
	params->showLicense         = false;

	/* Parse command line parameters. */
	while (1)
	{
		result = getopt_long(numArgs, args, "a:l:i:f:h:t", options, &index);
		if (result == -1)
		{
			break;	/* done */
		}

		switch (result)
		{
		case 'a':	/* IP address of the server */
			strncpy(params->serverAddrString, optarg,
					sizeof(params->serverAddrString) - 1);
			params->serverAddr = inet_addr(params->serverAddrString);
			break;
		case 'l':	/* Message to send to the echo server */
			params->echoLength = strtoul(optarg, NULL, 10);
			break;
		case 'i':	/* Number of loop iterations */
			params->numIterations = strtoul(optarg, NULL, 10);
			break;
		case 'f':	/* Number of forks. */
			params->numForks = strtoul(optarg, NULL, 10);
			break;
		case 'h':	/* Print help */
			params->showHelp = true;
			return 0;
		case 't':	/* Print license terms & conditions */
			params->showLicense = true;
			return 0;
		}
	}

	if ((!params->showHelp) && (!params->showLicense) && (!params->serverAddr))
	{
		return false;
	}
	else
	{
		return true;
	}
}

/******************************************************************************/

static uint32_t getElapsedTime(struct timeval *time1, struct timeval *time2)
{
	uint32_t elapsedTime;
	if (time1->tv_sec < time2->tv_sec)
	{
		elapsedTime = (time2->tv_sec - time1->tv_sec) * 1000;
		elapsedTime += (time2->tv_usec / 1000);
		elapsedTime -= (time1->tv_usec / 1000);
	}
	else
	{
		elapsedTime = (time2->tv_usec - time1->tv_usec) / 1000;
	}
	return elapsedTime;
}

/******************************************************************************/

static uint8_t *getEchoRequest(uint32_t length)
{
	static uint8_t message[92];

	/* Truncate the length if necessary. */
	if (length > sizeof(message))
	{
		length = sizeof(message);
	}

	/* Fill the message with random data. */
	uint32_t byteNr; for (byteNr = 0; byteNr < length; byteNr++)
	{
		message[byteNr] = random() % 0xFF;
	}
	return message;
}

/******************************************************************************/

static void receiveTimeoutHandler(int signalNr)
{
	signal(SIGALRM, SIG_DFL);
	siglongjmp(receiveTimeout, 1);
}
