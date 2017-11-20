// Abstract: Bit functions.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development.

#pragma once

#define SYSTEM_BIT_SET(a,b) ((a) |= (1 << (b)))
#define SYSTEM_BIT_CLEAR(a,b) ((a) &= !(1 << (b)))
#define SYSTEM_BIT_GET(a,b) ((a) & (1 << (b)) ? 1 : 0)
#define SYSTEM_BIT_IN_BYTES(a) ((a) % 8 != 0 ? (a) / 8 + 1 : (a) / 8)

// Get the numeric value for bit x.
#define SYSTEM_BIT_VALUE(x) (1 << (x))
