// Abstract: Routines for allocating memory in the global heap.
// Authors: Henrik Hallin <hal@chaosdev.org>
//          Per Lundberg <per@chaosdev.io>

// © Copyright 2000, 2013 chaos development.

#pragma once

#include <storm/generic/avl.h>
#include <storm/generic/defines.h>
#include <storm/generic/types.h>

extern void memory_global_init(void) INIT_CODE;
extern void *memory_global_allocate(unsigned int length);
extern return_type memory_global_deallocate(void *block);
extern unsigned int memory_global_get_used(void);
extern unsigned int memory_global_get_free(void);

extern avl_header_type *global_avl_header;
