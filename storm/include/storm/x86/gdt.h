// Abstract: Function prototypes for GDT routines.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1998-2000, 2013 chaos development.

#pragma once

#include <storm/x86/defines.h>
#include <storm/x86/types.h>
#include <storm/x86/descriptor.h>

#define GDT(n, p)                       (((n) << 3) + p)

#define SELECTOR_KERNEL_CODE            GDT(1, 0)
#define SELECTOR_KERNEL_DATA            GDT(2, 0)
#define SELECTOR_PROCESS_CODE           GDT(3, 3)
#define SELECTOR_DATA                   GDT(4, 3)
#define SELECTOR_TSS1                   GDT(5, 3)
#define SELECTOR_TSS2                   GDT(6, 3)

// The location of the exceptions in the GDT.
#define GDT_BASE_EXCEPTIONS             (16)

extern descriptor_type *gdt;

C_EXTERN_BEGIN

extern void gdt_add_entry(uint16_t number, descriptor_type *descriptor) INIT_CODE;
extern void gdt_setup_call_gate(uint8_t number, uint16_t selector, function_type handler, uint8_t dpl, uint8_t params) INIT_CODE;
extern void gdt_setup_tss_descriptor(uint16_t selector, void *address, int dpl, int limit);

C_EXTERN_END
