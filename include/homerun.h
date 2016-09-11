/*
   homerrun.h
   hdpirun
  
   HDHomeRun wrapper. Wraps library functionality for use
  
   Author: Robbie Duncan
   Copyright: Copyright (c) 2016 Robbie Duncan
   License: See LICENSE
*/
#include <stdint.h>
#include "libhdhomerun.h"
//
#ifndef HOMERUN_H
#define HOMERUN_H
//
// Called to verify there is a tuner on the network. Int is the IP address of the tuner expected. This is optional: pass -1 if any tuner is acceptable
int verifyHomerunTuner(int);
//
// Constructs a packet to use to respond to a discovery request. The caller is responsible for destroying the packet
struct hdhomerun_pkt_t* getDiscoveryReplyPacket();
//
// Returns the device validated by verifyHomerunTuner
struct hdhomerun_discover_device_t getValidDevice();
//
#endif
