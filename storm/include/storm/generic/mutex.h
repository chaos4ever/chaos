// Abstract: Function prototypes for the mutually exclusive code.
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 1999-2000, 2013 chaos development

#pragma once

#include <storm/generic/types.h>

typedef volatile int mutex_kernel_type;

enum
{
    MUTEX_SPIN_LOCKED,
    MUTEX_SPIN_UNLOCKED
};

enum
{
    MUTEX_LOCKED,
    MUTEX_UNLOCKED
};

extern return_type mutex_kernel_wait(mutex_kernel_type *mutex);
extern return_type mutex_kernel_signal(mutex_kernel_type *mutex);

// This spinlock locks the mutex system.
extern spinlock_type mutex_spinlock;

#include <storm/current-arch/mutex.h>
