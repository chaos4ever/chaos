/* $Id$ */
/* Abstract: Provides functions for managing the virtual memory (MMU)
             mechanisms of the IA32 architecture. */

/* Authors: Henrik Hallin <hal@chaosdev.org>
            Per Lundberg <plundis@chaosdev.org> */

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

#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/ia32/cpu.h>
#include "config.h"

#define DEBUG TRUE

page_directory_type *kernel_page_directory;

/* Create an entry in a page table. */

static void create_page_table_entry
  (page_table_type *page_table, unsigned int entry,
   unsigned int physical_page, unsigned int flags)
{
  page_table[entry].present = 1;
  page_table[entry].flags = (flags & 0xF);
  page_table[entry].accessed = 0;
  page_table[entry].zero = 0;
  page_table[entry].dirty = 0;
  page_table[entry].global = ((flags & PAGE_GLOBAL) == 1) ? 1 : 0;
  page_table[entry].available = 0;
  page_table[entry].page_base = physical_page;
}

/* Create an entry in a page directory. */

static void create_page_directory_entry
  (page_directory_type *page_directory, unsigned int entry,
   unsigned int page_table_physical_page, unsigned int flags)
{
  page_directory[entry].present = 1;
  page_directory[entry].flags = PAGE_DIRECTORY_FLAGS;
  page_directory[entry].accessed = 0;
  page_directory[entry].zero = 0;
  page_directory[entry].page_size = 0;
  page_directory[entry].global = ((flags & PAGE_GLOBAL) == 1) ? 1 : 0;
  page_directory[entry].available = 0;
  page_directory[entry].page_table_base = page_table_physical_page;
}

/* Create an entry in a page directory using 4 MB pages. */

static void create_page_directory_entry_4mb
  (page_directory_type *page_directory, unsigned int entry,
   unsigned int page_table_physical_page, unsigned int flags)
{
  page_directory[entry].present = 1;
  page_directory[entry].flags = PAGE_DIRECTORY_FLAGS;
  page_directory[entry].accessed = 0;
  page_directory[entry].zero = 0;
  page_directory[entry].page_size = 1;
  page_directory[entry].global = ((flags & PAGE_GLOBAL) == 1) ? 1 : 0;
  page_directory[entry].available = 0;
  page_directory[entry].page_table_base = page_table_physical_page;
}

/* Initialise the virtual memory system. */

void memory_virtual_init (void)
{
  page_table_type *shared_low_page_table;
  unsigned int counter, page_directory_entry, page_table_entry;

  debug_print ("Creating virtual memory structures...\n");

  shared_low_page_table = memory_physical_allocate ();
  kernel_page_directory = memory_physical_allocate ();

  /* Set up the low shared 4 MB. */

  memory_set_u8 ((u8 *) kernel_page_directory, 0, SIZE_PAGE);
  memory_set_u8 ((u8 *) shared_low_page_table, 0, SIZE_PAGE);

  /* Map the GDT and IDT. */

  create_page_table_entry (shared_low_page_table, GET_PAGE_NUMBER (BASE_GDT), 
                           GET_PAGE_NUMBER (BASE_GDT), PAGE_GLOBAL);

  /* Map the kernel. */

  for (counter = 0; counter < SIZE_IN_PAGES (SIZE_KERNEL); counter++)
  {
    create_page_table_entry (shared_low_page_table,
                             GET_PAGE_NUMBER (BASE_KERNEL) + counter,
                             GET_PAGE_NUMBER (BASE_KERNEL) + counter,
                             PAGE_GLOBAL);
  }

  /* Set up a page directory for the kernel which will also be used by
     the idle thread(s). Let's start by mapping the low shared 4
     MB. */

  create_page_directory_entry (kernel_page_directory, 0, 
                               GET_PAGE_NUMBER ((u32) shared_low_page_table),
                               PAGE_GLOBAL);

  if (cpu.pse == 1)
  {
    /* If the CPU supports 4 MB pages we don't need to allocate any
       page directories at all. We just map the low 2 GB of memory
       right in the page directory. */

    DEBUG_MESSAGE (DEBUG, "Using large pages.");

    cpu_set_cr4 (cpu_get_cr4 () | CPU_CR4_PSE |
                 (cpu.pge == 1 ? CPU_CR4_PGE : 0));

    for (page_directory_entry = 0; page_directory_entry < 512;
         page_directory_entry++)
    {
      create_page_directory_entry_4mb (kernel_page_directory,
                                       page_directory_entry + 512,
                                       GET_PAGE_NUMBER (page_directory_entry
                                                        * 4 * MB),
                                       PAGE_GLOBAL);
    }
  }
  else
  {
    /* We have an old CPU which forces us to waste a lot of memory on
       page tables. ;) */
    
    DEBUG_MESSAGE (DEBUG, "Not using large pages.");
    
    /* Map all physical memory. */
    
    for (page_directory_entry = 0; 
         page_directory_entry < number_of_physical_pages / 1024 + 
           ((number_of_physical_pages % 1024) != 0 ? 1 : 0);
         page_directory_entry++)
    {
      void *page_table = memory_physical_allocate ();
      
      create_page_directory_entry (kernel_page_directory, 
                                   BASE_PHYSICAL_MEMORY / (4 * MB) + 
                                   page_directory_entry, 
                                   GET_PAGE_NUMBER (page_table),
                                   PAGE_GLOBAL);
      
      for (page_table_entry = 0; page_table_entry < 1024; page_table_entry++)
      {
        create_page_table_entry (page_table, page_table_entry,
                                 GET_PAGE_NUMBER (page_directory_entry
                                                  * 4 * MB) + page_table_entry,
                                 PAGE_GLOBAL);
      }
    }
  }

  /* We have to update the physical memory allocation structures,
     since the physical memory will be moved. */

  memory_physical_update_pointers ();

#if !OPTION_RELEASE
  screen = memory_physical_to_virtual ((void *) BASE_SCREEN);
#endif

  /* Now it is time to actually enable paging. Specify page directory
     to use for the kernel. */

  cpu_set_cr3 ((u32) kernel_page_directory); 

  /* Before we enable paging, we have to move the stackpointer to the
     new stack location. */

  cpu_set_esp (cpu_get_esp () + BASE_PHYSICAL_MEMORY);

  /* Enable paging (virtual memory), protection and set the extension
     type flag. */

  cpu_set_cr0 (CPU_CR0_PE | CPU_CR0_PG | CPU_CR0_ET);

  /* FIXME: Re-implement the CPU probing. */

#if FALSE
  /* If PGE is available, set the PGE bit in CR4. */

  if (cpu_info.flags.flags.pge == 1)
  {
    cpu_set_cr4 (CPU_CR4_PGE);
  }
#endif
}

/* This function is for mapping physical memory in an address
   space. */
  
return_type memory_virtual_map
  (address_space_type *address_space UNUSED, unsigned int virtual_page UNUSED,
   unsigned int physical_page UNUSED, unsigned int pages UNUSED,
   unsigned int flags UNUSED)
{
#if FALSE
  page_table_type *page_table;
  u32 counter, index;

  for (counter = 0; counter < pages; counter++)
  {  
    index = (virtual_page + counter) / 1024;
    
    DEBUG_MESSAGE (DEBUG, "index = %u, counter = %u", index, counter);

    if (process_page_directory[index].present == 0)
    {
      u32 page_table_page;

      /* Page Table is not set up yet. Let's set up a new one. */

      process_page_directory[index].present = 1;
      process_page_directory[index].flags = PAGE_DIRECTORY_FLAGS;
      process_page_directory[index].accessed = 0;
      process_page_directory[index].zero = 0;
      process_page_directory[index].page_size = 0;
      process_page_directory[index].global = 0;
      process_page_directory[index].available = 0;

      /* FIXME: Check return value. */

      DEBUG_MESSAGE (DEBUG, "Allocating memory for a new page table.");
      memory_physical_allocate (&page_table_page, 1, "Process page table.");

      process_page_directory[index].page_table_base = page_table_page;

      //      memory_virtual_cache_invalidate
      //        ((void *) (process_page_directory[index].page table_base * SIZE_PAGE));
      
      /* Make sure we could allocate memory. */
      
      if (process_page_directory[index].page_table_base == 0)
      {
        return STORM_RETURN_OUT_OF_MEMORY;
      }

      DEBUG_MESSAGE (DEBUG, "Recursing.");

      memory_virtual_map_real
        (GET_PAGE_NUMBER (BASE_PROCESS_PAGE_TABLES) + index,
         (u32) process_page_directory[index].page_table_base, 1, PAGE_KERNEL);

      memory_set_u8 ((u8 *) (BASE_PROCESS_PAGE_TABLES + (index * SIZE_PAGE)),
                     0, SIZE_PAGE); 
    }

    /* The page table is in the page_directory. */

    
    page_table = (page_table_entry *)
      (BASE_PROCESS_PAGE_TABLES + (index * SIZE_PAGE));

    /* Which entry in the page table to modify. */

    index = (virtual_page + counter) % 1024;

    /* Set up a new page table entry. */

    /* FIXME: fix the flags to use defines. Both here and in the other
       functions. */

    DEBUG_MESSAGE (DEBUG, "slem! %x", page_table);

    page_table[index].present = 1;
    page_table[index].flags = flags;
    page_table[index].accessed = 0;
    page_table[index].dirty = 0;
    page_table[index].zero = 0;
    page_table[index].available = 0;
    page_table[index].page_base = physical_page + counter;

    /* FIXME: Only invalidate if present was 0. But there is maybe no
       performance loss in invalidating unconditionally? */

    DEBUG_MESSAGE (DEBUG, "Invalidating cache");

    memory_virtual_cache_invalidate
      ((void *) ((virtual_page + counter) * SIZE_PAGE));

    DEBUG_MESSAGE (DEBUG, "Done");
  }

  DEBUG_MESSAGE (DEBUG, "Returning");
#endif
  return STORM_RETURN_SUCCESS;
}

/* Unmap a region of pages for the current process. */

void memory_virtual_unmap
  (address_space_type *address_space UNUSED, unsigned int virtual_page UNUSED,
   unsigned int pages UNUSED)
{
}

/* Get the physical address for the given virtual address in an
   address space. */

void *memory_virtual_to_physical
  (address_space_type *address_space, void *virtual_address)
{
  address_space = address_space;
  virtual_address = virtual_address;
#if FALSE
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
                                SIZE_PAGE) + BASE_PHYSICAL_MEMORY;

#endif
  return STORM_RETURN_SUCCESS;
}
