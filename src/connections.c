/*
* ProxC - TCP/UDP Connections
*
* TODO:
*	- Add timeout to tcpConnect
*	- 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "connections.h"
#include "log_handler.h" // logError, logMsg


int createListener(int lport)
{
	int server_socket_fd;
	struct sockaddr_in server_addr;
	
	server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket_fd < 0)
	{
		logError("Error occured while creating socket");
		return -1;
	}

	const int enable = 1;
	if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		logError("Error setting socket options (SO_REUSEADDR)");
		return -1;
	}
	if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
	{
		logError("Error setting socket options (SO_REUSEPORT)");
		return -1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(lport);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(server_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		logError("Error occured while binding socket");
		close(server_socket_fd);
		return -1;
	}

	if (listen(server_socket_fd, 1) < 0)
	{
		logError("Failed to establish listening port");
		close(server_socket_fd);
		return -1;
	}

	logMsg("Listening on port %d", lport);

	return server_socket_fd;
}


int acceptConnection(int server_fd)
{
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_size = sizeof(client_addr);

	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_size);
	if (client_fd < 0)
	{
		logError("Failed to accept connection from SOCKS client");
		return -1;
	}

	logMsg("SOCKS Client connection from %s:%hu", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

	return client_fd;
}


int tcpConnect(const char *dest_ip, int dport)
{
	int dest_fd;
	struct sockaddr_in dest_addr;

	dest_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (dest_fd < 0)
	{
		logError("Error occured while creating socket");
		return -1;
	}

	dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(dport);
    dest_addr.sin_addr.s_addr = inet_addr(dest_ip);

    if(connect(dest_fd, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0)
    {
        logError("Failed to connect to host %s:%d", dest_ip, dport);
        close(dest_fd);
        return -1;
    }

    logMsg("SOCKS destination connected at %s:%hu", inet_ntoa(dest_addr.sin_addr), ntohs(dest_addr.sin_port));

	return dest_fd;
}

