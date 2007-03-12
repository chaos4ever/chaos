/* $Id$ */
/* Abstract: Function prototypes and structure definitions for the
   physical memory allocation system. */
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
   USA */

#ifndef __STORM_GENERIC_MEMORY_PHYSICAL_H__
#define __STORM_GENERIC_MEMORY_PHYSICAL_H__

#include <storm/generic/avl.h>
#include <storm/generic/types.h>

/* Function prototypes. */

extern void memory_physical_init (void) INIT_CODE;

extern return_type memory_physical_reserve 
  (unsigned int start, unsigned int length);

extern return_type memory_physical_allocate 
  (u32 *page, unsigned int length, char *description);

extern return_type memory_physical_deallocate (unsigned int start);

extern u32 memory_physical_get_number_of_pages (void);
extern u32 memory_physical_get_free (void);
extern u32 memory_physical_get_used (void);

/* Global variables. */

extern avl_header_type *page_avl_header;
extern u32 page_avl_pages;

#endif /* !__STORM_GENERIC_MEMORY_PHYSICAL_H__ */
