/* $Id$ */
/* Abstract: Routines for managing the Global Descriptor Table. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

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

#include <storm/generic/defines.h>
#include <storm/generic/memory.h>
#include <storm/generic/string.h>
#include <storm/generic/types.h>

#include <storm/ia32/descriptor.h>
#include <storm/ia32/gdt.h>

descriptor_type *gdt = (descriptor_type *) BASE_GDT;

/* Set up a call gate in the GDT. */

void gdt_setup_call_gate (unsigned int number, unsigned int selector,
                          void *address, unsigned int dpl, unsigned int params)
{
  gate_descriptor_type gate_descriptor;

  gate_descriptor.offset_lo = (u32) address & 0xFFFF;
  gate_descriptor.offset_hi = (((u32) address) >> 16) & 0xFFFF;
  gate_descriptor.segment_selector = selector;
  gate_descriptor.params = params;
  gate_descriptor.zero = 0;
  gate_descriptor.dpl = dpl;
  gate_descriptor.present = 1;
  gate_descriptor.type = DESCRIPTOR_TYPE_CALL_GATE;

  memory_copy (&gdt[number], (void *) &gate_descriptor, 8);
}

/* Create a TSS descriptor gate in the GDT. */

void gdt_setup_tss_descriptor (unsigned int selector, void *address, 
                               unsigned int dpl, unsigned int limit)
{
  descriptor_type descriptor;

  descriptor.limit_hi = (limit - 1) >> 16;
  descriptor.limit_lo = (limit - 1) & 0xFFFF;
  descriptor.granularity = 0;
  descriptor.base_lo = (u32) address & 0xFFFF;
  descriptor.base_hi = ((u32) address >> 16) & 0xFF;
  descriptor.base_hi2 = ((u32) address >> 24) & 0xFF;
  descriptor.type = DESCRIPTOR_TYPE_TSS;
  descriptor.descriptor_type = 0;
  descriptor.dpl = dpl;
  descriptor.segment_present = 1;
  descriptor.zero = 0;
  descriptor.operation_size = 0;
  
  memory_copy (&gdt[selector], (void *) &descriptor, 8);
}
