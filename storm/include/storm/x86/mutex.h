// Abstract: IA32-specific mutex code.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#pragma once

#include <storm/generic/mutex.h>

static inline void mutex_spin_lock(spinlock_type *spinlock)
{
    //  DEBUG_MESSAGE ("Called");

    // Based on a suggestion from SO: http://stackoverflow.com/a/1383501/227779
    // We used to have this in inline asm, but since GCC has a builtin for this we might as well not reinvent the wheel.
    while (__sync_lock_test_and_set(spinlock, MUTEX_SPIN_LOCKED))
        while (*spinlock);
}

static inline void mutex_spin_unlock(spinlock_type *spinlock)
{
    //  DEBUG_MESSAGE ("Called");
    *spinlock = MUTEX_SPIN_UNLOCKED;
}
