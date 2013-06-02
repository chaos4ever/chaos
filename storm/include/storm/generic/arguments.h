// Abstract: Function prototypes for command line argument parsing routines.

// © Copyright 1999-2000, 2013 chaos development.

#pragma once

#include <storm/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern u32 arguments_parse (char *source, char *destination, u32 delta);
extern char arguments_kernel[];

#ifdef __cplusplus
}
#endif
