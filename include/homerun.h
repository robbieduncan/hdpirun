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
// Adds an IP address to the list of addresses accepted. Returns 0 on success, 1 on failure. 
int addHomeRunTunerIP(char* ipAddress);
//
// Add a hostname ot the list of addresses accepted
int addHomeRunTunerHostname(char* hostname);
//
// Scan the network for HDHomeRunTuners and add valid tuners from the IP addresses, or all up to the maximum if no tuner specified
int addHomeRunTuners();
//
// Constructs a packet to use to respond to a discovery request. The caller is responsible for destroying the packet
struct hdhomerun_pkt_t* getDiscoveryReplyPacket();
//
#endif
