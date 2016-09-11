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
	__hdHomeRunIP = -1;
			
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
				if (setLogLevel(newDebug))
				{
					LOG(debug,"Debug level set to '%i'",newDebug);
				}
				else
				{
					LOG(critical,"Debug level must be one of 0, 1, 2, 3, 4, 5 (Trace, Debug, Message, Warn, Error, Critical). Passed '%i'",newDebug);
					__usage();
					return 0;
				}
				break;
			case 'l':
				if (setLogFile(optarg))
				{
					LOG(debug,"Logfile set to '%s'",optarg);
				}
				else
				{
					LOG(warn,"Could not set log file to '$s'",optarg);
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
