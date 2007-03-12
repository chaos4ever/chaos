/* $Id$ */
/* Abstract: Function prototypes and structures used by the virtual
   memory routines. */
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

#ifndef __STORM_GENERIC_MEMORY_VIRTUAL_H__
#define __STORM_GENERIC_MEMORY_VIRTUAL_H__

#include <storm/generic/avl.h>
#include <storm/generic/bit.h>
#include <storm/generic/cpu.h>
#include <storm/generic/types.h>
#include <storm/current-arch/tss.h>

/* Get the architecture specifics. */

#include <storm/current-arch/memory_virtual.h>

/* Prototypes. */

extern void memory_virtual_init (void) INIT_CODE;
extern void memory_virtual_enable (void) INIT_CODE;

extern return_type memory_virtual_map_kernel
  (page_directory_entry_page_table *page_directory, u32 virtual_page,
   u32 physical_page, u32 pages, u32 flags) INIT_CODE;

extern return_type memory_virtual_map_other 
  (storm_tss_type *tss, u32 virtual_page, u32 physical_page, u32 pages,
   u32 flags);

extern return_type memory_virtual_map
  (u32 virtual_page, u32 physical_page, u32 pages, u32 flags);

extern void memory_virtual_unmap (u32 virtual_page, u32 pages);

extern return_type map (process_id_type pid, u32 linear_page,
                        u32 physical_page, u32 pages);

extern page_directory_entry_page_table *memory_virtual_clone_page_directory
  (page_directory_entry_page_table *source);

extern void memory_virtual_dump_map
  (page_directory_entry_page_table *page_directory);

extern return_type memory_virtual_allocate (u32 *page_number,
                                            u32 pages);
extern return_type memory_virtual_deallocate (u32 page_number);
extern return_type memory_virtual_reserve (unsigned int start_page,
                                           unsigned int pages);

/* Global variables. */

extern avl_header_type *process_avl_header;
extern page_directory_entry_page_table *kernel_page_directory;
extern page_directory_entry_page_table *process_page_directory;
extern page_table_entry *shared_page_tables;

#endif /* !__STORM_GENERIC_MEMORY_VIRTUAL_H__ */
