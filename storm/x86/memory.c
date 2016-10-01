// Abstract: Architecture-specific memory stuff.
// Author: Per Lundberg <per@chaosdev.io>

// Copyright 2000, 2013 chaos development.

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
