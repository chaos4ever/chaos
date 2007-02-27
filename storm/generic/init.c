/* $Id$ */
/* Abstract: Initialise the state of a new process. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
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

#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/init.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/mutex.h>
#include <storm/generic/return_values.h>
#include <storm/generic/thread.h>

/* If defined as TRUE, this code will print lots of debugging
   information. */

#define DEBUG FALSE

/* This system call should be the first system call the process
   runs. It will perform such initialisation that can only (easily) be
   performed from within the process' own addressing space.

   Currently, all it does is configure the process' virtual memory AVL
   tree correctly, so that the process can allocate memory. */

return_type init (void)
{
  /* Clear the bitmap, marking all slots as free. */

  memory_set_u8 ((u8 *) process_avl_header->bitmap, 0, 
                 SIZE_PAGE - sizeof (avl_header_type));

  /* Mark the first entry in the bitmap as used. */

  process_avl_header->bitmap[0] = 1;

  /* Plundis: I'm not really sure if this is needed, but I presume
     memory_virtual_reserve () could happen to allocate memory at
     times? */

  mutex_kernel_wait (&tss_tree_mutex);

  /* Process AVL tree. */

  DEBUG_MESSAGE (DEBUG, "Reserving");
  memory_virtual_reserve (GET_PAGE_NUMBER (BASE_PROCESS_AVL_TREE), 
                          process_avl_header->limit_pages_bitmap +
                          process_avl_header->limit_pages_array);

  /* Page tables. */

  DEBUG_MESSAGE (DEBUG, "Reserving");
  memory_virtual_reserve (GET_PAGE_NUMBER (BASE_PROCESS_PAGE_TABLES), 1024);

  /* Global memory. */

  DEBUG_MESSAGE (DEBUG, "Reserving");
  memory_virtual_reserve (GET_PAGE_NUMBER (BASE_GLOBAL), 
                          SIZE_IN_PAGES (SIZE_GLOBAL));

  /* Stack. */

  DEBUG_MESSAGE (DEBUG, "Reserving");
  memory_virtual_reserve (GET_PAGE_NUMBER (BASE_PROCESS_STACK), 
                          SIZE_IN_PAGES (SIZE_PROCESS_STACK));

  /* Reserve the code and and data pages. */

  DEBUG_MESSAGE (DEBUG, "Reserving");
  memory_virtual_reserve (GET_PAGE_NUMBER (current_tss->virtual_code_base),
                          current_tss->code_pages);
  memory_virtual_reserve (GET_PAGE_NUMBER (current_tss->virtual_data_base),
                          current_tss->data_pages);

  mutex_kernel_signal (&tss_tree_mutex);

  current_tss->initialised = TRUE;

  DEBUG_MESSAGE (DEBUG, "Done");

  return STORM_RETURN_SUCCESS;
}
