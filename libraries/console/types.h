// Abstract: Console library types.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development.

#pragma once

#include <ipc/ipc.h>

typedef struct
{
    uint8_t character;
    uint8_t attribute;
} PACKED console_character_type;

typedef struct
{
    ipc_structure_type ipc_structure;
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    unsigned int type;
    bool initialised;
    bool opened;

    // If ipc_console_attribute_type.enable_buffer was set when
    // console_open was called, this will be a non-NULL pointer that
    // will be used to double-buffer content.
    console_character_type *buffer;
} console_structure_type;
