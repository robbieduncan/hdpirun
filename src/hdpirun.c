// HDPiRun.c

// Author: Robbie Duncan

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#ifndef SIOCGIFCONF
#include <sys/sockio.h>
#endif

#include "log.h" 
#include "abort.h"

/* This flag controls termination of the main loop. */
volatile sig_atomic_t keepRunning = 1;

/* The signal handler just clears the flag and re-enables itself. */
void catch_term (int sig)
{
	__LOG(__LOG_DEBUG,"SIGTERM caught, shutting down");
	keepRunning = 0;
	signal (sig, catch_term);
}


int main(int argc, char **argv)
{	
	
    /* Establish a handler for SIGTEM signals. */
    signal (SIGTERM, catch_term);
	
	int optchar;
	char *logF = NULL;
	char *hdHomeRunHost;	
	// Parse those args
	opterr = 0;
	__LOG(__LOG_DEBUG,"About to parse args");
	
	// Accepted args
	//	d : debug level
	//  l : log file
	//  h : hostname/IP of HDHomeRun
	while ((optchar = getopt(argc, argv, "d:l:h:")) != -1)
	{
		__LOG(__LOG_TRACE,"Arg: optopt='%c', optchar='%c'",optopt,optchar);
		
		switch(optchar)
		{
			case 'd':
				errno = 0;
				int newDebug = strtol(optarg,NULL,10);
				if (errno != 0)
				{
					__ABORT("Option -%c requires an integer arguement", optopt);
				}
				if (newDebug != __LOG_TRACE &&
					newDebug != __LOG_DEBUG &&
					newDebug != __LOG_WARN &&
					newDebug != __LOG_ERROR &&
					newDebug != __LOG_CRITICAL)
				{
					__ABORT("Debug level must be one of 0, 1, 2, 3, 4 (Trace, Debug, Warn, Error, Critical). Passed '%i'",newDebug);
				}
				__LOG(__LOG_DEBUG,"Debug level set to '%i'",newDebug);
				__logLevel = newDebug;
				break;
			case 'l':
				__LOG(__LOG_DEBUG,"Logfile arg set to '%s'",optarg);
				// To Do: log file opening. This can only be done once we have a safe method of aborting
				break;
			case 'h':
				__LOG(__LOG_DEBUG,"HDHomeRun host set to '%s'",optarg);
				hdHomeRunHost = optarg;
				break;
			case '?':
				__LOG(__LOG_TRACE,"Unkown arg");
				if (optopt == 'l' ||
					optopt == 'd' ||
					optopt == 'h' )
				{
					__ABORT("Option -%c requires an argument.", optopt);
				}
				else
				{
					__ABORT("Unknown arg -%c.", optopt);
				}
			default:
				__LOG(__LOG_TRACE,"Unkown arg");
				__LOG(__LOG_CRITICAL, "Unknown arg -%c.", optopt);
				return 1;
		}
	}
	
	__LOG(__LOG_DEBUG,"Args Parsed");
	
	__LOG(__LOG_MESSAGE,"hdpirun starting up")
	
	__LOG(__LOG_DEBUG,"Checking Tuner Hosts");
	__LOG(__LOG_DEBUG,"Tuner(s) OK");
	
	__LOG(__LOG_DEBUG,"Opening network ports");
	// Create a socket
	int udpServerSocket = socket(AF_INET,SOCK_DGRAM,0);
    if (udpServerSocket < 0)
	{
		__ABORT("Unable to create UDP socket for network commands");
  	}
	// Create a server address struct
	struct sockaddr_in serverAddress;
	bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sin_port = htons(65001); // ToDo: Take this from hdhomerunlibrary
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	// Bind the socket
    if (bind(udpServerSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
	{
		__ABORT("Cannot bind to port %i for UDP",65001);
	}
	__LOG(__LOG_TRACE,"UDP port %i bound",65001);
	__LOG(__LOG_DEBUG,"Network ports open");
	
	__LOG(__LOG_MESSAGE,"Waiting for network commands");
	while (keepRunning)
	{
		// Wait for a connection to the socket
		listen(udpServerSocket,5);
	}
	__LOG(__LOG_DEBUG,"Shutting down");
	
	__LOG(__LOG_MESSAGE,"hdpirun shut down");
	
	return 0;
}