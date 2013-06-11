// Abstract: Memory library functions.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2000, 2013 chaos development.

#pragma once

#include <system/system.h>

extern void memory_init(void);
extern return_type memory_allocate(void **pointer, unsigned int bytes);
extern return_type memory_deallocate(void **pointer);
