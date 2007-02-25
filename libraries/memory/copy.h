/* $Id$ */
/* Abstract: Functions for copying memory. */
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

#ifndef __LIBRARY_MEMORY_COPY_H__
#define __LIBRARY_MEMORY_COPY_H__

/* Prototypes. */

extern inline void *memory_copy (void *to, const void *from, int n);

/* Inlines. */

/* FIXME: The last function in this file is pure fulkod. */

/* FIXME: Finish this one. */

#ifdef MMX

extern inline void *memcpy (void *to, const void *from, size_type n)
{
  asm 
  ("\
    pushad              /* save registers */
    mov %0, %%edi       /* get 1st argument */
    mov %1. %%esi       /* ...2nd */
    mov %2, %%eax       /* ...3rd */

    mov %%eax, %%edx
    shrb 3, %%eax	/* figure out how many 8 byte chunks we have */
    andb 7, %%edx	/* also figure out slack */
    test %%eax, %%eax	/* Do we have any big chunks? */
    pushl %%edx
    jz .slack		/* If not, let's just do slack */

    mov %%eax, %%ecx

.mmx_move:
    movq (%%esi), %%mm0	/* move 8 byte blocks using MMX */
    movq %%mm0, (%%edi)
    addb 8, %%esi	/* increment pointers */
    addb 8, %%edi
    loopnz .mmx_move	/* continue until CX = 0 */

.slack:
    popl %%ecx
    rep movsb		/* move 1 byte slack */

    emms		/* Free up for the FPU */

    popad		/* clean up */
  "
   : (n) from, (n) to);
  return to;
}

#else

extern inline void *memory_copy (void *to, const void *from, int n)
{
  int d0, d1, d2;
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
   : "=&c" (d0), "=&D" (d1), "=&S" (d2)
   : "0" (n / 4), "q" (n), "1" ((long) to), "2" ((long) from)
   : "memory");
 return (to);
}

#endif

#endif /* !__LIBRARY_MEMORY_COPY_H__ */
