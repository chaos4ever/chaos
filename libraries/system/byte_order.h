/* $Id$ */
/* Abstract: Inline functions for swapping byte order. */
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

#ifndef __LIBRARY_SYSTEM_BYTE_ORDER_H__
#define __LIBRARY_SYSTEM_BYTE_ORDER_H__

#include <storm/storm.h>

/* 486 and above have the neat instruction 'bswap' which makes life a
   lot easier. */
/* FIXME: This doesn't work for some reason. Find out why! (It always
   includes this code) */

#if (TARGET_CPU == i486) || (TARGET_CPU == i586) || (TARGET_CPU == i686) || (TARGET_CPU == k6)

static inline const u32 system_byte_swap_u32 (u32 x)
{
  /* Swap the bytes. */

  asm 
  ("\
    bswap  %0
   "
   : "=r" (x)
   : "0" (x));
  
  return x;
}

#else

static inline const u32 system_byte_swap_u32 (u32 x)
{
  asm 
  ("\
    /* Swap lower bytes. */

    xchgb       %b0, %h0

    /* Swap u16s. */

    rorl        $16, %0

    /* Swap higher bytes. */

    xchgb       %b0, %h0
   "
   : "=q" (x)
   : "0" (x));

  return x;
}

#endif /* (!defined __i486__) && (!defined __i586__) && (!defined __i686) */

static inline const u16 system_byte_swap_u16 (u16 x)
{
  asm 
  ("\
    xchgb   %b0, %h0
   "
   : "=q" (x) 
   : "0" (x));
  return x;
}

#endif /* !__LIBRARY_SYSTEM_BYTE_ORDER_H__ */
