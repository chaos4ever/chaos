/* $Id$ */
/* Abstract: Various functions and macros. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1998-2000 chaos development. */

/* This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA */

#include <storm/generic/debug.h>
#include <storm/ia32/debug.h>
#include <storm/ia32/defines.h>
#include <storm/ia32/descriptor.h>
#include <storm/ia32/types.h>

#if !OPTION_RELEASE
/* Dump a descriptor numerically. */

void debug_dump_descriptor (descriptor_type *desc)
{
  u32 limit, base;

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
