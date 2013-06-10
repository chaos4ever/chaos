// Abstract: Functions for copying memory.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 1999-2000, 2007, 2013 chaos development.

#pragma once

extern inline void *memory_copy(void *to, const void *from, int n);

// TODO: Add architecture-optimized versions of this, probably as header files in storm by then.
 void *memory_copy(void * restrict to, const void * restrict from, int n)
 {
   char *dst = to;
   const char *src = from;

   while (n-- != 0)
   {
       *dst++ = *src++;
   }

   return to;
 }
