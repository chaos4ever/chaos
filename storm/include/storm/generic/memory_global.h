// Abstract: Routines for allocating memory in the global heap.
// Authors: Henrik Hallin <hal@chaosdev.org>
//          Per Lundberg <per@chaosdev.io>

// © Copyright 1999 chaos development

#pragma once

#include <storm/generic/avl.h>
#include <storm/generic/defines.h>
#include <storm/generic/types.h>

extern void memory_global_init(void) INIT_CODE;
extern void *memory_global_allocate(unsigned int length);
extern return_type memory_global_deallocate(void *block);
extern unsigned int memory_global_get_used(void);
extern unsigned int memory_global_get_free(void);
extern uint32_t memory_global_num_allocations(void);
extern uint32_t memory_global_num_deallocations(void);
extern uint32_t memory_global_allocation_cycles(void);

extern avl_header_type *global_avl_header;
