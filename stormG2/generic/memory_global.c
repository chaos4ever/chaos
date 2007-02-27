/* $Id$ */
/* Abstract: Global memory allocator. */

/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

/* This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/slab.h>
#include <storm/generic/memory_physical.h>

static slab_heap_type *slab_heap;

/* Initialise the global memory slab system. */

void memory_global_init (void)
{
  debug_print ("Setting up the global memory allocation...\n");

  slab_heap = memory_physical_allocate ();

  if (slab_heap == NULL)
  {
    DEBUG_HALT ("Out of memory!");
  }

  slab_heap_init (slab_heap);
}

/* Allocate a memory block. Returns NULL on error. */

void *memory_global_allocate (unsigned int size)
{
  int index = slab_heap_index (size);
  slab_block_type *block;

  if (size > SIZE_PAGE)
  {
    DEBUG_HALT ("Tried to allocate a global memory block bigger than a page");
  }
  
  /* If index now is -1, it means we tried to allocate more than 1024
     byte, and in this system, that gets rounded up to a page. */

  if (index == -1)
  {
    return memory_physical_allocate ();
  }
  else
  {
    slab_superblock_type *superblock;
    slab_block_type *next;

    /* Now, we know which entry in the slab heap to use. Check if
       there is already a slab superblock we can use. Otherwise, we
       will have to create one. */

    superblock = slab_heap->block[index];

    /* Walk the list of slab superblocks with free blocks. */

    while (superblock != NULL &&
           superblock->header.free_blocks == 0)
    {
      superblock = superblock->header.next_superblock;
    }
    
    /* No, we were out of luck. */

    if (superblock == NULL)
    {
      superblock = (slab_superblock_type *) memory_physical_allocate ();

      /* Initialise this newly created slab superblock. */

      slab_superblock_init (superblock, slab_heap, index);
      slab_heap->block[index] = superblock;
    }
    
    /* Now, we have our superblock. Get the block we want, and update
       the structures. */

    block = superblock->header.free_list;

    if (block == NULL)
    {
      DEBUG_HALT ("The list of free blocks of size %u (real size %u) was NULL.",
                  slab_block_size[index], size);
    }

    next = block->next;

    if (next != NULL)
    {
      next->previous = NULL;
    }
    
    superblock->header.free_list = next;
    superblock->header.free_blocks--;

    return block;
  }
}

/* Deallocate a memory block. */

void memory_global_deallocate (void *data)
{
  slab_superblock_type *superblock = 
    (slab_superblock_type *) ((u32) data & 0xFFFFF000);
  slab_block_type *block = (slab_block_type *) data;
  int index = slab_heap_index (superblock->header.buffer_size);

  /* If the data address is page aligned, it is not a slab block, so
     handle it specially. */

  if (data == superblock)
  {
    memory_physical_deallocate (data);
    return;
  }
  else
  {
    slab_block_type *free_list;

    /* Add this block to the free list. */
    
    free_list = superblock->header.free_list;
    block->previous = NULL;
    block->next = free_list;

    if (free_list != NULL)
    {
      free_list->previous = block;
    }

    superblock->header.free_list = block;
    superblock->header.free_blocks++;

    if (superblock->header.free_blocks == 1)
    {
      /* This superblock was all-allocated, so add it into the
         heap. We put it last, since that's probably the most
         optimised thing to do. */

      slab_superblock_type *new_superblock = slab_heap->block[index];
      
      while (new_superblock->header.next_superblock != NULL)
      {
        new_superblock = new_superblock->header.next_superblock;
      }

      new_superblock->header.next_superblock = superblock;
      superblock->header.previous_superblock = new_superblock;
      
      superblock->header.next_superblock = NULL;
    }

    /* FIXME: This code will make things a little more optimised, but
       it is not yet finished. */

#if FALSE
    else if (superblock->header.free_blocks ==
             superblock->header.total_blocks)
    {
      /* Move this superblock from its current location to the end of
         the heap. We want it in the end, so for this, we'll have to
         traverse the list some... */

      slab_superblock_type *new_superblock;

      new_superblock = global_slab_heap->block[index]; 
      while (new_superblock->header.next_superblock != NULL)
      {
        new_superblock = new_superblock->header.next_superblock;
      }

      new_superblock->header.next_superblock = superblock;
      superblock->header.previous_superblock = new_superblock;

      superblock_header.next_superblock = NULL;
    }
#endif
    
    return;
  }
}
