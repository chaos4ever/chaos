// Abstract: IDT management.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>

// Copyright 1999-2000 chaos development.

#include <storm/x86/defines.h>
#include <storm/x86/descriptor.h>
#include <storm/x86/gdt.h>
#include <storm/x86/idt.h>
#include <storm/x86/memory.h>
#include <storm/x86/string.h>
#include <storm/x86/types.h>

descriptor_type *idt = (descriptor_type *) BASE_IDT;
long no_idt[2] = { 0, 0 };

/* Create an interrupt gate. */

void idt_setup_interrupt_gate (uint8_t number, uint16_t selector, void *address, uint8_t pl)
{
  gate_descriptor_type gate_descriptor;

  gate_descriptor.offset_lo = (uint32_t) address & 0xFFFF;
  gate_descriptor.offset_hi = ((uint32_t) address >> 16) & 0xFFFF;
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

void idt_setup_task_gate (uint8_t number, uint16_t selector, uint8_t pl)
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
