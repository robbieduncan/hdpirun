#import "homerun.h"
#import "log.h"
#import "abort.h"
#include <arpa/inet.h>

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
	struct hdhomerun_discover_device_t foundDevices[10];
	LOG(trace,"About to scan for HD Homerun devices");
	// Find all devices on the network
	int no_devices = hdhomerun_discover_find_devices_custom_v2(0, HDHOMERUN_DEVICE_TYPE_TUNER, HDHOMERUN_DEVICE_ID_WILDCARD, foundDevices, 10);
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

struct hdhomerun_discover_device_t getValidDevice()
{
	return __validDevice;
}