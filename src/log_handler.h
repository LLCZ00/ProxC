/*
* ProxC - Error/Success Message Handler (Header)
*
* TODO:
*	- 
*	- 
*/

#ifndef PROXC_LOGHANDLER
#define PROXC_LOGHANDLER


#include <stdarg.h>


void logError(const char *format, ...);
void logMsg(const char *format, ...);

#endif // PROXC_LOGHANDLER