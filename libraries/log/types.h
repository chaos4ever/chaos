// Abstract: Type definitions used by the log library.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development.

#pragma once

#include <ipc/ipc.h>

typedef struct
{
    ipc_structure_type ipc_structure;
    char *log_class;
} log_structure_type;
