// Abstract: IA32-specific thread stuff.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development

#pragma once

#include <storm/x86/flags.h>

// Default flags for new processes are IF, PF, AF. See the Intel documentation for more information about this.
#define THREAD_NEW_EFLAGS (FLAG_INTERRUPT_ENABLE | FLAG_ADJUST | FLAG_PARITY | FLAG_SET)
