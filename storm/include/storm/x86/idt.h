// Abstract: Function prototypes for IDT routines.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1999, 2013 chaos development.

#pragma once

#include <storm/generic/defines.h>
#include <storm/generic/types.h>
#include <storm/x86/descriptor.h>

void idt_setup_interrupt_gate(u8 number, u16 selector, void *address, u8 pl) INIT_CODE;
void idt_setup_task_gate(u8 number, u16 selector, u8 pl) INIT_CODE;

extern descriptor_type *idt;
