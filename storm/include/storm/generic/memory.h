// Abstract: Functions for memory operations.
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 1999 chaos development.

#pragma once

#include <storm/current-arch/memory.h>
#include <storm/generic/mutex.h>

extern mutex_kernel_type memory_mutex;

extern return_type memory_allocate(void **address, uint32_t pages, bool cacheable);
extern return_type memory_deallocate(void **address);
extern uint32_t memory_num_allocations(void);
extern uint32_t memory_num_deallocations(void);
extern uint32_t memory_allocation_cycles(void);

extern return_type memory_get_physical_address(void *virtual_address, void **physical_address);
