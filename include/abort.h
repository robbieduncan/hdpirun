// abort.h
// hdpirun
//
// Utility header to abort the program on an unrecoverable error. Prints the message (printf format) and closes any open resources
//
// Author: Robbie Duncan
// Copyright: Copyright (c) 2016 Robbie Duncan
// License: See LICENSE
//
#import <stdio.h>
#import "log.h"

#ifndef ABORT_H
#define ABORT_H
#define ABORT(message,...) LOG(critical,message,##__VA_ARGS__);if (logFile!=stdout){fclose(logFile);};exit(1);
#endif
