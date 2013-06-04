// Abstract: Process control.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 1999-2000, 2013 chaos development.

#pragma once

#include <storm/types.h>

// Process types.
enum
{
    PROCESS_TYPE_REGULAR,
    PROCESS_TYPE_SERVER
};

// Structure used by process_create.
typedef struct
{
    u32 process_type;
    u32 initial_eip;
    process_id_type *process_id;

    bool block;

    u8 *code_section_address;
    u32 code_section_base;
    u32 code_section_size;

    u8 *data_section_address;
    u32 data_section_base;
    u32 data_section_size;

    u32 bss_section_base;
    u32 bss_section_size;
    char *parameter_string;
} process_create_type;
