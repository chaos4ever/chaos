// Abstract: Thread capabilities.
// Author: Per Lundberg <per@halleluja.nu>

// © Copyright 2000, 2013 chaos development.

#pragma once

typedef struct
{
    // Is this thread allowed to create and delete services?
    u32 modify_services : 1;

    // Is this thread allowed to register and unregister I/O ports, DMA channels and IRQ levels?
    u32 modify_hardware : 1;

    // Is this thread allowed to do thread_control on threads owned by someone else? (for example, to modify priority
    // suspend/resume threads, etc)
    u32 thread_control_others : 1;

    // Is this thread allowed to kill threads owned by someone else?
    u32 kill_other_threads : 1;
} capability_type;
