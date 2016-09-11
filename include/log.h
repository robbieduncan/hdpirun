/*
   log.h
   hdpirun
  
   Utility header to log messages. Supports multiple log levels, control over which messages are printed via the _logLevel variable, logging to file or stderr (stderr is the default) and printf format in messages
  
   Author: Robbie Duncan
   Copyright: Copyright (c) 2016 Robbie Duncan
   License: See LICENSE
*/
#import <stdlib.h>
#import <stdio.h>
#include <time.h>
#include <string.h>
//
#ifndef LOG_H
#define LOG_H
//
// Public types, variables and macros
//
// Log Levels
typedef enum loglevel {
	trace = 0,
	debug,
	message,
	warn,
	error,
	critical
} LogLevel; 
//
// Set the log level
int setLogLevel(LogLevel logLevel);
//
// Get the log level
LogLevel getLogLevel();
//
// Set the file path for log output
int setLogFile(char *logFilePath);
//
// Get the log file
FILE* getLogFile();
//
// Function to clean up any log resources on app shutdown
void log_cleanUp();
//
// Logging macro
#define LOG(level,message,...) \
if ( getLogFile() == NULL )\
	{\
		__logInit();\
	} \
if ( level >= getLogLevel() )\
	{\
		char *__log_totalMessage;\
		int __log_totalMessageLength = strlen(message)+13;\
		__log_totalMessage = malloc(sizeof(char)*__log_totalMessageLength);\
		sprintf(__log_totalMessage,"%s\t%s\n","%9s %20s",message);\
		time_t __log_timer;\
		char _log_buffer[20];\
		struct tm* tm_info;\
		time(&__log_timer);\
		tm_info = localtime(&__log_timer);\
		strftime(_log_buffer, 20, "%Y:%m:%d %H:%M:%S", tm_info);\
		fprintf(getLogFile(),__log_totalMessage,__LOG_NAMES[level],_log_buffer,##__VA_ARGS__);\
		free(__log_totalMessage);\
	}
//
// Private internal variables and functions below here
//
// Initialise logging
void __logInit();
//
// Variable to hold human readable names for log levels
char __LOG_NAMES[6][9];
//
#endif
