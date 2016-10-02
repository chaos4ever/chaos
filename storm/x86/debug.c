// Abstract: Various functions and macros.
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 1998-2000, 2013 chaos development.

#include <storm/generic/debug.h>
#include <storm/x86/debug.h>
#include <storm/x86/defines.h>
#include <storm/x86/descriptor.h>
#include <storm/x86/types.h>

#if !OPTION_RELEASE

void debug_dump_descriptor (descriptor_type *desc)
{
  uint32_t limit, base;

  limit = (desc->limit_lo + (desc->limit_hi << 16)) * (desc->granularity ? 4096 : 1) + (desc->granularity ^ 1);
  base = desc->base_lo + (desc->base_hi << 16) + (desc->base_hi2 << 24);

  debug_print ("limit: 0x%08lX\n", limit);
  debug_print ("base: 0x%08lX\n", base);
  debug_print ("type: %d\n", desc->type);
  debug_print ("descriptor type: %d\n", desc->descriptor_type);
  debug_print ("dpl: %d\n", desc->dpl);
  debug_print ("segment present: %d\n", desc->segment_present);
  debug_print ("operation size: %d\n", desc->operation_size);
  debug_print ("granularity: %d\n", desc->granularity);
}
#endif
