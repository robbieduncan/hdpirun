
#include <net/if.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#ifndef SIOCGIFCONF
#include <sys/sockio.h>
#endif
#import "network.h"
#import "log.h"
#import "abort.h"
#import "homerun.h"
#import "hdhomerun_pkt.h"

int __udpServerSocket;
int __tcpServerSocket;

// Binds to the UDP Control Port
void bindDiscoverPort()
{
	// Create a socket
	__udpServerSocket = socket(AF_INET,SOCK_DGRAM,0);
    if (__udpServerSocket < 0)
	{
		ABORT("Unable to create UDP socket for network discovery");
  	}
	
	// Create a server address struct
	struct sockaddr_in serverAddress;
	bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sin_port = htons(HDHOMERUN_DISCOVER_UDP_PORT);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	// Bind the socket
    if (bind(__udpServerSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
	{
		ABORT("Cannot bind to port %i for UDP",HDHOMERUN_DISCOVER_UDP_PORT);
	}
	LOG(trace,"UDP discovery port %i bound",HDHOMERUN_DISCOVER_UDP_PORT);
	
	// Set socket to non blocking
    if ( fcntl( __udpServerSocket, F_SETFL, O_NONBLOCK, 1 ) == -1 )
    {
        ABORT("Cannot set UDP discovery port %i to non blocking",HDHOMERUN_DISCOVER_UDP_PORT);
    }
	
	return;
}

void bindControlPort()
{
	// Create a socket
	__tcpServerSocket = socket(AF_INET,SOCK_STREAM,0);
    if (__tcpServerSocket < 0)
	{
		ABORT("Unable to create TCP socket for network commands");
  	}
	
	// Create a server address struct
	struct sockaddr_in serverAddress;
	bzero((char *) &serverAddress, sizeof(serverAddress));
	serverAddress.sin_port = htons(HDHOMERUN_CONTROL_TCP_PORT);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	// Bind the socket
    if (bind(__tcpServerSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
	{
		ABORT("Cannot bind to port %i for TCP",HDHOMERUN_CONTROL_TCP_PORT);
	}
	LOG(trace,"TCP control port %i bound",HDHOMERUN_CONTROL_TCP_PORT);
	
	// Set socket to non blocking
    if ( fcntl( __tcpServerSocket, F_SETFL, O_NONBLOCK, 1 ) == -1 )
    {
        ABORT("Cannot set TCP control port %i to non blocking",HDHOMERUN_CONTROL_TCP_PORT);
    }
		
    //try to specify maximum of 3 pending connections for the master socket
    if (listen(__tcpServerSocket, 3) < 0)
    {
		ABORT("Cannot listen on TCP");
    }
		
	return;
}

int handleDiscoveryRequest(struct sockaddr_in *sock_addr)
{
	// Build a discovery response packet to reply with
	struct hdhomerun_pkt_t *pkt = hdhomerun_pkt_create();
	
	// Get the cached valid device structure checked at startup
	struct hdhomerun_discover_device_t device = getValidDevice();
	
	// The packet frame contains TLV entries. I think this is Tag Length Value
	// Tag is a u8
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
	
	// Send the packet
	//return hdhomerun_sock_sendto(dss->sock, target_ip, HDHOMERUN_DISCOVER_UDP_PORT, tx_pkt->start, tx_pkt->end - tx_pkt->start, 0);
	const uint8_t *ptr = (const uint8_t *)pkt->start;
	int length = pkt->end - pkt->start;
	LOG(trace,"Length of data to send is %i",length);
	LOG(trace,"Sending discovery reply");
	
	// Create a UDP socket for sending
	socklen_t sendSocket = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in myaddr;
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);
	
	// Setup the send structure
	struct sockaddr_in send_sock;
	memset((char *)&send_sock, 0, sizeof(send_sock));
	send_sock.sin_family = AF_INET;
	send_sock.sin_addr = sock_addr->sin_addr;
	send_sock.sin_port = sock_addr->sin_port;
	
	int ret = sendto(sendSocket, ptr, length, 0, (struct sockaddr *)&send_sock, sizeof(send_sock));
	if (ret == -1)
	{
		perror("Error:");
		LOG(error,"Error sending discovery reply. Error number %i",ret);
	}
	else
	{
		LOG(trace,"Sent %i bytes",ret);
	}
	// Destroy the packet
	hdhomerun_pkt_destroy(pkt);
	return 0;
}

int handleUDP()
{
	// Try and build a packet
	struct hdhomerun_pkt_t *pkt = hdhomerun_pkt_create();
	
	struct sockaddr clientAddress;
	socklen_t fromLen = sizeof(clientAddress);
	// Read into the packet
	size_t length = pkt->limit - pkt->end;
	int readCount = recvfrom(__udpServerSocket, pkt->end, length, 0, &clientAddress, &fromLen);
	pkt->end +=readCount;
	struct sockaddr_in *ipv4 = (struct sockaddr_in *)(&clientAddress);
	char ipAddress[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(ipv4->sin_addr), ipAddress, INET_ADDRSTRLEN);
	int port = ntohs(ipv4->sin_port);
	
	LOG(trace,"UDP Packet recieved. Byte count:%i. Client address:%s. Client port:%i",readCount,ipAddress,port);
	
	uint16_t type;
	if (hdhomerun_pkt_open_frame(pkt, &type) <= 0) {
		LOG(error,"Could not open frame");
		return 0;
	}
	
	switch(type)
	{
		case HDHOMERUN_TYPE_DISCOVER_REQ:
			LOG(trace,"Discovery request received");
			handleDiscoveryRequest(ipv4);
			break;
		default:
			LOG(trace,"Unknow request received: %i",type)
	}
		
	// ToDo: We should destroy that packet!
	hdhomerun_pkt_destroy(pkt);
	return 1;
}

void receive()
{
	LOG(trace,"Network IO Handler entered")
	//set of socket descriptors
    fd_set readfds;
	// Clear the socket set
	FD_ZERO(&readfds);
	// Add the discovery and control ports
    FD_SET(__udpServerSocket, &readfds);
	FD_SET(__tcpServerSocket, &readfds);
	
	errno = 0;
	int activity = select( FD_SETSIZE, &readfds , NULL , NULL , NULL);
	
	if ((activity < 0) && (errno!=EINTR)) 
	{
		LOG(error,"Error in select. Continuing");
	}
	else
	{
		if (FD_ISSET(__udpServerSocket, &readfds))
		{
			// UDP socket can be read from
			LOG(trace,"UDP ready to read");
			handleUDP();
		}
		
		if (FD_ISSET(__tcpServerSocket, &readfds))
		{
			// TCP socket can be read from
			LOG(trace,"TCP ready to read");
		}
	}
			
}