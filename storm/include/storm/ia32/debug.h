/* $Id$ */
/* Abstract: ia32 specific debug functions. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */
/* Copyright 2007 chaos development. */

#ifndef __STORM_IA32_DEBUG_H__
#define __STORM_IA32_DEBUG_H__

#include <storm/generic/debug.h>
#include <storm/generic/cpu.h>
#include <storm/ia32/descriptor.h>

/* Function prototypes. */

extern void debug_dump_descriptor (descriptor_type *desc);
extern void debug_memory_dump (u32 *memory, u32 length);

#endif /* !__STORM_IA32_DEBUG_H__ */
