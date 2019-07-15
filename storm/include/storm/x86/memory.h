// Abstract: Functions for memory operations.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#pragma once

#include <storm/generic/types.h>

#ifdef MMX

// MMX optimized functions. Will not work on non-MMX systems.
// FIXME: Doesn't seem to work 100%...
static inline void *memory_copy(void *to, const void *from, int n)
{
    uint32_t ebx, ecx, edi, esi;

    asm volatile
    (
        "\n\
    testl       %%ecx, %%ecx \n\
    jz          2f \n\
1:  movq        (%%esi), %%mm0 \n\
    movq        %%mm0, (%%edi) \n\
    addl        $8, %%esi \n\
    addl        $8, %%edi \n\
    loop        1b \n\
\n\
2:  movl        %%ebx, %%ecx \n\
    cld \n\
    rep         movsb		/* Move 1 byte slack. */ \n\
  "
        : "=&b" (ebx), "=&c" (ecx), "=&D" (edi), "=&S" (esi)
        : "0" (n % 8), "1" (n / 8), "2" (to), "3" (from)
        : "memory");

    return to;
}

#if FALSE
static inline void memory_set_uint8_t(uint8_t *address, uint8_t c, unsigned int length)
{
    int ebx, ecx, edi;

    asm volatile
    (
        "\n\
    testl       %%ecx, %%ecx \n\
    jz          2f \n\
    movd        %%eax, %%mm0 \n\
1:  movq        %%mm0, (%%edi) \n\
    addl        $8, %%edi \n\
    loop        1b \n\
\n\
    movl        %%ebx, %%ecx \n\
2:  cld \n\
    rep         stosb           /* Copy 1 byte slack. */ \n\
   "
        : "=&b" (ebx), "=&c" (ecx), "=&D" (edi)
        : "a" (c), "0" (length % 8), "1" (length / 8), "2" (address)
        : "memory");
}
#endif

#else

static inline void *memory_copy(void *to, const void *from, int n)
{
    int ecx, edi, esi;

    asm volatile
    (
        "cld \n\n\
    rep ; movsl \n\
    testb $2, %b4 \n\
    je 1f \n\
    movsw \n\
 1: testb $1, %b4 \n\
    je 2f \n\
    movsb \n\
 2:"
        : "=&c" (ecx), "=&D" (edi), "=&S" (esi)
        : "0" (n / 4), "q" (n), "1" ((long) to), "2" ((long) from)
        : "memory");

    return (to);
}

#endif

static inline void memory_set_uint8_t(uint8_t *address, uint8_t c, unsigned int size)
{
    int ecx, edi;

    asm volatile
    ("cld \n\
    rep \n\
    stosb"
     : "=&c" (ecx), "=&D" (edi)
     : "a" (c), "1" (address), "0" (size)
     : "memory");
}

static inline void memory_set_uint16_t(uint16_t *addr, uint16_t c, unsigned int size)
{
    int d0, d1;
    asm volatile
    ("\
    cld \n\
    rep \n\
    stosw \n\
   "
     : "=&c" (d0), "=&D" (d1)
     : "a" (c), "1" (addr), "0" (size)
     : "memory");
}
