// Abstract: ia32 specific debug functions.
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 2000 chaos development.
// © Copyright 2007 chaos development.
// © Copyright 2015-2016 chaos development.

#pragma once

#include <storm/generic/debug.h>
#include <storm/generic/cpu.h>
#include <storm/x86/descriptor.h>

extern void debug_dump_descriptor(descriptor_type *desc);
extern void debug_memory_dump(u32 *memory, u32 length);
