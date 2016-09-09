// log.h
// Logging utils
#import <stdlib.h>
#import <stdio.h>
#include <time.h>
#include <string.h>

#ifndef LOG_H
#define LOG_H
// Log Levels
extern int __LOG_TRACE;
extern int __LOG_DEBUG;
extern int __LOG_MESSAGE;
extern int __LOG_WARN;
extern int __LOG_ERROR;
extern int __LOG_CRITICAL;
extern char __LOG_NAMES[6][9];

#define __LOG(level,message,...) if (__logFile==NULL) {__logInit();} if (level>=__logLevel) { char *totalMessage;int totalMessageLength = strlen(message)+13;totalMessage=malloc(sizeof(char)*totalMessageLength);sprintf(totalMessage,"%s\t%s\n","%9s %20s",message);time_t timer; char buffer[20]; struct tm* tm_info; time(&timer); tm_info = localtime(&timer); strftime(buffer, 20, "%Y:%m:%d %H:%M:%S", tm_info); fprintf(__logFile,totalMessage,__LOG_NAMES[level],buffer,##__VA_ARGS__);free(totalMessage);}

// Client can set this variable to set log level
extern int __logLevel;

// Default log file is stdout
extern FILE* __logFile;
// Initialise logging
extern void __logInit();

#endif
