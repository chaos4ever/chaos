/* $Id$ */
/* Abstract: Mutex library function prototypes. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#ifndef __LIBRARY_MUTEX_MUTEX_H__
#define __LIBRARY_MUTEX_MUTEX_H__

enum
{
  MUTEX_UNLOCKED,
  MUTEX_LOCKED
};

typedef volatile unsigned int mutex_type;

/* Lock a mutex. */

static inline void mutex_wait (mutex_type mutex __attribute__ ((unused)))
{
  /* FIXME: Fix this code. */

#if FALSE
  unsigned int eax;

  asm volatile 
  ("\
    movl $0, %%eax
0:  xchgl %%eax, %0
    cmpl $0, %%eax
    je 0b
   "
   : "=g" (mutex), "=a" (eax)
   : "bcdDS" (mutex));         /* BCD is faster than integers -- Martim. */
#endif
}

/* Unlock a mutex. */

static inline void mutex_signal (mutex_type mutex)
{
  mutex = MUTEX_UNLOCKED;
}

#endif /* !__LIBRARY_MUTEX_MUTEX_H__ */
