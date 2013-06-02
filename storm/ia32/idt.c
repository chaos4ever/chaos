// Abstract: IDT management.
// Authors: Per Lundberg <per@halleluja.nu>
//          Henrik Hallin <hal@chaosdev.org>

// Copyright 1999-2000 chaos development.

#include <storm/ia32/defines.h>
#include <storm/ia32/descriptor.h>
#include <storm/ia32/gdt.h>
#include <storm/ia32/idt.h>
#include <storm/ia32/memory.h>
#include <storm/ia32/string.h>
#include <storm/ia32/types.h>

descriptor_type *idt = (descriptor_type *) BASE_IDT;
long no_idt[2] = { 0, 0 };

/* Create an interrupt gate. */

void idt_setup_interrupt_gate (u8 number, u16 selector, void *address, u8 pl)
{
  gate_descriptor_type gate_descriptor;

  gate_descriptor.offset_lo = (u32) address & 0xFFFF;
  gate_descriptor.offset_hi = ((u32) address >> 16) & 0xFFFF;
  gate_descriptor.segment_selector = selector;
  gate_descriptor.params = 0;
  gate_descriptor.zero = 0;
  gate_descriptor.dpl = pl;
  gate_descriptor.present = 1;
  gate_descriptor.segment_selector = selector;
  gate_descriptor.params = 0;
  gate_descriptor.zero = 0;
  gate_descriptor.dpl = pl;
  gate_descriptor.present = 1;
  gate_descriptor.type = DESCRIPTOR_TYPE_INTERRUPT_GATE_32;

  memory_copy (&idt[number], (void *) &gate_descriptor, 8);
}

/* Create a task gate. */

void idt_setup_task_gate (u8 number, u16 selector, u8 pl)
{
  gate_descriptor_type gate_descriptor;

  gate_descriptor.segment_selector = selector;
  gate_descriptor.zero = 0;
  gate_descriptor.params = 0;
  gate_descriptor.dpl = pl;
  gate_descriptor.present = 1;
  gate_descriptor.type = DESCRIPTOR_TYPE_TASK_GATE_32;

  memory_copy (&idt[number], (void *) &gate_descriptor, 8);
}
