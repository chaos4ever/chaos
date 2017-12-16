// Abstract: Function prototypes and structures used by the virtual memory routines.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development.

#pragma once

#include <storm/generic/avl.h>
#include <storm/generic/bit.h>
#include <storm/generic/cpu.h>
#include <storm/generic/types.h>
#include <storm/current-arch/tss.h>

#include <storm/current-arch/memory_virtual.h>

extern void memory_virtual_init(void) INIT_CODE;
extern void memory_virtual_enable(void) INIT_CODE;

extern return_type memory_virtual_map_paging_disabled(page_directory_entry_page_table *page_directory,
                                                      uint32_t virtual_page, uint32_t physical_page, uint32_t pages,
                                                      uint32_t flags) INIT_CODE;
extern return_type memory_virtual_map_other(storm_tss_type *tss, uint32_t virtual_page, uint32_t physical_page,
                                            uint32_t pages, uint32_t flags);
extern return_type memory_virtual_map(uint32_t virtual_page, uint32_t physical_page, uint32_t pages, uint32_t flags) WEAK;
extern void memory_virtual_unmap(uint32_t virtual_page, uint32_t pages);
extern return_type map(process_id_type pid, uint32_t linear_page, uint32_t physical_page, uint32_t pages);

extern page_directory_entry_page_table *memory_virtual_clone_page_directory(page_directory_entry_page_table *source);

extern void memory_virtual_dump_map(page_directory_entry_page_table *page_directory);

extern return_type memory_virtual_allocate(uint32_t *page_number, uint32_t pages);
extern return_type memory_virtual_deallocate(uint32_t page_number);
extern return_type memory_virtual_reserve(unsigned int start_page, unsigned int pages);
extern void memory_virtual_create_page_tables_mapping(page_directory_entry_page_table *process_page_directory,
                                                      uint32_t page_directory_page);

extern avl_header_type *process_avl_header;
extern page_directory_entry_page_table *kernel_page_directory;
extern page_directory_entry_page_table *process_page_directory;
extern page_table_entry *shared_page_tables;
