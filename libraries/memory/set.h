// Abstract: Functions for setting memory blocks.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development.
//
// FIXME: Inspired by Linux source code, replace with MIT/BSD licensed versions.

#pragma once

#include <system/system.h>

// Inlines.

// Set memory in bytes.
// FIXME: If multiples of words, could use stosd instead of stosb which is much more efficient.
static inline void memory_set_uint8_t(uint8_t *address, uint8_t c, unsigned int size)
{
    int ecx, edi;

    asm volatile
    ("cld\n"
     "rep\n"
     "stosb"
     : "=&c"(ecx), "=&D"(edi)
     : "a"(c), "1"(address), "0"(size)
     : "memory");
}

// Set memory in 16-bit integers.
static inline void memory_set_uint16_t (uint16_t *address, uint16_t c, unsigned int size)
{
    int ecx, edi;

    asm volatile
    ("cld\n"
     "rep\n"
     "stosw"
     : "=&c"(ecx), "=&D"(edi)
     : "a"(c), "1"(address), "0"(size)
     : "memory");
}

// Set memory in 32-bit integers.
static inline void memory_set_uint32_t(uint32_t *address, uint32_t c, unsigned int size)
{
    int ecx, edi;

    asm volatile
    ("cld\n"
     "rep\n"
     "stosl"
     : "=&c"(ecx), "=&D"(edi)
     : "a"(c), "1"(address), "0"(size)
     : "memory");
}
