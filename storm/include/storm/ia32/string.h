// Abstract: Inline string routines.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000, 2013 chaos development.

#pragma once

#include <storm/generic/defines.h>
#include <storm/generic/types.h>

// FIXME: Clean up those functions.
static inline char *string_copy(char *destination, const char *source)
{
    int d0, d1, d2;
    asm volatile
    ("cld\n"
     "1: lodsb\n"
     "stosb\n"
     "testb %%al,%%al\n"
     "jne 1b\n"
     : "=&S" (d0), "=&D" (d1), "=&a" (d2)
     : "0" (source), "1" (destination)
     : "memory");

    return destination;
}

// Copy a string, but no more than 'count' bytes.
static inline char *string_copy_max(char *destination, const char *source, unsigned int count)
{
    int esi, edi, ecx, eax;

    asm volatile
    ("cld\n"
     "1: decl %2\n"
     "js 2f\n"
     "lodsb\n"
     "stosb\n"
     "testb %%al,%%al\n"
     "jne 1b\n"
     "rep\n"
     "stosb\n"
     "2:"
     : "=&S" (esi), "=&D" (edi), "=&c" (ecx), "=&a" (eax)
     : "0" (source), "1" (destination), "2" (count)
     : "memory");

    return destination;
}

// Compare two strings. Returns 0 if equal, negative if the first string is 'less' than the second, or otherwise positive.
static inline int string_compare(const char *string1, const char *string2)
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

// Compare two strings, but no more than count characters.
static inline int string_compare_max(const char *string1, const char *string2, unsigned int count)
{
    register int eax;
    int esi, edi, ecx;

    asm volatile
    (
        "cld \
1: decl %3 \
   js 2f \
   lodsb \
   scasb \
   jne 3f \
   testb %%al, %%al \
   jne 1b \
2: xorl %%eax, %%eax \
   jmp 4f \
3: sbbl %%eax, %%eax \
   orb $1, %%al \
4:"
        : "=a" (eax), "=&S" (esi), "=&D" (edi), "=&c" (ecx)
        : "1" (string1), "2" (string2), "3" (count));

    return eax;
}

static inline unsigned int string_length(const char *string)
{
    int edi;
    register int return_value;

    asm volatile
    ("cld\n"
     "repne\n"
     "scasb\n"
     "notl %0\n"
     "decl %0"
     : "=c" (return_value), "=&D" (edi)
     : "1" (string), "a" (0), "0" (0xffffffff));

    return return_value;
}

// Returns the string length, but only if it is less than size.
static inline unsigned int string_length_max(const char *string, unsigned int count)
{
    int edi;
    register int return_value;

    asm volatile
    ("movl %2, %0\n"
     "jmp 2f\n"
     "1: cmpb $0, (%0)\n"
     "je 3f\n"
     "incl %0\n"
     "2: decl %1\n"
     "cmpl $-1, %1\n"
     "jne 1b\n"
     "3: subl %2, %0"
     : "=a" (return_value), "=&d" (edi)
     : "c" (string), "1" (count));

    return return_value;
}
