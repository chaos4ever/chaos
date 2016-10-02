// Abstract: Slab memory allocator.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000, 2013 chaos development.

#pragma once

#include <system/system.h>

#define SLAB_MAGIC_COOKIE       0xC0CAC01A

typedef struct
{
    void *block[0];
} slab_heap_type;

typedef struct
{
    uint32_t magic_number;
    unsigned int buffer_size;
    void *previous_superblock;
    void *next_superblock;
    void *free_list;
    unsigned int free_blocks;
    unsigned int total_blocks;
} slab_superblock_header_type;

typedef struct
{
    void *previous;
    void *next;
} slab_block_type;

typedef struct
{
    slab_superblock_header_type header;
    uint8_t buffer[0];
} slab_superblock_type;

extern const int slab_block_size[];

extern bool slab_heap_init(slab_heap_type *heap);
extern bool slab_superblock_init(slab_superblock_type *superblock, slab_heap_type *heap, int index);
extern int slab_heap_index(unsigned int buffer_size);
