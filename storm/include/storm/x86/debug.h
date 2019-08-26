// Abstract: ia32 specific debug functions.
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 1999 chaos development.

#pragma once

#include <storm/generic/debug.h>
#include <storm/generic/cpu.h>
#include <storm/x86/descriptor.h>

extern void debug_dump_descriptor(descriptor_type *desc);
extern void debug_memory_dump(uint32_t *memory, uint32_t length);
extern void decimal_string(char *string, unsigned int number);
