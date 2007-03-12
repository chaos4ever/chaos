/* $Id$ */
/* Abstract: Functions for memory operations. */
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

#ifndef __STORM_GENERIC_MEMORY_H__
#define __STORM_GENERIC_MEMORY_H__

#include <storm/current-arch/memory.h>

#include <storm/generic/mutex.h>

extern mutex_kernel_type memory_mutex;

/* Prototypes. */

extern return_type memory_allocate 
  (void **address, u32 pages, bool cacheable);

extern return_type memory_deallocate (void **address);

extern return_type memory_get_physical_address 
  (void *virtual_address, void **physical_address);

#endif /* !__STORM_GENERIC_MEMORY_H__ */
