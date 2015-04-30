// Abstract: Function prototypes and structure definitions for the physical memory allocation system.
// Author: Per Lundberg <per@halleluja.nu>

// © Copyright 1999-2000 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015 chaos development

#pragma once

#include <storm/generic/avl.h>
#include <storm/generic/types.h>

extern void memory_physical_init(void) INIT_CODE;

extern return_type memory_physical_reserve(unsigned int start, unsigned int length);
extern return_type memory_physical_allocate(u32 *page, unsigned int length, char *description)  __attribute__ ((weak));
extern return_type memory_physical_deallocate(unsigned int start);
extern u32 memory_physical_get_number_of_pages(void);
extern u32 memory_physical_get_free(void);
extern u32 memory_physical_get_used(void);

extern avl_header_type *page_avl_header;
extern u32 page_avl_pages;
