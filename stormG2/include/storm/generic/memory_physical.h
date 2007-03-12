/* $Id$ */
/* Abstract: Physical memory allocation functions. */

/* Author: Henrik Hallin <hal@chaosdev.org> 
           Per Lundberg <plundis@chaosdev.org> */

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

#ifndef __STORM_GENERIC_MEMORY_PHYSICAL_H__
#define __STORM_GENERIC_MEMORY_PHYSICAL_H__

#include <storm/current-arch/memory_physical.h>

/* External variables. */

extern unsigned int number_of_physical_pages;

/* Function prototypes. */

extern void memory_physical_init (void);
extern void *memory_physical_allocate (void);
extern void memory_physical_deallocate (void *physical_pointer);
extern void memory_physical_update_pointers (void);
extern unsigned int memory_physical_get_allocated_pages (void);

#endif /* !__STORM_GENERIC_MEMORY_PHYSICAL_H__ */
