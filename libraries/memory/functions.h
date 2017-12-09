// Abstract: Memory library functions.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development.

#pragma once

#include <system/system.h>

C_EXTERN_BEGIN

extern void memory_init(void);
extern return_type memory_allocate(void **pointer, unsigned int bytes);
extern return_type memory_deallocate(void **pointer);

C_EXTERN_END
