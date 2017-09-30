// Abstract: IPC library types.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013-2017 chaos development

#pragma once

#include <system/system.h>

// Type definitions.
typedef struct
{
    mailbox_id_type input_mailbox_id;
    mailbox_id_type output_mailbox_id;

    // FIXME: Fields for vendor stuff and service ID.
} ipc_structure_type;
