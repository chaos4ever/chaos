/* $Id$ */
/* Abstract: IA32-specific mutex code. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development */

/* This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA */

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
