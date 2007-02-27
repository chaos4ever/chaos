/* $Id$ */
/* Abstract: Architecture-specific memory stuff. */
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

#include <storm/generic/defines.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/return_values.h>
#include <storm/generic/types.h>

/* Get the physical address for the given virtual address. */

return_type memory_get_physical_address (void *virtual_address, 
                                         void **physical_address)
{
  u32 page_directory_index = ((u32) virtual_address / SIZE_PAGE) / 1024;
  u32 page_table_index = ((u32) virtual_address / SIZE_PAGE) % 1024;
  page_table_entry *page_table;

  /* Make sure the page table we are going to access is mapped. */

  if (process_page_directory[page_directory_index].present == 0)
  {
    return STORM_RETURN_INVALID_ARGUMENT;
  }

  page_table = (page_table_entry *) (BASE_PROCESS_PAGE_TABLES + 
                                     (page_directory_index * SIZE_PAGE));
  
  /* Is this page mapped? */

  if (page_table[page_table_index].present == 0)
  {
    return STORM_RETURN_INVALID_ARGUMENT;
  }

  *physical_address = (void *) (page_table[page_table_index].page_base *
                                SIZE_PAGE);

  return STORM_RETURN_SUCCESS;
}
