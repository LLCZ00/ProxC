/*
* ProxC - Error/Success Message Handler
*
* TODO:
*	- 
*	- 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "log_handler.h"


void logError(const char *format, ...)
{
	fprintf(stderr, "[!] ");
	va_list arg;
	va_start(arg, format);
    vfprintf(stderr, format, arg);
    va_end(arg);
    fputs("\n", stderr);
}


void logMsg(const char *format, ...)
{
	fprintf(stdout, "[*] ");
	va_list arg;
	va_start(arg, format);
    vfprintf(stdout, format, arg);
    va_end(arg);
    fputs("\n", stdout);
}
