// Abstract: Function prototypes and structure definitions for the physical memory allocation system.
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 1999 chaos development

#pragma once

#include <storm/generic/avl.h>
#include <storm/generic/types.h>

extern void memory_physical_init(void) INIT_CODE;

extern return_type memory_physical_reserve(unsigned int start, unsigned int length, const char *description);
extern return_type memory_physical_allocate(uint32_t *page, unsigned int length, const char *description) WEAK;
extern return_type memory_physical_deallocate(unsigned int start);
extern uint32_t memory_physical_get_number_of_pages(void);
extern uint32_t memory_physical_get_free(void);
extern uint32_t memory_physical_get_used(void);

extern avl_header_type *page_avl_header;
extern uint32_t page_avl_pages;
