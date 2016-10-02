// Abstract: Descriptor definitions.
// Author: Per Lundberg <per@chaosdev.io>
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
    uint16_t limit_lo;

    // Low 16 bits of base address.
    uint16_t base_lo;

    // Bits 16-23 of base address.
    uint8_t base_hi;

    // Segment type.
    uint8_t type                  : 4;

    // 0 = system, 1 = code or data.
    uint8_t descriptor_type       : 1;

    // Descriptor privilege level.
    uint8_t dpl                   : 2;

    // Zero if segment isn't used.
    uint8_t segment_present       : 1;

    // High four bits of segment limit.
    uint8_t limit_hi              : 4;

    // Intel is being 'smart' as usual...
    uint8_t unused                : 1;

    // Always zero!
    uint8_t zero                  : 1;

    // 0 = 16-bit, 1 = 32-bit.
    uint8_t operation_size        : 1;

    // If 1, segment limit is multiplied by 4096.
    uint8_t granularity           : 1;

    // Bits 24-31 of base address.
    uint8_t base_hi2;
} PACKED descriptor_type;

typedef struct
{
    // Low 16 bits of offset.
    uint16_t offset_lo;

    uint16_t segment_selector;

    // Zero if not type == 12.
    uint8_t params                : 5;

    // Zero
    uint8_t zero                  : 3;

    uint8_t type                  : 5;

    // Descriptor privilege level.
    uint8_t dpl                   : 2;

    // If zero, this descriptor isn't used.
    uint8_t present               : 1;

    // High 16 bits of offset.
    uint16_t offset_hi;
} PACKED gate_descriptor_type;
