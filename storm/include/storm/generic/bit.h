// Abstract: Bit functions.
// Author: Per Lundberg <per@halleluja.nu>

// © Copyright 1999-2000, 2013 chaos development.

#pragma once

#if (defined __i386__) || (defined __i486__) || (defined __i586__) || (defined __i686__)
// Little-endian system.
#   define BIT_SET(a, b) ((a) |= (1 << (b)))
#   define BIT_CLEAR(a, b) ((a) &= !(1 << (b)))
#   define BIT_GET(a, b) ((a) & (1 << (b)) ? 1 : 0)
#   define BIT_IN_BYTES(a) ((a) % 8 != 0 ? (a) / 8 + 1 : (a) / 8)

// Get the numeric value for bit x.
#   define BIT_VALUE(x) (1 << (x))
#else
#   error A big-endian version of this file has not yet been implemented.
#endif
