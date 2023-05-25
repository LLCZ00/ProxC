/*
* ProxC - CLI Argument Handler (Header)
*/


#ifndef PROXC_ARGPARSE
#define PROXC_ARGPARSE

typedef struct {
	int lport;
	int version;
} ProxcArgs;


int parseArguments(int argc, char **argv, ProxcArgs *args);


#endif //PROXC_ARGPARSE
