// Abstract: Slab structures.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2000, 2013 chaos development.

#pragma once

#include <storm/types.h>

// Type definitions.
typedef struct
{
  void *block[0];
} slab_heap_type;

typedef struct
{
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
  u8 buffer[0];
} slab_superblock_type;

extern const int slab_block_size[];

// Function prototypes.
extern bool slab_heap_init (slab_heap_type *heap);
extern bool slab_superblock_init (slab_superblock_type *superblock,
                                  slab_heap_type *heap,
                                  int index);
extern int slab_heap_index (unsigned int buffer_size);
