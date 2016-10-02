// Abstract: Thread capabilities.
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 2000, 2013 chaos development.

#pragma once

typedef struct
{
    // Is this thread allowed to create and delete services?
    uint32_t modify_services : 1;

    // Is this thread allowed to register and unregister I/O ports, DMA channels and IRQ levels?
    uint32_t modify_hardware : 1;

    // Is this thread allowed to do thread_control on threads owned by someone else? (for example, to modify priority
    // suspend/resume threads, etc)
    uint32_t thread_control_others : 1;

    // Is this thread allowed to kill threads owned by someone else?
    uint32_t kill_other_threads : 1;
} capability_type;
