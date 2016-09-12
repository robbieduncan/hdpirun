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
#include "homerun.h"
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
				if (!addHomeRunTunerHostname(optarg))
				{
					LOG(warn,"Ignoring hostname '%s'",optarg);
				}
				break;
			case 'i':
				if (!addHomeRunTunerIP(optarg))
				{
					LOG(warn,"Ignoring I{} '%s'",optarg);
				}
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