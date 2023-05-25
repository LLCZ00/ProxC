/*
* ProxC - SOCKS Protocol Functions (Header)
*
* TODO:
*	- Impliment both SOCKS4 and SOCKS5 API functions
*	- 
*/

#ifndef PROXC_SOCKS4
#define PROXC_SOCKS4

typedef struct {
	int server_fd;
	int client_fd;
	int dest_fd;
} Socks4Proxy;

void proxyLoop(Socks4Proxy *proxy);

int socks4Server(int lport);


#endif //PROXC_SOCKS4
