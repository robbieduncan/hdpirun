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
#include "exit.h"
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
	
	LOG(debug,"Args Parsed");
	
	// OK note startup
	LOG(message,"hdpirun starting up");
	
	// Check the tuner hosts/IPs
	LOG(debug,"Checking Tuner Hosts");
	if (!addHomeRunTuners())
	{
		ABORT("Unable to verify at least one tuner");
	}
	LOG(debug,"Tuner(s) OK");
	
	// Start networking
	LOG(debug,"Opening network ports");
	if (!bindDiscoverPort())
	{
		ABORT("Failed to create/bind discovery port");
	}
	if (!bindControlPort())
	{
		ABORT("Failed to create/bind control port");
	}
	LOG(debug,"Network ports open");
	
	LOG(message,"Waiting for network commands");
	while (keepRunning)
	{
		receive();
	}
	LOG(debug,"Shutting down");
	
	EXIT("hdpirun shut down");
}