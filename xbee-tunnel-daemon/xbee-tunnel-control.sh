#!/bin/bash
#
# Author: d264, 2011
#
# LICENSE
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# DESCRIPTION
#
# Script for managing the xbee-tunnel-daemon.
#

# Path to the xbee-tunnel-daemon; here it is assumed, tha the xbee-tunnel-daemon
# resides in the same directory as this script.
XBEE_TUNNEL_DAEMON_PATH="$(pwd)/xbee-tunnel-daemon"

# Default arguments for calling the xbee-tunnel-daemon.
# Before starting the xbee-tunnel-daemon its args are read from the config files
# in ./.xbee-tunnel-daemon.rc or ~/.xbee-tunnel-daemon.rc, if these files are
# found, then the default args are used.
XBEE_TUNNEL_DAEMON_ARGS="--inet 10.10.123.123 --serial /dev/ttyUSB0"

# Name of the xbee-tunnel-daemon configuration file.
XBEE_TUNNEL_RC=".xbee-tunnel-rc"

# Path to the PID file of the xbee-tunnel-daemon.
PIDFILE_PATH="/var/run/xbee-tunnel-daemon.pid"

# Command to run the daemonizer; here the start-stop-daemon tool is used.
DAEMONIZER="start-stop-daemon"

# Args for the daemonizer to start the daemon.
DAEMON_START="-S -b -k -d / -p $PIDFILE_PATH -m -a $XBEE_TUNNEL_DAEMON_PATH"

# Args for the daemonizer to stop the daemon.
DAEMON_STOP="-K -p $PIDFILE_PATH"

#
# Start the xbee-tunnel-daemon.
#
# Try to read the ./.xbee-tunnel-rc or ~/.xbee-tunnel-rc for getting the 
# xbee-tunnel-daemon options. Use default options if file is not found in 
# either of the directories.
#
# Extract the IP address of the TUN device from the xbee-tunnel-daemon options.
#
# Configure the TUN device with the IP address extracted above, use default
# address if no IP address was extracted.
#
# Use the start-stop-daemon to daemonize and run the xbee-echo-daemon.
#
start() {
	if [ -f ./$XBEE_TUNNEL_RC ]
	then
		echo "Using values from .xbee-tunnel-rc."
		XBEE_TUNNEL_DAEMON_ARGS=`cat ./$XBEE_TUNNEL_RC`
	elif [ -f ~/$XBEE_TUNNEL_RC ]
	then
		echo "Using values from ~/.xbee-tunnel-rc."
		XBEE_TUNNEL_DAEMON_ARGS=`cat ~/$XBEE_TUNNEL_RC`
	else
		echo "Warning: No .xbee-tunnel-rc found. Using default values."
	fi
	
	echo "Starting xbee-tunnel-daemon with: $XBEE_TUNNEL_DAEMON_ARGS"
	
	$DAEMONIZER $DAEMON_START -- $XBEE_TUNNEL_DAEMON_ARGS
	if [[ $? -ne 0 ]]
	then
		echo "Error starting the xbee-tunnel-daemon"
		return $?
	fi
	
	return $?
}

#
# Stop the xbee-tunnel-daemon.
#
# Use the start-stop-daemon to locate the PIDFILE and stopping the process.
#
stop() {
	echo "Stopping xbee-tunnel-daemon..."
	$DAEMONIZER $DAEMON_STOP
	return $?
}

# First check if the start-stop-daemon tool is available.
# No point in continuing without this tool.
type start-stop-daemon > /dev/null
if [ "$?" -ne 0 ]
then
	echo "start-stop-daemon not found."
	echo "Please install start-stop-daemon first using your package manager."
	exit 1
fi

# Check which command to execute.
case "$1" in
	start)
		start
		;;
	stop)
		stop
		;;
	restart)
		stop
		start
		;;
	*)
		echo "Usage:  {start|stop|restart}"
		exit 1
		;;
esac
exit $?

