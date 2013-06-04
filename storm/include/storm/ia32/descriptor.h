// Abstract: Descriptor definitions.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 1999-2000, 2013 chaos development.

#pragma once

#include <storm/types.h>
#include <storm/defines.h>

enum
{
    DESCRIPTOR_TYPE_TASK_GATE_32 = 5,
    DESCRIPTOR_TYPE_TSS = 9,
    DESCRIPTOR_TYPE_CALL_GATE = 12,
    DESCRIPTOR_TYPE_INTERRUPT_GATE_32 = 14,
    DESCRIPTOR_TYPE_TRAP_GATE_32 = 15,
};

typedef struct
{
    // Low 16 bits of segment limit.
    u16 limit_lo;

    // Low 16 bits of base address.
    u16 base_lo;

    // Bits 16-23 of base address.
    u8 base_hi;

    // Segment type.
    u8 type: 4;

    // 0 = system, 1 = code or data.
    u8 descriptor_type: 1;

    // Descriptor privilege level.
    u8 dpl: 2;

    // Zero if segment isn't used.
    u8 segment_present: 1;

    // High four bits of segment limit.
    u8 limit_hi: 4;

    // Intel is being 'smart' as usual...
    u8 unused: 1;

    // Always zero!
    u8 zero: 1;

    // 0 = 16-bit, 1 = 32-bit.
    u8 operation_size: 1;

    // If 1, segment limit is multiplied by 4096.
    u8 granularity: 1;

    // Bits 24-31 of base address.
    u8 base_hi2;
} PACKED descriptor_type;

typedef struct
{
    // Low 16 bits of offset.
    u16 offset_lo;

    u16 segment_selector;

    // Zero if not type == 12.
    u8 params: 5;

    // Zero
    u8 zero: 3;

    u8 type: 5;

    // Descriptor privilege level.
    u8 dpl: 2;

    // If zero, this descriptor isn't used.
    u8 present: 1;

    // High 16 bits of offset.
    u16 offset_hi;
} PACKED gate_descriptor_type;
