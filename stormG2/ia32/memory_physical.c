/* $Id$ */
/* Abstract: Physical memory allocation. */

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

#include <storm/generic/memory_physical.h>
#include <storm/generic/defines.h>
#include <storm/generic/multiboot.h>
#include <storm/generic/debug.h>

#define DEBUG TRUE

static physical_page_slab_type *first_free_page = (physical_page_slab_type *)
  BASE_FIRST_FREE_PHYSICAL_MEMORY;
unsigned int number_of_physical_pages;
unsigned int number_of_allocated_pages = 0;

/* This function builds a slab-like system on the physical memory and
   reserves parts marked as reserved by BIOS. */

void memory_physical_init (void)
{
  unsigned int index;
  physical_page_slab_type *page;
  physical_page_slab_type *previous_page = NULL;

  debug_print ("Initializing physical memory allocation...\n");

  number_of_physical_pages = (multiboot_info.memory_upper + 1024) / 4;

  /* Do the first MB of memory. */

  for (index = GET_PAGE_NUMBER (BASE_FIRST_FREE_PHYSICAL_MEMORY);
       index < GET_PAGE_NUMBER (BASE_RESERVED_MEMORY); index++)
  {
    page = (physical_page_slab_type *) (index * SIZE_PAGE);
    page->previous = (struct physical_page_slab_type *) previous_page;
    page->next = NULL;

    if (previous_page != NULL)
    {
      previous_page->next = (struct physical_page_slab_type *) page;
    }
    previous_page = page;
  }

  /* Do the rest of the memory. */

  for (index = GET_PAGE_NUMBER (BASE_KERNEL) + SIZE_IN_PAGES (SIZE_KERNEL);
       index < number_of_physical_pages; index++)
  {
    page = (physical_page_slab_type *) (index * SIZE_PAGE);
    page->previous = (struct physical_page_slab_type *) previous_page;
    page->next = NULL;

    if (previous_page != NULL)
    {
      previous_page->next = (struct physical_page_slab_type *) page;
    }
    previous_page = page;
  }

  DEBUG_MESSAGE (DEBUG, "%u physical pages (%u MB).",
                 number_of_physical_pages,
                 number_of_physical_pages * SIZE_PAGE / MB);
}

/* Update all pointers to point into the mapped physical memory. */

void memory_physical_update_pointers (void)
{
  physical_page_slab_type *current;
  physical_page_slab_type *next;

  current = first_free_page;

  while (current != NULL)
  {
    next = (physical_page_slab_type *) current->next;

    if (current->next != NULL)
    {
      current->next =
        memory_physical_to_virtual (current->next);
    }

    if (current->previous != NULL)
    {
      current->previous =
        memory_physical_to_virtual (current->previous);
    }

    if ((u32) next % SIZE_PAGE != 0)
    {
      DEBUG_HALT ("Memory allocation tree broken!");
    }
    current = next;
  }

  first_free_page = memory_physical_to_virtual (first_free_page);
}

/* Allocates a physical page. Returns NULL on failure. */

void *memory_physical_allocate (void)
{
  void *return_value = first_free_page;

  first_free_page = (physical_page_slab_type *) first_free_page->next;

  if (first_free_page == NULL)
  {
    return NULL;
  }
  else
  {
    number_of_allocated_pages++;
    return return_value;
  }
}

/* Deallocate a physical page. */

void memory_physical_deallocate (void *physical_pointer)
{
  physical_page_slab_type *temporary;

  if (((u32) physical_pointer % SIZE_PAGE) != 0 ||
      (u32) physical_pointer < BASE_PHYSICAL_MEMORY)
  {
    /* This is not a valid page. */

    return;
  }

  /* Deallocate the page. */

  temporary = first_free_page;
  first_free_page = (physical_page_slab_type *) physical_pointer;

  first_free_page->previous = NULL;
  first_free_page->next = (struct physical_page_slab_type *) temporary;

  number_of_allocated_pages--;
}

/* Get the amount of allocated memory. */

unsigned int memory_physical_get_allocated_pages (void)
{
  return number_of_allocated_pages;
}
