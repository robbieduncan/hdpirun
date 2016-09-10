// homerun.h
// HDHomeRun Wrapper

#include <stdint.h>
#include "libhdhomerun.h"

#ifndef HOMERUN_H
#define HOMERUN_H

int verifyHomerunTuner(int);
struct hdhomerun_discover_device_t getValidDevice();
	
#endif
