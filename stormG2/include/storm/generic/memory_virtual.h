/* $Id$ */
/* Abstract: Virtual memory prototypes. */

/* Author: Per Lundberg <plundis@chaosdev.org>
           Henrik Hallin <hal@chaosdev.org> */

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

#ifndef __STORM_GENERIC_MEMORY_VIRTUAL_H__
#define __STORM_GENERIC_MEMORY_VIRTUAL_H__

#include <storm/generic/types.h>

/* Get the architecture specifics. */

#include <storm/current-arch/memory_virtual.h>

/* Function prototypes. */

extern void memory_virtual_init (void);
extern return_type memory_virtual_map
  (address_space_type *address_space, unsigned int virtual_page,
   unsigned int physical_page, unsigned int pages,
   unsigned int flags);
extern void memory_virtual_unmap
  (address_space_type *address_space, unsigned int virtual_page,
   unsigned int pages);
void *memory_virtual_to_physical
  (address_space_type *address_space, void *virtual_address);

/* External variables. */

extern page_directory_type *kernel_page_directory;

#endif /* !__STORM_GENERIC_MEMORY_VIRTUAL_H__ */
