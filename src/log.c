
#include <stdio.h>
#include "log.h"

char __LOG_NAMES[6][9] = {"TRACE","DEBUG","MESSAGE","WARN","ERROR","CRITICAL"};

// Internal variables
LogLevel _logLevel = message;
FILE *_logFile;

int setLogLevel(LogLevel logLevel)
{
	if (logLevel<trace || logLevel>critical)
	{
		LOG(warn,"Attempt to set log level to an illegal value");
		return 0;
	}
	LOG(message,"Setting logLevel to %s",__LOG_NAMES[logLevel]);
	_logLevel = logLevel;
	return 1;
}

LogLevel getLogLevel()
{
	return _logLevel;
}

int setLogFile(char *logFilePath)
{
	FILE *tempLog = fopen(logFilePath,"a");
	if (tempLog == NULL)
	{
		LOG(critical,"Cannot open log file '%s' for append",logFilePath);
		return 0;
	}
	else
	{
		_logFile = tempLog;
	}
	return 1;
}

FILE* getLogFile()
{
	return _logFile;
}

void log_cleanUp()
{
	FILE *logFile = getLogFile();
	if (logFile != NULL && logFile != stderr)
	{
		fclose(logFile);
	}
}

void __logInit()
{
	_logFile = stdout;
}
