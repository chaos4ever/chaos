// Abstract: Console server functions
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development.

#include <memory/memory.h>

#include "console_output_all.h"

bool console_output_all(console_type *console, void *buffer, unsigned int length)
{
    if (length != console->width * console->height * sizeof(character_type))
    {
        return FALSE;
    }

    memory_copy(console->output, buffer,
                console->width * console->height * sizeof(character_type));

    return TRUE;
}
