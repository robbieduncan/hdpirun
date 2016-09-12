#import "homerun.h"
#import "log.h"
#import "homerun.h"
#include <arpa/inet.h>
//
// Maimum numbfer of HDHomeRun devices supported
#define HOMERUN_MAX_DEVICES 10
//
// Internal variable to store requested IP addresses
int _ipAddresses[HOMERUN_MAX_DEVICES];
//
// How many IP addresses have been requested
int _requestedIPAddresses = 0;
//
// Array of discoved devices that we can use
struct hdhomerun_discover_device_t _validDevices[HOMERUN_MAX_DEVICES];
//
// Internal method to create a human readable description of a HDHomeRun validated device
void _hdHomeRunDeviceDescription(struct hdhomerun_discover_device_t device, char* buffer)
{
	char initAddress[INET_ADDRSTRLEN];
	int ip = htonl(device.ip_addr);
	
	inet_ntop(AF_INET, &(ip), initAddress, INET_ADDRSTRLEN);
	
	sprintf(buffer,"ID:%i IP:%s Tuners:%i",device.device_id,initAddress,device.tuner_count);
}
//
// Adds an IP address to the list of addresses accepted. Returns 1 on success, 0 on failure. 
int addHomeRunTunerIP(char* ipAddress)
{
	if (_requestedIPAddresses >= HOMERUN_MAX_DEVICES)
	{
		LOG(error,"Compiled support for %i HDHomeRunDevices. Cannot request more than this.",HOMERUN_MAX_DEVICES);
		return 0;
	}
	// See if this is a valid IP Address
	int parsedIP;
	if (!inet_pton(AF_INET, ipAddress, &parsedIP))
	{
		LOG(critical,"Could not interpret %s as an IP address",ipAddress);
		return 0;
	}
	// Turn into network byte order
	parsedIP = htonl(parsedIP);
	_ipAddresses[_requestedIPAddresses] = parsedIP;
	_requestedIPAddresses++;
	LOG(debug,"Added HDHomeRun IP Address '%s'",ipAddress);
	return 1;
}
// Add a hostname ot the list of addresses accepted
int addHomeRunTunerHostname(char* hostname)
{	
	if (_requestedIPAddresses >= HOMERUN_MAX_DEVICES)
	{
		LOG(error,"Compiled support for %i HDHomeRunDevices. Cannot request more than this.",HOMERUN_MAX_DEVICES);
		return 0;
	}
	// Is this a valid hostname (can we look up it's IP)?
	struct hostent *hostDetails;
	if ((hostDetails = gethostbyname(hostname)) == NULL)
	{
		LOG(critical,"Could not get IP address for host %s",hostname);
		return 0;
	}
	struct in_addr **addr_list;
    addr_list = (struct in_addr **)hostDetails->h_addr_list;
	// Use the first address
	int parsedIP = htonl((*addr_list[0]).s_addr);
	_ipAddresses[_requestedIPAddresses] = parsedIP;
	_requestedIPAddresses++;
	LOG(debug,"Added HDHomeRun hostname '%s' as IP '%s'",hostname,(*addr_list[0]).s_addr);
	
	return 1;
}
//
// Scan the network for HDHomeRunTuners and add valid tuners from the IP addresses, or all up to the maximum if no tuner specified
int addHomeRunTuners()
{
	struct hdhomerun_discover_device_t foundDevices[2*HOMERUN_MAX_DEVICES]; // Scan for two times as many devices as there are. It is possible this is still not enough
	LOG(trace,"About to scan for HD Homerun devices");
	// Find all devices on the network
	int no_devices = hdhomerun_discover_find_devices_custom_v2(0, HDHOMERUN_DEVICE_TYPE_TUNER, HDHOMERUN_DEVICE_ID_WILDCARD, foundDevices, HOMERUN_MAX_DEVICES);
	LOG(trace,"%i HDHomeRun devices discovered",no_devices);
	if (no_devices<1)
	{
		LOG(critical,"No HDHomerun devices found on the network");
		return 0;
	}
	
	// If a host or IP address was supplied on the command line we need to check if each found host to see if it's one of the valid hosts
	if (_requestedIPAddresses > 0)
	{
		// Need to check each found device against the list of valid IP addresses.
		int added_devices = 0;
		for (int i = 0;i < no_devices && added_devices < HOMERUN_MAX_DEVICES;i++)
		{
			int foundIP = foundDevices[0].ip_addr;
			for (int j = 0 ;j < _requestedIPAddresses; j++)
			{
				if (foundIP == _ipAddresses[j])
				{
					_validDevices[added_devices] = foundDevices[i];
					char devBuffer[255];
					_hdHomeRunDeviceDescription(_validDevices[added_devices],devBuffer);
					LOG(message,"Found valid HDHomeRun device %s",devBuffer);
					added_devices++;
				}
			}
		}
	}
	else
	{
		LOG(trace,"No specific IPs or hostnames requested: adding any found devices up to compiled in max %i",HOMERUN_MAX_DEVICES);
		// No specified IP adresses or hostnames. Simply add all to the valid devices up to the device limit
		for (int i=0; i<HOMERUN_MAX_DEVICES && i<no_devices; i++)
		{
			_validDevices[i] = foundDevices[i];
			char devBuffer[255];
			_hdHomeRunDeviceDescription(_validDevices[i],devBuffer);
			LOG(message,"Found valid HDHomeRun device %s",devBuffer);
		}
	}
	return 1;
}

struct hdhomerun_pkt_t* getDiscoveryReplyPacket()
{
	// Build a discovery response packet to reply with
	struct hdhomerun_pkt_t *pkt = hdhomerun_pkt_create();
	
	// Get the cached valid device structure checked at startup
	struct hdhomerun_discover_device_t device = _validDevices[0];
	
	// The packet frame contains TLV entries.

	// Write the device type TLV
	hdhomerun_pkt_write_u8(pkt,HDHOMERUN_TAG_DEVICE_TYPE);
	hdhomerun_pkt_write_var_length(pkt,sizeof(uint32_t));
	hdhomerun_pkt_write_u32(pkt,device.device_type);
	
	// Write the device ID
	hdhomerun_pkt_write_u8(pkt,HDHOMERUN_TAG_DEVICE_ID);
	hdhomerun_pkt_write_var_length(pkt,sizeof(uint32_t));
	hdhomerun_pkt_write_u32(pkt,device.device_id);
	
	// Write the tuner count
	hdhomerun_pkt_write_u8(pkt,HDHOMERUN_TAG_TUNER_COUNT);
	hdhomerun_pkt_write_var_length(pkt,sizeof(uint8_t));
	hdhomerun_pkt_write_u32(pkt,device.tuner_count);
	
	// Write the auth data
	hdhomerun_pkt_write_u8(pkt,HDHOMERUN_TAG_DEVICE_AUTH_BIN);
	hdhomerun_pkt_write_var_length(pkt,18*sizeof(uint8_t));
	// To Do: encode the string
	for (int i=0;i<18;i++)
	{
		hdhomerun_pkt_write_var_length(pkt,0);
	}
	
	// Seal the packet frame ready to send
	hdhomerun_pkt_seal_frame(pkt,HDHOMERUN_TYPE_DISCOVER_RPY);
	
	return pkt;
}
