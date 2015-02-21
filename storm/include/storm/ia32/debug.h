// Abstract: ia32 specific debug functions.
// Author: Per Lundberg <per@halleluja.nu>

// © Copyright 2000 chaos development.
// © Copyright 2007 chaos development.
// © Copyright 2015 chaos development.

#pragma once

#include <storm/generic/debug.h>
#include <storm/generic/cpu.h>
#include <storm/ia32/descriptor.h>

extern void debug_dump_descriptor(descriptor_type *desc);
extern void debug_memory_dump(u32 *memory, u32 length);
