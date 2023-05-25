/*
* ProxC - TCP/UDP Connections (Header)
*
* TODO:
*	- 
*	- 
*/


#ifndef PROXC_CONNS
#define PROXC_CONNS

#include <stdint.h>
#include <stddef.h>


int createListener(int lport);

int acceptConnection(int server_fd);

int tcpConnect(const char *dest_ip, int dport);


#endif //PROXC_CONNS
