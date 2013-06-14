// Abstract: Startup code file for chaos binaries. This code is what executes the main () function.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 1999-2000, 2013 chaos development.

#include <memory/memory.h>
#include <system/system.h>

// FIXME: Write a mutex library.
enum
{
    MUTEX_UNLOCKED,
    MUTEX_LOCKED
};

// This is a little bit ugly, but it would be even uglier to have each process declare this separately... or whatever.
memory_structure_type memory_structure;
int memory_mutex = MUTEX_UNLOCKED;

extern int main(int argc, char **argv);
void startup(void) NORETURN;

void startup(void)
{
    kernelfs_self_type kernelfs_self;

    system_call_init();

    // FIXME: Reimplement the command line passing. It was made in an extremely ugly way, so we removed it for now.
    main(0, (char **) NULL);

    kernelfs_self.kernelfs_class = KERNELFS_CLASS_SELF_INFO;
    system_call_kernelfs_entry_read(&kernelfs_self);
    system_call_thread_control(kernelfs_self.thread_id, THREAD_TERMINATE, 0);
    while (TRUE);
}
