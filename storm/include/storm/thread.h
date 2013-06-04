// Abstract: Thread related stuff.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2000, 2013 chaos development.

#pragma once

// Those are used by system_call_thread_control.
enum
{
    // Tell the kernel to kill the process unconditionally.
    THREAD_TERMINATE = 0,

    // Suspend a process until resumed by THREAD_RESUME.
    THREAD_SUSPEND,

    // Resumes a process suspended with THREAD_SUSPEND.
    THREAD_RESUME,

    // Puts a process to sleep for a user specified amount of time (in milliseconds).
    THREAD_SLEEP,

    // Set the priority of a process. If U32_MAX is given, it will be treated as an "idle" task (i.e. only dispatched when no other
    // tasks are running). FIXME: NOT U32_MAX!!!!!!!!!!!!!!!!!!!!!!
    THREAD_PRIORITY_SET,

    // The rest are up to the process to handle or not.
    //
    // This is to be forward-compatible.
    THREAD_SHUTDOWN = 4096,

    // Tells a process to reload its configuration file(s).
    THREAD_RELOAD_CONFIGURATION,

    // Tells a process to restart and possibly re-probe for hardware. This makes hotswapping easy.
    THREAD_RESTART,

    // Tells a process to update its (virtual) screen, possibly because of resize.
    THREAD_UPDATE_SCREEN,

    // Some kind of fault occured during execution. The handler should shutdown the program immediately.
    THREAD_EXECUTION_FAULT
};
