// commandline.h
// hdpirun
//
// Utility functions to parse and validate the command line args and access to those parsed args
//
// Author: Robbie Duncan
// Copyright: Copyright (c) 2016 Robbie Duncan
// License: See LICENSE
//
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "commandline.h"
//
// Internal private variables for args. Access is via accessor functions
LogLevel __argLogLevel;
FILE *__logFile;
int __hdHomeRunIP;
//
// Print usage message to stderr
void __usage()
{
	fprintf(stderr,"\nUsage: hdpirun [-d<messageLevel>] [-l<logfile>] [-h<hostname>|-i<ipaddress>]\n\n");
}
//
// Parse and validate the command line args
int parseArgs(int argc, char **argv)
{
	int optchar;
	struct hostent *hostDetails;
	// Set to defaults
	__argLogLevel = message;
	__hdHomeRunIP = -1;
	__logFile = stdout;
		
	// Parse the args
	while ((optchar = getopt(argc, argv, "d:l:h:i:")) != -1)
	{
		LOG(trace,"Arg: optopt='%c', optchar='%c'",optopt,optchar);
		
		switch(optchar)
		{
			case 'd':
				errno = 0;
				int newDebug = strtol(optarg,NULL,10);
				if (errno != 0)
				{
					LOG(critical,"Option -%c requires an integer arguement", optopt);
					__usage();
					return 0;
				}
				if (newDebug != trace &&
					newDebug != debug &&
					newDebug != warn &&
					newDebug != error &&
					newDebug != critical)
				{
					LOG(critical,"Debug level must be one of 0, 1, 2, 3, 4, 5 (Trace, Debug, Message, Warn, Error, Critical). Passed '%i'",newDebug);
					__usage();
					return 0;
				}
				LOG(debug,"Debug level set to '%i'",newDebug);
				__argLogLevel = newDebug;
				break;
			case 'l':
				LOG(debug,"Logfile arg set to '%s'",optarg);
				FILE *tempLog = fopen(optarg,"a");
				if (tempLog == NULL)
				{
					LOG(critical,"Cannot open log file '' for append",optarg)
				}
				else
				{
					__logFile = tempLog;
				}
				break;
			case 'h':
				if ((hostDetails = gethostbyname(optarg)) == NULL)
				{
					LOG(critical,"Could not get IP address for host %s",optarg);
					return 0;
				}
				struct in_addr **addr_list;
			    addr_list = (struct in_addr **)hostDetails->h_addr_list;
				// Use the first address
				__hdHomeRunIP = htonl((*addr_list[0]).s_addr);
				LOG(debug,"HDHomeRun hostname set to '%s'",optarg);
				break;
			case 'i':
				// Is the arg a real IP address?
				if (!inet_pton(AF_INET, optarg, &__hdHomeRunIP))
				{
					LOG(critical,"Could not interpret %s as an IP address",optarg);
					return 0;
				}
				// Turn into network byte order
				__hdHomeRunIP = htonl(__hdHomeRunIP);
				LOG(debug,"HDHomeRun IP Address set to '%s'",optarg);
				break;
			case '?':
				LOG(trace,"Unkown arg");
				if (optopt == 'l' ||
					optopt == 'd' ||
					optopt == 'h' ||
					optopt == 'i' )
				{
					LOG(critical,"Option -%c requires an argument.", optopt);
					__usage();
					return 0;
				}
				else
				{
					LOG(critical,"Unknown arg -%c.", optopt);
					__usage();
					return 0;
				}
				break;
			default:
				LOG(critical, "Unknown arg -%c.", optopt);
				__usage();
				return 0;
		}
	}
	return 1;
}

LogLevel getArgLogLevel()
{
	return __argLogLevel;
}

FILE* getArgLogFile()
{
	return __logFile;
}

int getArgHDHomeRunIP()
{
	return __hdHomeRunIP;
}
