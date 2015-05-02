// Abstract: Function prototypes and structures used by the virtual memory routines.
// Author: Per Lundberg <per@halleluja.nu>

// © Copyright 1999-2000, 2013 chaos development.

#pragma once

#include <storm/generic/avl.h>
#include <storm/generic/bit.h>
#include <storm/generic/cpu.h>
#include <storm/generic/types.h>
#include <storm/current-arch/tss.h>

#include <storm/current-arch/memory_virtual.h>

extern void memory_virtual_init(void) INIT_CODE;
extern void memory_virtual_enable(void) INIT_CODE;

extern return_type memory_virtual_map_kernel(page_directory_entry_page_table *page_directory, u32 virtual_page, u32 physical_page,
                                             u32 pages, u32 flags) INIT_CODE;
extern return_type memory_virtual_map_other(storm_tss_type *tss, u32 virtual_page, u32 physical_page, u32 pages, u32 flags);
extern return_type memory_virtual_map(u32 virtual_page, u32 physical_page, u32 pages, u32 flags) WEAK;
extern void memory_virtual_unmap(u32 virtual_page, u32 pages);
extern return_type map(process_id_type pid, u32 linear_page, u32 physical_page, u32 pages);

extern page_directory_entry_page_table *memory_virtual_clone_page_directory(page_directory_entry_page_table *source);

extern void memory_virtual_dump_map(page_directory_entry_page_table *page_directory);

extern return_type memory_virtual_allocate(u32 *page_number, u32 pages);
extern return_type memory_virtual_deallocate(u32 page_number);
extern return_type memory_virtual_reserve(unsigned int start_page, unsigned int pages);

extern avl_header_type *process_avl_header;
extern page_directory_entry_page_table *kernel_page_directory;
extern page_directory_entry_page_table *process_page_directory;
extern page_table_entry *shared_page_tables;
