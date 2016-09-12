/*
   commandline.h
   hdpirun

   Utility functions to parse and validate the command line args and access to those parsed args

   Accepted args
	d : debug level
    l : log file
    h : hostname of HDHomeRun
    i : IP Address of HDHomeRun

   Author: Robbie Duncan
   Copyright: Copyright (c) 2016 Robbie Duncan
   License: See LICENSE
*/
#import "Log.h"
//
#ifndef COMMANDLINE_H
#define COMMANDLINE_H
//
// Parse the args
int parseArgs(int argc, char **argv);
//
#endif