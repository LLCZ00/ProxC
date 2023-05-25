/*
* ProxC - SOCKS4 Protocol Functions
*
* TODO:
*	- Fork somewhere
*	- 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include "connections.h" // createListener, acceptConnection, tcpConnect
#include "socks.h"
#include "log_handler.h" // logError, logMsg


#define BUFFSIZE 1024

enum {
    RC_GRANTED = 0x5a,
    RC_FAILED,
    RC_FAILED_NO_IDENT,
    RC_FAILED_IDENT
};

typedef struct {
	uint8_t ver; // SOCKS version number (4)
	uint8_t cmd_code; // command code
	uint16_t dport; // Destination port
	uint32_t dip; // Destination ip
	char id[256]; // User ID string	
} initial_request;


typedef struct {
	uint8_t ver; // Reply version (NULL)
	uint8_t rep_code; // Reply code
	uint16_t dport; // Destination port (meaningful only after BIND command)
	uint32_t dip; // Destination IP
} initial_response;


static void sendInitialResponse(int client_fd, initial_response *resp);
Socks4Proxy *initialExchange(int client_fd);


void proxyLoop(Socks4Proxy *proxy)
{
	uint8_t data_buffer[BUFFSIZE];
	struct pollfd poll_list[2];
	int retval;
	ssize_t bytesread;
	int running = 1;

	poll_list[0].fd = proxy->client_fd;
	poll_list[1].fd = proxy->dest_fd;
	poll_list[0].events = POLLIN;
	poll_list[1].events = POLLIN;

	while(running)
	{
		retval = poll(poll_list, (unsigned int)2, -1);
		if (retval < 0)
		{
			logError("Error occured while polling connections");
		}

		for (int i = 0; i < 2; i++)
		{
			if (poll_list[i].revents & POLLIN)
			{
				memset(data_buffer, 0, BUFFSIZE);
				bytesread = read(poll_list[i].fd, data_buffer, BUFFSIZE);
				if (bytesread < 1)
				{
					running = 0;
				}
				else
				{
					if (i) { write(poll_list[0].fd, data_buffer, bytesread); }
					else { write(poll_list[1].fd, data_buffer, bytesread); }
				}
			}
		}
	}
	for (int i = 0; i < 2; i++)
	{
		close(poll_list[i].fd);
	}
	if (proxy->server_fd)
	{
		close(proxy->server_fd);
	}
	free(proxy);
	proxy = NULL;
}


int socks4Server(int lport)
{
	int server_fd = createListener(lport);
	if (server_fd < 0) { return -1; }

	int client_fd = acceptConnection(server_fd);
	if (client_fd < 0) { return -1; }

	Socks4Proxy *proxy = initialExchange(client_fd);
	if (proxy == NULL)
	{
		close(client_fd);
		logError("SOCKS4 initial exchange failed");
	}

	proxyLoop(proxy);
	close(server_fd);
	return 0;
}


static void sendInitialResponse(int client_fd, initial_response *resp)
{
	resp->ver = 0x00;
	if (send(client_fd, resp, sizeof(initial_response), 0) < 0)
	{
		logError("Failed to send response to initial request");
	}
}


Socks4Proxy *initialExchange(int client_fd)
{
	initial_request init_req;
	initial_response init_resp;

	// 1. Recieve and validate initial request packet
	if (recv(client_fd, &init_req, sizeof(init_req), 0) < 0)
	{
		logError("Failed to recieve SOCKS4 initial request");
		return NULL;
	}

	if (init_req.ver != 4)
	{
		logError("Client init_request: Invalid protocol/version number '%hhu'", init_req.ver);
		init_resp.rep_code = RC_FAILED;
		sendInitialResponse(client_fd, &init_resp);
		return NULL;
	}

	// 2. Establish connection to destination host, or bind socket and create listener
	Socks4Proxy *proxy = malloc(sizeof(Socks4Proxy));
	if (proxy == NULL)
	{
		logError("Error allocating memory");
		return NULL;
	}
	proxy->client_fd = client_fd;
	struct in_addr address;
	address.s_addr = init_req.dip;
	init_resp.rep_code = RC_FAILED;

	if (init_req.cmd_code == 0x01)
	{
		proxy->dest_fd = tcpConnect(inet_ntoa(address), htons(init_req.dport));
		if (proxy->dest_fd < 0)
		{
			sendInitialResponse(client_fd, &init_resp);
			free(proxy);
			return NULL;
		}
		init_resp.dport = init_req.dport;
		init_resp.dip = init_req.dip;
	}
	else if (init_req.cmd_code == 0x02)
	{
		proxy->server_fd = createListener(0);
		if (proxy->server_fd < 0) { free(proxy); return NULL; }

		struct sockaddr_in sin;
		socklen_t len = sizeof(sin);
		if (getsockname(proxy->server_fd, (struct sockaddr *)&sin, &len) < 0)
		{
			logError("Error getting BIND port number");
			sendInitialResponse(client_fd, &init_resp);
			free(proxy);
			return NULL;
		}

		proxy->dest_fd = acceptConnection(proxy->server_fd);
		if (proxy->server_fd < 0) { free(proxy); return NULL; }

		init_resp.dport = sin.sin_port;
		init_resp.dip = INADDR_ANY;
	}
	else
	{
		logError("Client init_request: Invalid command code '%hhu'", init_req.cmd_code);
		init_resp.rep_code = RC_FAILED;
		sendInitialResponse(client_fd, &init_resp);
		free(proxy);
		return NULL;
	}

	init_resp.rep_code = RC_GRANTED;
	sendInitialResponse(client_fd, &init_resp);

	return proxy;
}
