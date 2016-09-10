// abort.h
// hdpirun
//
// Utility header to abort the program on an unrecoverable error. Prints the message (printf format) and closes any open resources
//
// Author: Robbie Duncan
// Copyright: Copyright (c) 2016 Robbie Duncan
// License: See LICENSE
//
#import "log.h"
#import <stdio.h>

#ifndef ABORT_H
#define ABORT_H
#define __ABORT(message,...) __LOG(__LOG_CRITICAL,message,##__VA_ARGS__);if (__logFile!=stdout){fclose(__logFile);};exit(1);
#endif
