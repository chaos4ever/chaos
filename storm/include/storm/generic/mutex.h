/* $Id$ */
/* Abstract: Function prototypes for the mutually exclusive code. */
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

#ifndef __STORM_GENERIC_MUTEX_H__
#define __STORM_GENERIC_MUTEX_H__

#include <storm/generic/types.h>

/* Type definitions. */

typedef volatile int mutex_kernel_type;

/* Defines. */

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

/* Prototypes. */

extern return_type mutex_kernel_wait (mutex_kernel_type *mutex);
extern return_type mutex_kernel_signal (mutex_kernel_type *mutex);

/* This spinlock locks the mutex system. */

extern spinlock_type mutex_spinlock;

/* Also get the architecture specifics. */

#include <storm/current-arch/mutex.h>

#endif /* !__STORM_GENERIC_MUTEX_H__ */
