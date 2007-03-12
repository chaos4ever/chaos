/* $Id$ */
/* Abstract: Functions for memory operations. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

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
   USA. */

#ifndef __STORM_IA32_MEMORY_H__
#define __STORM_IA32_MEMORY_H__

#include <storm/generic/types.h>

/* Inlines. */
/* FIXME: These are not all nice. */

static inline void *memory_copy (void *to, void *from, int n)
{
  int ecx, edi, esi;

  asm volatile 
  ("\
    cld
    rep ; movsl
    testb $2, %b4
    je 1f
    movsw
 1: testb $1, %b4
    je 2f
    movsb
 2:
   "
   : "=&c" (ecx), "=&D" (edi), "=&S" (esi)
   :"0" (n / 4), "q" (n),"1" ((long) to),"2" ((long) from)
   : "memory");

  return (to);
}

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

static inline void memory_set_u16 (u16 *addr, u16 c, unsigned int size)
{
  int d0, d1;
  asm volatile
  ("\
    cld
    rep
    stosw
"
   : "=&c" (d0), "=&D" (d1)
   : "a" (c), "1" (addr), "0" (size)
   : "memory");
}

#endif /* !__STORM_IA32_MEMORY_H__ */
