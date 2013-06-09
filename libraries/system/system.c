// Abstract: Library for system related stuff.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2000, 2007, 2013 chaos development.

#include <system/system.h>

void system_shutdown(void)
{
    // TODO: Make sure we are root. If not, we will only end up killing our own processes, which is probably not what we want...
    // TODO: Kill all processes.
    while (TRUE);
}

// Sleep for a given amount of time. (In milliseconds)
return_type system_sleep(unsigned int time)
{
#if FALSE
    time_type start_time, current_time;

    system_call_timer_read(&start_time);
    current_time = start_time;
    while (current_time < start_time + time + 1)
    {
        system_call_timer_read(&current_time);
        system_call_dispatch_next();
    }
#endif

    kernelfs_self_type kernelfs_self;
    kernelfs_self.kernelfs_class = KERNELFS_CLASS_SELF_INFO;
    system_call_kernelfs_entry_read(&kernelfs_self);

    system_call_thread_control(kernelfs_self.thread_id, THREAD_SLEEP, time);

    return SYSTEM_RETURN_SUCCESS;
}

return_type system_sleep_microseconds(unsigned int time)
{
    kernelfs_self_type kernelfs_self;
    kernelfs_self.kernelfs_class = KERNELFS_CLASS_SELF_INFO;
    system_call_kernelfs_entry_read(&kernelfs_self);

    system_call_thread_control(kernelfs_self.thread_id, THREAD_SLEEP, (time / 1000) + 1);

    return SYSTEM_RETURN_SUCCESS;
}

return_type system_process_name_set(char *name)
{
    system_call_process_name_set(name);

    return SYSTEM_RETURN_SUCCESS;
}

return_type system_cluster_name_set(char *name UNUSED)
{
    // FIXME: Do something here. I guess we need a conforming system call first...

    return SYSTEM_RETURN_SUCCESS;
}

return_type system_thread_name_set(char *name)
{
    system_call_thread_name_set(name);

    return SYSTEM_RETURN_SUCCESS;
}

return_type system_thread_create(void)
{
    switch (system_call_thread_create())
    {
        case STORM_RETURN_THREAD_NEW:
        {
            return SYSTEM_RETURN_THREAD_NEW;
        }

        case STORM_RETURN_THREAD_OLD:
        {
            return SYSTEM_RETURN_THREAD_OLD;
        }

        // Someone has added a return code in thread.c in the kernel without handling it in the system library. Please fix this in
        // the system library and send us a patch/pull request.
        default:
        {
            return SYSTEM_RETURN_THREAD_CREATE_FAILED;
        }
    }
}

// Execute the program specified in the foreground (that is, this function doesn't return until the child process has been
// terminated)
// FIXME: Maybe have a flag that says whether we should wait or not?
// FIXME: Should we open a connection to the VFS on startup, or...?
return_type system_execute(char *program UNUSED)
{
    return SYSTEM_RETURN_SUCCESS;
}

// Exit the current thread. If this is the last thread of the process, the process will be terminated too.
void system_exit(void)
{
    kernelfs_self_type kernelfs_self;

    kernelfs_self.kernelfs_class = KERNELFS_CLASS_SELF_INFO;
    system_call_kernelfs_entry_read(&kernelfs_self);
    system_call_thread_control(kernelfs_self.thread_id, THREAD_TERMINATE, 0);
    while (TRUE);
}
