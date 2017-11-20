// Abstract: Functions for copying memory.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development.

#pragma once

// TODO: Add architecture-optimized versions of this, probably as header files in storm.
static inline void *memory_copy(void * __restrict__ to, const void * __restrict__ from, int n)
{
    char *dst = (char *) to;
    const char *src = (const char *) from;

    while (n-- != 0)
    {
        *dst++ = *src++;
    }

    return to;
}
