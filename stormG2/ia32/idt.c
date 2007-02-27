/* $Id$ */
/* Abstract: IDT management. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

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

#include <storm/ia32/defines.h>
#include <storm/ia32/descriptor.h>
#include <storm/ia32/gdt.h>
#include <storm/ia32/idt.h>
#include <storm/ia32/memory.h>
#include <storm/ia32/string.h>
#include <storm/ia32/types.h>

descriptor_type *idt = (descriptor_type *) BASE_IDT;
u32 no_idt[2] = { 0, 0 };

/* Create an interrupt gate. */

void idt_setup_interrupt_gate (unsigned int number, unsigned int selector, 
                               void *address, unsigned int privilege_level)
{
  gate_descriptor_type gate_descriptor;

  gate_descriptor.offset_lo = (u32) address & 0xFFFF;
  gate_descriptor.offset_hi = ((u32) address >> 16) & 0xFFFF;
  gate_descriptor.segment_selector = selector;
  gate_descriptor.params = 0;
  gate_descriptor.zero = 0;
  gate_descriptor.dpl = privilege_level;
  gate_descriptor.present = 1;
  gate_descriptor.segment_selector = selector;
  gate_descriptor.params = 0;
  gate_descriptor.zero = 0;
  gate_descriptor.dpl = privilege_level;
  gate_descriptor.present = 1;
  gate_descriptor.type = DESCRIPTOR_TYPE_INTERRUPT_GATE_32;

  memory_copy (&idt[number], (void *) &gate_descriptor, 8);
}

/* Create a task gate. */

void idt_setup_task_gate (unsigned int number, unsigned int selector, 
                          unsigned int privilege_level)
{
  gate_descriptor_type gate_descriptor;

  gate_descriptor.segment_selector = selector;
  gate_descriptor.zero = 0;
  gate_descriptor.params = 0;
  gate_descriptor.dpl = privilege_level;
  gate_descriptor.present = 1;
  gate_descriptor.type = DESCRIPTOR_TYPE_TASK_GATE_32;

  memory_copy (&idt[number], (void *) &gate_descriptor, 8);
}
