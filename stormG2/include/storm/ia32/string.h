/* $Id$ */
/* Abstract: Inline string routines. */
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

#ifndef __STORM_IA32_STRING_H__
#define __STORM_IA32_STRING_H__

#include <storm/generic/defines.h>
#include <storm/generic/types.h>

/* FIXME: Clean up those functions. */

/* Copy a string. */

static inline char *string_copy (char *destination, const char *source)
{
  int d0, d1, d2;
  asm volatile 
  ("\
    cld
1:  lodsb
    stosb
    testb %%al,%%al
    jne 1b
   "
   : "=&S" (d0), "=&D" (d1), "=&a" (d2)
   : "0" (source), "1" (destination)
   : "memory");
  return destination;
}

/* Copy a string, but no more than 'count' bytes. */

static inline char *string_copy_max (char *destination, const char *source,
                                     unsigned int count)
{
  int esi, edi, ecx, eax;

  asm volatile
  ("\
    cld
1:  decl %2
    js 2f
    lodsb
    stosb
    testb %%al,%%al
    jne 1b
    rep
    stosb
2:"
   : "=&S" (esi), "=&D" (edi), "=&c" (ecx), "=&a" (eax)
   : "0" (source), "1" (destination), "2" (count) 
   : "memory");
  return destination;
}

/* Compare two strings. Returns 0 if equal, negative if the first
   string is 'less' than the second, or otherwise positive. */

static inline int string_compare (const char *string1, const char *string2)
{
  int counter = -1;

  do 
  {
    counter++;

    if (string1[counter] < string2[counter])
    {
      return -1;
    }
    
    if (string1[counter] > string2[counter])
    {
      return 1;
    }

  } while (string1[counter] != '\0' && string2[counter] != '\0');

  return 0;
}


/* Compare two strings, but no more than count characters. */

static inline int string_compare_max
  (const char *string1, const char *string2, unsigned int count)
{
  register int eax;
  int esi, edi, ecx;

  asm volatile
  ("\
    cld
1:  decl %3
    js 2f
    lodsb
    scasb
    jne 3f
    testb %%al, %%al
    jne 1b
2:  xorl %%eax, %%eax
    jmp 4f
3:  sbbl %%eax, %%eax
    orb $1, %%al
4:
"
   : "=a" (eax), "=&S" (esi), "=&D" (edi), "=&c" (ecx)
   : "1" (string1), "2" (string2), "3" (count));

  return eax;
}

/* Returns the string length. */

static inline unsigned int string_length (const char *string)
{
  int edi;
  register int return_value;

  asm volatile
  ("\
    cld
    repne
    scasb
    notl %0
    decl %0
  "
   : "=c" (return_value), "=&D" (edi)
   : "1" (string), "a" (0), "0" (0xffffffff));
  return return_value;
}

/* Returns the string length, but only if it is less than size. */

static inline unsigned int string_length_max 
  (const char *string, unsigned int count)
{
  int edi;
  register int return_value;

  asm volatile
  ("\
    movl %2, %0
    jmp 2f\n
 1: cmpb $0, (%0)
    je 3f
    incl %0
 2: decl %1
    cmpl $-1, %1
    jne 1b
 3: subl %2, %0
"
   : "=a" (return_value), "=&d" (edi)
   :"c" (string),"1" (count));

  return return_value;
}

#endif /* !__STORM_IA32_STRING_H__ */
