// Abstract: Platform-independent defines local to storm.
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 1999 chaos development.

#pragma once

#include <storm/generic/types.h>

#define STORM

// Gets the largest one of two values
#define MAX_OF_TWO(a, b)                 ((a) > (b) ? (a) : (b))

// Gets the largest one of three values
#define MAX_OF_THREE(a, b, c)             (MAX_OF_TWO((a), \
                                                      MAX_OF_TWO((b), (c))))


// This is a symbol inserted by the linker which tells us where the end of the ELF file is located when it's loaded into memory. We
// use it to figure out how much space the kernel uses, when mapping memory for newly created processes.
extern void *_end;

// The start and end of the initialisation code segment.
extern void *_init_start;
extern void *_init_end;

// This tells whether storm is finished booting or not.
extern bool initialised;

// These probably need to come at the end, since they rely on things declared earlier in this file.
// TODO: Check if this is really the case.
#include <storm/defines.h>
#include <storm/current-arch/defines.h>
