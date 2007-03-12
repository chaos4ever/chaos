/* $Id$ */
/* Abstract: Function prototypes and structures used by the virtual
             memory routines. */
/* Authors: Per Lundberg <plundis@chaosdev.org> 
            Henrik Hallin <hal@chaosdev.org> */

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
   USA */

#ifndef __STORM_IA32_MEMORY_VIRTUAL_H__
#define __STORM_IA32_MEMORY_VIRTUAL_H__

#include <storm/generic/types.h>
#include <storm/return_values.h>

/* Supervisor-only, read-only (since we don't set the WP bit, it isn't
   really read-only for the kernel). */

#define PAGE_KERNEL             0

/* Standard flags. */

#define PAGE_WRITABLE           BIT_VALUE (0)
#define PAGE_NON_PRIVILEGED     BIT_VALUE (1)
#define PAGE_WRITE_THROUGH      BIT_VALUE (2)
#define PAGE_CACHE_DISABLE      BIT_VALUE (3)
#define PAGE_GLOBAL             BIT_VALUE (4)

/* The following flags are used when creating new entries in the page
   directory. */

#define PAGE_DIRECTORY_FLAGS     (PAGE_WRITABLE | PAGE_NON_PRIVILEGED)

/* Type definitions. */

typedef struct
{
  u32 present           : 1;
  u32 flags             : 4;
  u32 accessed          : 1;
  u32 dirty             : 1;

  /* Should always be one. */

  u32 page_size         : 1;
  u32 global            : 1;
  u32 available         : 3;

  /* Obvious? */

  u32 zero              : 10;
  u32 page_base         : 10;
} page_directory_4mb_type;

typedef struct
{
  u32 present           : 1;
  u32 flags             : 4;
  u32 accessed          : 1;
  u32 zero              : 1;

  /* Should always be zero. */

  u32 page_size         : 1;
  u32 global            : 1;
  u32 available         : 3;
  u32 page_table_base   : 20;
} page_directory_type;

typedef struct
{
  u32 present           : 1;
  u32 flags             : 4;
  u32 accessed          : 1;
  u32 dirty             : 1;
  u32 zero              : 1;
  u32 global            : 1;
  u32 available         : 3;
  u32 page_base         : 20;
} page_table_type;

typedef page_directory_type address_space_type;

/* Inlines. */

static inline void memory_virtual_cache_invalidate (void *address)
{
  /* If we're 486 or better, invalidate the cache. If not, we don't
     have a cache and thus we don't have to invalidate it. */
  
  //  if (cpu_info.family >= 4)
  //  {
    asm ("invlpg %0"
         :
         : "m" (*(u8 *) address)
         : "memory");
    //  }
}

#endif /* !__STORM_IA32_MEMORY_VIRTUAL_H__ */
