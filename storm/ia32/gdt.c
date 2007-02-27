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
#include <storm/generic/return_values.h>
#include <storm/generic/memory.h>
#include <storm/generic/string.h>
#include <storm/generic/types.h>

#include <storm/ia32/descriptor.h>
#include <storm/ia32/gdt.h>

descriptor_type *gdt = (descriptor_type *) BASE_GDT;

/* Add a GDT entry. */

void gdt_add_entry (u16 number, descriptor_type *descriptor)
{
  memory_copy (&gdt[number], descriptor, 8);
}

/* Set up a call gate in the GDT. */

void gdt_setup_call_gate (u8 number, u16 selector, void *address, u8 dpl, u8 params)
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

void gdt_setup_tss_descriptor (u16 selector, void *address, int dpl, int limit)
{
  descriptor_type tmpdesc;

  tmpdesc.limit_hi = 0;
  tmpdesc.limit_lo = limit;
  tmpdesc.granularity = 0;
  tmpdesc.base_lo = (u32) address & 0xFFFF;
  tmpdesc.base_hi = ((u32) address >> 16) & 0xFF;
  tmpdesc.base_hi2 = ((u32) address >> 24) & 0xFF;
  tmpdesc.type = DESCRIPTOR_TYPE_TSS;
  tmpdesc.descriptor_type = 0;
  tmpdesc.dpl = dpl;
  tmpdesc.segment_present = 1;
  tmpdesc.zero = 0;
  tmpdesc.operation_size = 0;
  
  memory_copy (&gdt[selector], (void *) &tmpdesc, 8);
}
