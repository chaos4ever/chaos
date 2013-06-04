// Abstract: Definitions of the bits in the flag field.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2000, 2013 chaos development.

#pragma once

#include <storm/generic/bit.h>

// Flags in the EFLAGS register. See the Intel documentation for more information about what those does.
enum
{
    FLAG_CARRY = (BIT_VALUE(0)),
    FLAG_SET = (BIT_VALUE(1)),
    FLAG_PARITY = (BIT_VALUE(2)),
    FLAG_ADJUST = (BIT_VALUE(4)),
    FLAG_ZERO = (BIT_VALUE(6)),
    FLAG_SIGN = (BIT_VALUE(7)),
    FLAG_TRAP = (BIT_VALUE(8)),
    FLAG_INTERRUPT_ENABLE = (BIT_VALUE(9)),
    FLAG_DIRECTION = (BIT_VALUE(10)),
    FLAG_OVERFLOW = (BIT_VALUE(11)),
    FLAG_IOPL_LOW = (BIT_VALUE(12)),
    FLAG_IOPL_HIGH = (BIT_VALUE(13)),
    FLAG_NESTED_TASK = (BIT_VALUE(14)),
    FLAG_RESUME_TASK = (BIT_VALUE(16)),
    FLAG_V8086_MODE = (BIT_VALUE(17)),
    FLAG_ALIGNMENT_CHECK = (BIT_VALUE(18)),
    FLAG_VIRTUAL_INTERRUPT = (BIT_VALUE(19)),
    FLAG_VIRTUAL_INTERRUPT_PENDING = (BIT_VALUE(20)),
    FLAG_ID = (BIT_VALUE(21))
};
