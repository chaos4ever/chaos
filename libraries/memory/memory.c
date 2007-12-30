/* $Id$ */
/* Abstract: Memory library. */
/* Authors: Anders Ohrt <doa@chaosdev.org> 
            Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

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

#include <memory/memory.h>
#include <mutex/mutex.h>
#include <system/system.h>

/* The memory_structure must be defined by the process if it wants to
   link to this library. This is done by our default stub. (startup.c) */

extern memory_structure_type memory_structure;
extern mutex_type memory_mutex;

/* FIXME: This is a little ugly... Any better ideas? Having an extra
   parameter to memory_allocate () would be totally gross. */

bool initialised = FALSE;

/* Initialise the slab system. */

void memory_init (void)
{
  slab_heap_type **heap = &memory_structure.slab_heap;
  system_call_memory_allocate ((void **) heap, 1,
                               TRUE);
  memory_structure.locked = FALSE;
  slab_heap_init (memory_structure.slab_heap);
  initialised = TRUE;
}

/* Allocate memory. */

return_type memory_allocate (void **pointer, unsigned int bytes)
{
  int index = slab_heap_index (bytes);
  slab_block_type *block;

  if (!initialised)
  {
    memory_init ();
  }

  while (memory_structure.locked)
  {
    system_sleep (100);
  }

  memory_structure.locked = TRUE;

  /* If index now is -1, it means we tried to allocate more than 1024
     byte, and in this system, that gets rounded to the closest upper
     page boundary. */

  if (index == -1)
  {
    /* FIXME: Check return value. */

    system_call_memory_allocate (pointer, SIZE_IN_PAGES (bytes), TRUE);
    memory_structure.locked = FALSE;

    return MEMORY_RETURN_SUCCESS;
  }
  else
  {
    slab_superblock_type *superblock;
    slab_block_type *next;

    /* Now, we know which entry in the slab heap to use. Check if
       there is already a slab superblock we can use. Otherwise, we
       will have to create one. */

    superblock = memory_structure.slab_heap->block[index];

    /* Walk the list of slab superblocks with free blocks. */

    while (superblock != NULL &&
           superblock->header.free_blocks == 0)
    {
      superblock = superblock->header.next_superblock;
    }
    
    /* No, we were out of luck. */

    if (superblock == NULL)
    {
      /* FIXME: Check return value. */

      slab_superblock_type **superblock_pointer = &superblock;
      system_call_memory_allocate ((void **) superblock_pointer, 1, TRUE);

      /* Initialise this newly created slab superblock. */

      slab_superblock_init (superblock, memory_structure.slab_heap, index);
      memory_structure.slab_heap->block[index] = superblock;
    }

    /* Make sure the magic cookie has the right value. */

    if (superblock->header.magic_number != SLAB_MAGIC_COOKIE)
    {
      memory_structure.locked = FALSE;
      return MEMORY_RETURN_SLAB_BROKEN;
    }
    
    /* Now, we have our superblock. Get the block we want, and update
       the structures. */

    block = superblock->header.free_list;
    next = block->next;

    if (next != NULL)
    {
      next->previous = NULL;
    }
    
    superblock->header.free_list = next;
    superblock->header.free_blocks--;

    *pointer = block;
  }

  memory_structure.locked = FALSE;

  return MEMORY_RETURN_SUCCESS;
}

/* Deallocate a previously allocated memory buffer. */

return_type memory_deallocate (void **pointer)
{
  slab_superblock_type *superblock = 
    (slab_superblock_type *) ((u32) *pointer & 0xFFFFF000);
  slab_block_type *block = (slab_block_type *) *pointer;
  int index = slab_heap_index (superblock->header.buffer_size);

  if (!initialised)
  {
    memory_init ();
  }

  while (memory_structure.locked)
  {
    system_sleep (100);
  }

  memory_structure.locked = TRUE;

  /* If the data address is page aligned, it is not a slab block, so
     handle it specially. */

  if (*pointer == superblock)
  {
    // FIXME: I'm not 100% sure I got this one right... The original code was:
    // system_call_memory_deallocate ((void **) &pointer)
    // but that's not OK now with the new rules for strict aliasing in gcc
    // 3 and 4.
    void *double_pointer = &pointer;
    system_call_memory_deallocate ((void **) double_pointer);
    memory_structure.locked = FALSE;

    return MEMORY_RETURN_SUCCESS;
  }
  else
  {
    slab_block_type *free_list;

    /* Make sure our magic cookie has the correct value. */

    if (superblock->header.magic_number != SLAB_MAGIC_COOKIE)
    {
      memory_structure.locked = FALSE;
      return MEMORY_RETURN_SLAB_BROKEN;
    }

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

      slab_superblock_type *new_superblock = 
        memory_structure.slab_heap->block[index];
      
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

    memory_structure.locked = FALSE;
    *pointer = NULL;

    return MEMORY_RETURN_SUCCESS;
  }
}
