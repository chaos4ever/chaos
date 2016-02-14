// Abstract: Console library types.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000, 2013 chaos development.

#pragma once

#include <ipc/ipc.h>

typedef struct
{
    ipc_structure_type ipc_structure;
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    unsigned int type;
    bool initialised;
    bool opened;
} console_structure_type;
