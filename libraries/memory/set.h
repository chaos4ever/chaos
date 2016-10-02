/* $Id$ */
/* Abstract: Functions for setting memory blocks. */
/* Author: Per Lundberg <per@chaosdev.io> 
   
   Probably a bit ripped off of Linux or similar. OTOH, it is hard to
   write these functions differently to how they are currently
   written... */

/* Copyright 1999-2000 chaos development. */
/* Copyright 2007 chaos development. */

#ifndef __LIBRARY_MEMORY_SET_H__
#define __LIBRARY_MEMORY_SET_H__

#include <system/system.h>

/* Inlines. */

/* Set memory in uint8_t:s. */
/* FIXME: If multiples of words, use stosd instead of stosb. */

static inline void memory_set_uint8_t (uint8_t *address, uint8_t c, unsigned int size)
{
  int ecx, edi;

  asm volatile
  ("cld\n"
   "rep\n"
   "stosb"
   : "=&c" (ecx), "=&D" (edi)
   : "a" (c), "1" (address), "0" (size)
   : "memory");
}

/* Set memory in uint16_t:s. */

static inline void memory_set_uint16_t (uint16_t *address, uint16_t c, unsigned int size)
{
  int ecx, edi;

  asm volatile
  ("cld\n"
   "rep\n"
   "stosw"
   : "=&c" (ecx), "=&D" (edi)
   : "a" (c), "1" (address), "0" (size)
   : "memory");
}

/* Set memory in u32:s. */

static inline void memory_set_uint32_t (uint32_t *address, uint32_t c, unsigned int size)
{
  int ecx, edi;

  asm volatile
  ("cld\n"
   "rep\n"
   "stosl"
   : "=&c" (ecx), "=&D" (edi)
   : "a" (c), "1" (address), "0" (size)
   : "memory");
}

#endif /* !defined __LIBRARY_MEMORY_SET_H__ */
