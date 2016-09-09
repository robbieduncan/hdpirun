#include "log.h"

int __LOG_TRACE = 0;
int __LOG_DEBUG = 1;
int __LOG_MESSAGE = 2;
int __LOG_WARN = 3;
int __LOG_ERROR = 4;
int __LOG_CRITICAL = 5;
char __LOG_NAMES[6][9] = {"TRACE","DEBUG","MESSAGE","WARN","ERROR","CRITICAL"};

int __logLevel = 2;
FILE* __logFile;


void __logInit()
{
	__logFile = stdout;
}
