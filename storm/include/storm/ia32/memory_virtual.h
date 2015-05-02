// Abstract: Function prototypes and structures used by the virtual memory routines.
// Authors: Per Lundberg <per@halleluja.nu>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015 chaos development

#pragma once

#include <storm/generic/types.h>
#include <storm/current-arch/cpuid.h>

// Supervisor-only, read-only (since we don't set WP bit, it isn't really read-only for the kernel..).
#define PAGE_KERNEL             0

#define PAGE_WRITABLE           BIT_VALUE(0)
#define PAGE_NON_PRIVILEGED     BIT_VALUE(1)
#define PAGE_WRITE_THROUGH      BIT_VALUE(2)
#define PAGE_CACHE_DISABLE      BIT_VALUE(3)
#define PAGE_GLOBAL             BIT_VALUE(4)

typedef struct
{
    u32 present           : 1;
    u32 flags             : 4;
    u32 accessed          : 1;
    u32 dirty             : 1;

    // Should always be one.
    u32 page_size         : 1;
    u32 global            : 1;
    u32 available         : 3;

    // Obvious?
    u32 zero              : 10;
    u32 page_base         : 10;
} page_directory_entry_4mbpage;

typedef struct
{
    u32 present           : 1;
    u32 flags             : 4;
    u32 accessed          : 1;
    u32 zero              : 1;

    // Should always be zero.
    u32 page_size         : 1;
    u32 global            : 1;
    u32 available         : 3;
    u32 page_table_base    : 20;
} page_directory_entry_page_table;

typedef struct
{
    u32 present           : 1;
    u32 flags             : 4;
    u32 accessed          : 1;
    u32 dirty             : 1;
    u32 zero              : 1;
    u32 global            : 1;
    u32 available         : 3;
    u32 page_base         : 20;
} page_table_entry;

static inline void memory_virtual_cache_invalidate(void *address)
{
    // If we're 486 or better, invalidate the cache. If not, we don't have a cache and thus we don't have to invalidate it.
    if (cpu_info.family >= 4)
    {
        asm ("invlpg %0"
             :
             : "m" (*(u8 *) address)
             : "memory");
    }
}
