/* 
   exit.h (was abort.h)
   hdpirun
  
   Utility header to abort the program on an unrecoverable error. Prints the message (printf format) and closes any open resources
  
   Author: Robbie Duncan
   Copyright: Copyright (c) 2016 Robbie Duncan
   License: See LICENSE
*/
#import "log.h"

#ifndef EXIT_H
#define EXIT_H
//
// Cleanup all open resources. Calls cleanUp in each module
void cleanUp();
//
// ABORT macro. Uses the LOG macro to display the message, does any cleanup then exits with failure
#define ABORT(msg,...) LOG(critical,msg,##__VA_ARGS__);cleanUp();exit(1);
//
// EXIT macro. Uses the LOG macro to display the message, does any cleanup then exits with success.
#define EXIT(msg,...) LOG(message,msg,##__VA_ARGS__);cleanUp();exit(1);
#endif
