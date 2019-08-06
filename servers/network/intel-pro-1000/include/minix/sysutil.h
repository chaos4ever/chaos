// Based on the file with the same name from the Minix 3 source code

#pragma once

/*==========================================================================*
 * Miscellaneous helper functions.
 *==========================================================================*/

void env_setargs(int argc, char *argv[]);

int printf(const char *fmt, ...);
__dead void panic(const char *fmt, ...)
     __attribute__((__format__(__printf__,1,2)));
int tsc_calibrate(void);
