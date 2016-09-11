#import "homerun.h"
#import "log.h"
#include <arpa/inet.h>

#define HOMERUN_MAX_DEVICES 10

struct hdhomerun_discover_device_t __validDevice;

void hdHomeRunDeviceDescription(struct hdhomerun_discover_device_t device, char* buffer)
{
	char initAddress[INET_ADDRSTRLEN];
	int ip = htonl(device.ip_addr);
	
	inet_ntop(AF_INET, &(ip), initAddress, INET_ADDRSTRLEN);
	
	sprintf(buffer,"ID:%i IP:%s Tuners:%i",device.device_id,initAddress,device.tuner_count);
}

int verifyHomerunTuner(int ipAddress)
{
	struct hdhomerun_discover_device_t foundDevices[HOMERUN_MAX_DEVICES];
	LOG(trace,"About to scan for HD Homerun devices");
	// Find all devices on the network
	int no_devices = hdhomerun_discover_find_devices_custom_v2(0, HDHOMERUN_DEVICE_TYPE_TUNER, HDHOMERUN_DEVICE_ID_WILDCARD, foundDevices, HOMERUN_MAX_DEVICES);
	LOG(trace,"%i HDHomeRun devices discovered",no_devices);
	if (no_devices<1)
	{
		LOG(critical,"No HDHomerun devices found on the network");
		return 0;
	}
	
	// If a host or IP address was supplied on the command line we need to check if that's one of the devices found. If not we will simply use the first one in the list
	if (ipAddress!=-1)
	{
		// Loop over all found devices and see if any match the requeted IP address
		int i;
		int found = 0;
		for (i=0;i<no_devices;i++)
		{
			if (foundDevices[0].ip_addr == ipAddress)
			{
				LOG(debug,"Found device matching specified ip/hostname");
				__validDevice = foundDevices[i];
				found++;
			}
		}
		if (found == 0)
		{
			LOG(critical,"No HDHomeRun devices found on the network for the specified ip/hostname");
			return 0;
		}
	}
	else
	{
		__validDevice = foundDevices[0];
		char devBuffer[255];
		hdHomeRunDeviceDescription(__validDevice,devBuffer);
		LOG(message,"Using HDHomeRun device %s",devBuffer);
	}
	return 1;
}

struct hdhomerun_pkt_t* getDiscoveryReplyPacket()
{
	// Build a discovery response packet to reply with
	struct hdhomerun_pkt_t *pkt = hdhomerun_pkt_create();
	
	// Get the cached valid device structure checked at startup
	struct hdhomerun_discover_device_t device = getValidDevice();
	
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

struct hdhomerun_discover_device_t getValidDevice()
{
	return __validDevice;
}