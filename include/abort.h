#import "log.h"
#import <stdio.h>

#ifndef ABORT_H
#define ABORT_H
#define __ABORT(message,...) __LOG(__LOG_CRITICAL,message,##__VA_ARGS__);if (__logFile!=stdout){fclose(stdout);};return 1;
#endif
