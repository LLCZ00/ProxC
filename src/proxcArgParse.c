/*
* ProxC - CLI Argument Handler
*
* TODO:
*	- Add option for adding authenticated users (--add )
*	- 
*/

// printf("Listening port: %d\n", args.lport);
// printf("SOCKS version: %d\n", args.version);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "proxcArgParse.h"


#define SOCKSPORT 1080 // Default SOCKS port, as per RFC 1928
#define SOCKSVERSION 4 // Default SOCKS protocol version


static const char *HELP[] = {
	"~ ProxC v1.0.0, by LLCZ00 ~",
	"",
	"Description: A basic SOCKS4/5 proxy server",
	"",
	"Usage: ./proxc [-h] [-v PROTOCOL] [-p PORT]",
	"",
	"Options:", 
	"\t-h, --help                Show this message and exit.",
	"\t-v, --version PROTOCOL    SOCKS protocol version to use (4 or 5). Default: 4",
	"\t-p, --port PORT           Port number to listen on. Default: 1080",
	"",
	0
};


static void logError(const char *format, ...) // Error handler
{
	va_list arg;
	va_start(arg, format);
    vfprintf(stderr, format, arg);
    va_end(arg);
    fputs("\n", stderr);

    fputs("Try './proxc --help' for more information\n", stderr);
}


int parseArguments(int argc, char **argv, ProxcArgs *args)
{
	/* Assign default values */
	args->lport = SOCKSPORT;
	args->version = SOCKSVERSION;

	int rval = 0;
	int index = 1;

	while(1)
	{
		if (index == argc || rval == -1) { break; }

		/* Display help message */
		else if ((strncmp(argv[index], "-h\0", 3) == 0) || (strncmp(argv[index], "--help\0", 7)) == 0)
		{
			for (int i = 0; HELP[i]; i++) 
			{
				printf("%s\n", HELP[i]); 
			}
			rval = -1;
		}

		/* Set SOCKS version */
		else if ((strncmp(argv[index], "-v\0", 3) == 0) || (strncmp(argv[index], "--version\0", 10)) == 0)
		{
			if (index+1 == argc)
			{
				logError("Argument '%s' requires additional argument 'PROTOCOL'", argv[index]);
				rval = -1;
			}
			else
			{
				int proto = atoi(argv[index+1]);
				if (proto == 4 || proto == 5)
				{
					args->version = proto;
					index += 2;	
				}
				else
				{
					logError("Invalid protocol version '%s'", argv[index+1]);
					rval = -1;
				}
			}			
		}

		/* Set listening port */
		else if ((strncmp(argv[index], "-p\0", 3) == 0) || (strncmp(argv[index], "--port\0", 7)) == 0)
		{
			if (index+1 == argc)
			{
				logError("Argument '%s' requires additional argument 'PORT'", argv[index]);
				rval = -1;
			}
			else
			{
				if (strncmp(argv[index+1], "0\0", 2) == 0)
				{
					args->lport = 0;
					index +=2;
				}
				else
				{
					int port = atoi(argv[index+1]);
					if (port < 1 || port > 65535)
					{
						logError("Invalid port number '%s'", argv[index+1]);
						rval = -1;
					}
					else
					{
						args->lport = port;
						index +=2;
					}
					
				}
				
			}
			
		}

		/* Unknown argument */
		else
		{
			logError("Unknown argument '%s'", argv[index]);
			rval = -1;
		}
	}
	return rval;
}

