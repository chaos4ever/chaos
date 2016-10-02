// Abstract: Function prototypes for command line argument parsing routines.

// © Copyright 1999-2000, 2013 chaos development.

#pragma once

#include <storm/defines.h>
#include <storm/types.h>

C_EXTERN_BEGIN

extern uint32_t arguments_parse (char *source, char *destination, uint32_t delta);
extern char arguments_kernel[];

C_EXTERN_END
