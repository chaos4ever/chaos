/* $Id$ */
/* Abstract: Physical memory stuff. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

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

#ifndef __STORM_IA32_MEMORY_PHYSICAL_H__
#define __STORM_IA32_MEMORY_PHYSICAL_H__

#include <storm/generic/types.h>
#include <storm/generic/defines.h>

/* Type definitions. */

typedef struct
{
  struct physical_page_slab_type *previous;
  struct physical_page_slab_type *next;
} physical_page_slab_type;

/* Inline functions. */

static inline void *memory_physical_to_virtual (void *physical_address)
{
  return (void *) ((u32) physical_address + BASE_PHYSICAL_MEMORY);
}

#endif /* !__STORM_IA32_MEMORY_PHYSICAL_H__ */
