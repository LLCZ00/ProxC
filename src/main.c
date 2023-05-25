/*
* ProxC - Basic SOCKS proxy server
*
* TODO:
*	- Establish self as linux service, ala sshd
*	- /etc/passwd-like file for authentication (username:password-hash)
*	- 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "proxcArgParse.h" // ProxcArgs, parseArguments
#include "socks.h"
#include "log_handler.h" // logError, logMsg



int main(int argc, char **argv)
{
	ProxcArgs args;
	if (parseArguments(argc, argv, &args) == -1) { return 1; }

	if (args.version == 4)
	{
		socks4Server(args.lport);
	}
	else if (args.version == 5)
	{
		logError("SOCKS5 protocol not yet implimented");
	}
	else
	{
		logError("Unknown argument error");
		return 1;
	}

	logMsg("ProxC server exited gracefully");
	return 0;
}


