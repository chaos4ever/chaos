// Abstract: Inline functions for swapping byte order.
// Author: Per Lundberg <per@halleluja.nu>
//
// Copyright 1999-2000, 2007, 2013 chaos development.

#pragma once

#include <storm/storm.h>

// 486 and above have the neat instruction 'bswap' which makes life a lot easier. Since 99,9% of all Intel machines that you
// reasonably want to run chaos on is 486 or greater, there's little point in supporting 386 at this point.
static inline u32 system_byte_swap_u32(u32 x)
{

    // Swap the bytes.
    asm
    ("bswap  %0"
     : "=r" (x)
     : "0" (x));

    return x;
}

static inline u16 system_byte_swap_u16(u16 x)
{
    asm
    ("xchgb   %b0, %h0"
     : "=q" (x)
     : "0" (x));

    return x;
}
