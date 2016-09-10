/*
   hdpirun.c
   hdpirun

   Main entry point for hdpirun

   Author: Robbie Duncan
   Copyright: Copyright (c) 2016 Robbie Duncan
   License: See LICENSE
*/
#include <signal.h>

#include "log.h" 
#include "abort.h"
#include "commandline.h"
#include "network.h"
#include "homerun.h"

// This flag controls termination of the main loop.
volatile sig_atomic_t keepRunning = 1;

// Signal handler for termination signal. Stops the app
void catch_term (int sig)
{
	switch (sig)
	{
		case SIGTERM:
			LOG(debug,"SIGTERM caught, shutting down");
			break;
		case SIGINT:
			LOG(debug,"SIGINT caught, shutting down");
			break;
		default:
			LOG(debug,"Unexpected signal caught, shutting down");
	}
	keepRunning = 0;
	signal (sig, catch_term);
}

// main entry point for the app
int main(int argc, char **argv)
{	
    // Establish a handler for SIGTEM signals.
    signal(SIGTERM, catch_term);
	// Use the same for SIGINT (ctrl-c)
	signal(SIGINT, catch_term);
	
	// Parse those args
	LOG(debug,"About to parse args");
	
	if (!parseArgs(argc,argv))
	{
		ABORT("Failed to parse args, exiting");
	}
	logLevel = getArgLogLevel();
	logFile = getArgLogFile();
	
	LOG(debug,"Args Parsed");
	
	// OK note startup
	LOG(message,"hdpirun starting up");
	
	// Check the tuner host
	LOG(debug,"Checking Tuner Hosts");
	verifyHomerunTuner(getArgHDHomeRunIP());
	LOG(debug,"Tuner(s) OK");
	
	// Start networking
	LOG(debug,"Opening network ports");
	bindDiscoverPort();
	bindControlPort();
	LOG(debug,"Network ports open");
	
	LOG(message,"Waiting for network commands");
	while (keepRunning)
	{
		receive();
	}
	LOG(debug,"Shutting down");
	
	LOG(message,"hdpirun shut down");
	
	return 0;
}