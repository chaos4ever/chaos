/* $Id$ */
/* Abstract: Inlined functions. */
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

#ifndef __LIBRARY_STRING_INLINES_H__
#define __LIBRARY_STRING_INLINES_H__

/* Inlined functions. */
/* Copy a string. */

static inline char *string_copy (char *destination, const char *source)
{
  int esi, edi, eax;

  asm volatile 
  ("\
    cld
1:  lodsb
    stosb
    testb %%al,%%al
    jne 1b
   "
   : "=&S" (esi), "=&D" (edi), "=&a" (eax)
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

/* Compare two strings. Returns 0 if equal. */

static inline int string_compare (const char *string1, const char *string2)
{
  int esi, edi;
  register int result;

  asm volatile
  ("\
    cld
1:  lodsb
    scasb
    jne 2f
    testb %%al, %%al
    jne 1b
    xorl %%eax, %%eax
    jmp 3f
2:  sbbl %%eax, %%eax
    orb $1, %%al
3:
"
   : "=a" (result), "=&S" (esi), "=&D" (edi)
   : "1" (string1), "2" (string2));

  return result;
}


/* Compare two strings, but no more than count characters. */

static inline int string_compare_max (const char *string1, const char *string2,
                                      unsigned int count)
{
  /* FIXME: Change those names. */

  register int __res;
  int d0, d1, d2;

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
   : "=a" (__res), "=&S" (d0), "=&D" (d1), "=&c" (d2)
   : "1" (string1), "2" (string2), "3" (count));

  return __res;
}

/* Returns the string length. */

static inline unsigned int string_length (const char *string)
{
  int d0;
  register int __res;

  asm volatile
  ("\
    cld
    repne
    scasb
    notl %0
    decl %0
  "
   : "=c" (__res), "=&D" (d0)
   : "1" (string), "a" (0), "0" (0xffffffff));
  return __res;
}

/* Returns the string length, but only if it is less than size. */

static inline unsigned int string_length_max (const char *string, 
                                              unsigned int count)
{
  int d0;
  register int __res;

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
   : "=a" (__res), "=&d" (d0)
   :"c" (string),"1" (count));
  return __res;
}

static inline void string_append (char *destination, char *source)
{
  string_copy (&destination[string_length (destination)], source);
}

#endif /* !__LIBRARY_STRING_INLINES_H__ */
