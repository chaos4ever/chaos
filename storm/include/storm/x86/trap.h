// Abstract: Exception stuff.
//
// © Copyright 1999 chaos development.

#pragma once

#include <storm/x86/tss.h>
#include <storm/x86/types.h>

// Prototypes.
extern void trap_init(void) INIT_CODE;

// Inlines.
// FIXME: make these support popping errorcode from stack.
static inline void trap_enter(void)
{
    asm
    (
        "pushl %ds\n"
        "movw $1 << 3, %ax\n"
        "movw %ax, %ds\n"
        "cli"
    );
}

static inline void trap_leave(void)
{
    asm
    (
        "popl %ds\n"
        "iret"
    );
}

// External variables.
extern tss_type *trap_tss;
