// Abstract: Functions for setting up the system calls.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015-2016 chaos development

#include <storm/x86/defines.h>
#include <storm/x86/gdt.h>
#include <storm/x86/system_calls.h>
#include <storm/x86/types.h>

void system_calls_init (void)
{
    for (int counter = 0; counter < SYSTEM_CALLS; counter++)
    {
        gdt_setup_call_gate(system_call[counter].number, SELECTOR_KERNEL_CODE, system_call[counter].handler, 3,
                            system_call[counter].arguments);
    }
}
