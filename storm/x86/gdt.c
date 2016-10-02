// Abstract: Routines for managing the Global Descriptor Table.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1998-2000, 2013 chaos development.

#include <storm/generic/defines.h>
#include <storm/generic/return_values.h>
#include <storm/generic/memory.h>
#include <storm/generic/string.h>
#include <storm/generic/types.h>

#include <storm/x86/descriptor.h>
#include <storm/x86/gdt.h>

descriptor_type *gdt = (descriptor_type *) BASE_GDT;

void gdt_add_entry(uint16_t number, descriptor_type *descriptor)
{
    memory_copy(&gdt[number], descriptor, 8);
}

void gdt_setup_call_gate(uint8_t number, uint16_t selector, function_type address, uint8_t dpl, uint8_t params)
{
    gate_descriptor_type gate_descriptor;

    gate_descriptor.offset_lo = (uint32_t) address & 0xFFFF;
    gate_descriptor.offset_hi = (((uint32_t) address) >> 16) & 0xFFFF;
    gate_descriptor.segment_selector = selector;
    gate_descriptor.params = params;
    gate_descriptor.zero = 0;
    gate_descriptor.dpl = dpl;
    gate_descriptor.present = 1;
    gate_descriptor.type = DESCRIPTOR_TYPE_CALL_GATE;

    memory_copy(&gdt[number], (void *) &gate_descriptor, 8);
}

void gdt_setup_tss_descriptor(uint16_t selector, void *address, int dpl, int limit)
{
    descriptor_type tmpdesc;

    tmpdesc.limit_hi = 0;
    tmpdesc.limit_lo = limit;
    tmpdesc.granularity = 0;
    tmpdesc.base_lo = (uint32_t) address & 0xFFFF;
    tmpdesc.base_hi = ((uint32_t) address >> 16) & 0xFF;
    tmpdesc.base_hi2 = ((uint32_t) address >> 24) & 0xFF;
    tmpdesc.type = DESCRIPTOR_TYPE_TSS;
    tmpdesc.descriptor_type = 0;
    tmpdesc.dpl = dpl;
    tmpdesc.segment_present = 1;
    tmpdesc.zero = 0;
    tmpdesc.operation_size = 0;

    memory_copy(&gdt[selector], (void *) &tmpdesc, 8);
}
