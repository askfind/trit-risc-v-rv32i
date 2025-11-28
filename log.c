#include "error.h"
#include <stdarg.h>
#include <stdio.h>

#define LOGGING 1

int flag_quiet = 0;


#if 1 //viv- old version 
void log_printf(char *fmt, ...) {
    
    
    //if (flag_quiet) return;

	//TODO error что-то с выводом 
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}
#else //viv+ new version
void log_printf(x) {
            //do { 
			//	if (LOGGING) fprintf(stderr, fmt, __VA_ARGS__); 
			//} while (0);
			do { if (LOGGING) dbg_printf(x); } while (0);
			
}	
#endif
