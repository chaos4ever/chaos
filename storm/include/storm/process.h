// Abstract: Process control.
// Author: Per Lundberg <per@chaosdev.io>
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
    uint32_t process_type;
    uint32_t initial_eip;
    process_id_type *process_id;

    bool block;

    uint8_t *code_section_address;
    uint32_t code_section_base;
    uint32_t code_section_size;

    uint8_t *data_section_address;
    uint32_t data_section_base;
    uint32_t data_section_size;

    uint32_t bss_section_base;
    uint32_t bss_section_size;
    char *parameter_string;
} process_create_type;
