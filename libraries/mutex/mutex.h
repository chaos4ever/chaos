// Abstract: Mutex library function prototypes.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2000, 2013 chaos development.

#pragma once

enum
{
    MUTEX_UNLOCKED,
    MUTEX_LOCKED
};

typedef volatile unsigned int mutex_type;

// Lock a mutex.
static inline void mutex_wait(mutex_type mutex UNUSED)
{
    // FIXME: Fix this code. For now, user-level mutexing is broken. We should perhaps 

#if FALSE
    unsigned int eax;

    asm volatile("movl $0, %%eax\n"
                 "0:  xchgl %%eax, %0\n"
                 "cmpl $0, %%eax\n"
                 "je 0b"
                 : "=g" (mutex),
                   "=a" (eax)
                 : "bcdDS" (mutex));    // BCD is faster than integers -- Martim.

#endif
}

// Unlock a mutex.
static inline void mutex_signal(mutex_type *mutex)
{
    *mutex = MUTEX_UNLOCKED;
}
