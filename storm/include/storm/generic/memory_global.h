/* $Id$ */
/* Abstract: Routines for allocating memory in the global heap. */
/* Authors: Henrik Hallin <hal@chaosdev.org>
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

#ifndef __STORM_MEMORY_GENERIC_GLOBAL_H__
#define __STORM_MEMORY_GENERIC_GLOBAL_H__

#include <storm/generic/avl.h>
#include <storm/generic/defines.h>
#include <storm/generic/types.h>

/* Function prototypes. */

extern void memory_global_init (void) INIT_CODE;
extern void *memory_global_allocate (unsigned int length);
extern return_type memory_global_deallocate (void *block);
extern unsigned int memory_global_get_used (void);
extern unsigned int memory_global_get_free (void);

/* External variables. */

extern avl_header_type *global_avl_header;

#endif /* !__STORM_MEMORY_GENERIC_GLOBAL_H__ */
