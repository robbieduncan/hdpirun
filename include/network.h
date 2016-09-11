/*
   network.h
   hdpirun
  
   Network communication
  
   Author: Robbie Duncan
   Copyright: Copyright (c) 2016 Robbie Duncan
   License: See LICENSE
*/
#ifndef NET_H
#define NET_H
//
// Create and bind the UDP discovery port ready for receive().
int bindDiscoverPort();
//
// Create and bind the TCP control port ready for receive()
int bindControlPort();
//
// Receive a packet. Will block until a packet is received and handle that
void receive();
//
#endif
