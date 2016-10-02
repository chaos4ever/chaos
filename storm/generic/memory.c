// Abstract: Functions for memory operations.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>

// Copyright 1999-2000, 2013 chaos development.

#include <storm/generic/debug.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/mutex.h>
#include <storm/generic/types.h>
#include <storm/generic/return_values.h>

/* Define as TRUE if you want lots of debug information. */

#define DEBUG FALSE

/* Define this if you want to try the memory deallocation. */

#undef DEALLOCATE

mutex_kernel_type memory_mutex = MUTEX_UNLOCKED;

/* Allocate a memory block 'pages' pages in size, and put the
   resulting address in *address. */

return_type memory_allocate (void **address, uint32_t pages,
                             bool cacheable)
{
  uint32_t page_number;

  if (!current_tss->initialised)
  {
    return STORM_RETURN_ACCESS_DENIED;
  }

  mutex_kernel_wait (&memory_mutex);

  switch (memory_virtual_allocate (&page_number, pages))
  {
    case RETURN_OUT_OF_MEMORY:
    {
      mutex_kernel_signal (&memory_mutex);
      return STORM_RETURN_OUT_OF_MEMORY;
    }

    case RETURN_SUCCESS:
    {
      uint32_t physical_page;
      unsigned int flags = PAGE_WRITABLE | PAGE_NON_PRIVILEGED;

      if (!cacheable)
      {
        flags |= PAGE_CACHE_DISABLE;
      }

      /* FIXME: Check return value. */

      memory_physical_allocate (&physical_page, pages, 
                                "User-level allocation");

      memory_virtual_map (page_number, physical_page, pages,
                          flags);
      *address = (void *) ((uint32_t) page_number * SIZE_PAGE);
      mutex_kernel_signal (&memory_mutex);
      
      /* Make sure this memory is clean. Otherwise, we might have a
         security risk on our hand. */

      memory_set_uint8_t (*address, 0, pages * SIZE_PAGE);

      DEBUG_MESSAGE (DEBUG, "address = %p", *address);
      return STORM_RETURN_SUCCESS;
    }

    default:
    {
      DEBUG_HALT ("Unknown return value.");
    }
  }
}

/* Deallocate the given block. */
/* FIXME: Think about whether the virtual or physical memory should be
   deallocated first. */

return_type memory_deallocate (void **address)
{
  uint32_t page_number = ((uint32_t) *address) / SIZE_PAGE;

  if (!current_tss->initialised)
  {
    return STORM_RETURN_ACCESS_DENIED;
  }

#ifndef DEALLOCATE
  return STORM_RETURN_SUCCESS;
#endif

  mutex_kernel_wait (&memory_mutex);

  switch (memory_virtual_deallocate (page_number))
  {
    case RETURN_SUCCESS:
    {
      page_table_entry *page_table = (page_table_entry *)
        (BASE_PROCESS_PAGE_TABLES + (page_number / (1 * MB)) * SIZE_PAGE);
      uint32_t physical_page = page_table[page_number % 1024].page_base;
      
      /* FIXME: Check the return value from this call. But what do we
         do should it fail? */
      
      memory_physical_deallocate (physical_page);

      mutex_kernel_signal (&memory_mutex);
      *address = NULL;
      return STORM_RETURN_SUCCESS;
    }

    case RETURN_MEMORY_NOT_ALLOCATED:
    {
      mutex_kernel_signal (&memory_mutex);
      return STORM_RETURN_MEMORY_NOT_ALLOCATED;
    }

    default:
    {
      DEBUG_HALT ("Unknown return code.");
    }
  }
}
