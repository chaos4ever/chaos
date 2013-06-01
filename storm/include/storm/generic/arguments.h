/* Abstract: Function prototypes for command line argument parsing
   routines. */

/* Copyright 1999-2000, 2013 chaos development. */

#ifndef __STORM_I386_ARGUMENTS_H__
#define __STORM_I386_ARGUMENTS_H__

#include <storm/types.h>

/* Function prototypes. */

#ifdef __cplusplus
extern "C"
{
#endif
  extern u32 arguments_parse (char *source, char *destination, u32 delta);
  extern char arguments_kernel[];
#ifdef __cplusplus
}
#endif

#endif /* !__STORM_I386_ARGUMENTS_H__ */
