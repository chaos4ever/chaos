/* $Id$ */
/* Abstract: Slab memory allocator. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#ifndef __LIBRARY_MEMORY_SLAB_H__
#define __LIBRARY_MEMORY_SLAB_H__

#include <system/system.h>

/* Defines. */

#define SLAB_MAGIC_COOKIE       0xC0CAC01A

/* Type definitions. */

typedef struct
{
  void *block[0];
} slab_heap_type;

typedef struct
{
  u32 magic_number;
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

/* Function prototypes. */

extern bool slab_heap_init (slab_heap_type *heap);
extern bool slab_superblock_init (slab_superblock_type *superblock,
                                  slab_heap_type *heap,
                                  int index);
extern int slab_heap_index (unsigned int buffer_size);

#endif /* !__LIBRARY_MEMORY_SLAB_H__ */
