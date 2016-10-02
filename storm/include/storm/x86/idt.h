// Abstract: Function prototypes for IDT routines.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1999, 2013 chaos development.

#pragma once

#include <storm/generic/defines.h>
#include <storm/generic/types.h>
#include <storm/x86/descriptor.h>

void idt_setup_interrupt_gate(uint8_t number, uint16_t selector, void *address, uint8_t pl) INIT_CODE;
void idt_setup_task_gate(uint8_t number, uint16_t selector, uint8_t pl) INIT_CODE;

extern descriptor_type *idt;
