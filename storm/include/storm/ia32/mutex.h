/* Abstract: IA32-specific mutex code. */
/* Author: Per Lundberg <per@halleluja.nu> */

/* Copyright 1999-2000, 2013 chaos development */

#ifndef __STORM_IA32_MUTEX_H__
#define __STORM_IA32_MUTEX_H__

#include <storm/generic/mutex.h>

/* Inlines. */

/* Lock the given spinlock. This function loops until it may access the
   semaphores, and then locks them. */

static inline void mutex_spin_lock (spinlock_type spinlock)
{
  unsigned int eax;

  //  DEBUG_MESSAGE ("Called");
  
  asm volatile 
  ("\
    movl $0, %%eax \n\
0:  xchgl %%eax, %0 \n\
    cmpl $0, %%eax \n\
    je 0b"
   : "=g" (spinlock), "=a" (eax)
   : "bcdDS" (spinlock));         /* BCD is faster than integers -- Martim. */
}

/* Unlock a mutex which has previously been locked with
   mutex_spin_lock. */

static inline void mutex_spin_unlock (spinlock_type spinlock)
{
  //  DEBUG_MESSAGE ("Called");
  
  spinlock = MUTEX_SPIN_UNLOCKED;
}

#endif /* !__STORM_IA32_MUTEX_H__ */
