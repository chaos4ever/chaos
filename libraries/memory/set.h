/* $Id$ */
/* Abstract: Functions for setting memory blocks. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

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

#ifndef __LIBRARY_MEMORY_SET_H__
#define __LIBRARY_MEMORY_SET_H__

#include <system/system.h>

/* Inlines. */

/* Set memory in u8:s. */
/* FIXME: If multiples of words, use stosd instead of stosb. */

static inline void memory_set_u8 (u8 *address, u8 c, unsigned int size)
{
  int ecx, edi;

  asm volatile
  ("\
    cld
    rep
    stosb
   "
   : "=&c" (ecx), "=&D" (edi)
   : "a" (c), "1" (address), "0" (size)
   : "memory");
}

/* Set memory in u16:s. */

static inline void memory_set_u16 (u16 *address, u16 c, unsigned int size)
{
  int ecx, edi;

  asm volatile
  ("\
    cld
    rep
    stosw
   "
   : "=&c" (ecx), "=&D" (edi)
   : "a" (c), "1" (address), "0" (size)
   : "memory");
}

/* Set memory in u32:s. */

static inline void memory_set_u32 (u32 *address, u32 c, unsigned int size)
{
  int ecx, edi;

  asm volatile
  ("\
    cld
    rep
    stosl
   "
   : "=&c" (ecx), "=&D" (edi)
   : "a" (c), "1" (address), "0" (size)
   : "memory");
}

#endif /* !__LIBRARY_MEMORY_SET_H__ */
