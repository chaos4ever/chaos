// Abstract: Protocol used for communicating with mouse hardware servers.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013-2017 chaos development

#pragma once

enum
{
    // The mouse position or button state was changed.
    IPC_MOUSE_EVENT = (IPC_PROTOCOL_MOUSE << 16),

    // Register ourselves as receiver of mouse events.
    IPC_MOUSE_REGISTER_TARGET,

    // Unregister ourselves as receiver of mouse events.
    IPC_MOUSE_UNREGISTER_TARGET,
};

typedef struct
{
    unsigned int x;
    unsigned int y;
    unsigned int button;
} ipc_mouse_event_type;
