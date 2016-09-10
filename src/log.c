#include "log.h"

char __LOG_NAMES[6][9] = {"TRACE","DEBUG","MESSAGE","WARN","ERROR","CRITICAL"};

FILE *logFile;
// "Shared" variable that controls logging level
LogLevel logLevel;

void __logInit()
{
	logLevel = message;
	logFile = stdout;
}
