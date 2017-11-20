// Abstract: Endian conversion.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development.

#pragma once

#if defined __i386__
#include <system/little_endian.h>
#elif defined __sparc__
#include <system/big_endian.h>
#else
#error "I don't know whether your system is little or big endian. Please instruct me."
#endif
