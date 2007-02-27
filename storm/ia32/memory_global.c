/* $Id$ */
/* Abstract: Memory allocation functions for the global memory area. */
/* Authors: Henrik Hallin <hal@chaosdev.org>
            Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

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

/* If DEBUG is TRUE, lots of debug information will be printed. */

#define DEBUG FALSE

/* Define this if you want to check the data structures all the
   time. Slow, but good for debugging. */

#undef CHECK 

/* Define this if you want to enable deallocation of memory. */

#undef DEALLOCATE

#include <storm/generic/avl.h>
#include <storm/generic/avl_debug.h>
#include <storm/generic/avl_update.h>
#include <storm/generic/cpu.h>
#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/mutex.h>
#include <storm/generic/return_values.h>
#include <storm/generic/slab.h>
#include <storm/generic/thread.h>

/* Globals. */

static slab_heap_type *global_slab_heap;
avl_header_type *global_avl_header;
u32 global_memory_left;

/* Locals. */

static return_type memory_global_deallocate_page (u32 page_number) __attribute__ ((unused));
static u32 memory_global_allocate_page (u32 length);

/* Initialise the global memory heap. */

void memory_global_init (void)
{
  u32 physical_page;

  /* All memory is free initially. */

  global_memory_left = SIZE_GLOBAL_HEAP;

  /* Set up an AVL-tree for allocation. */

  global_avl_header = (avl_header_type *) BASE_GLOBAL_MEMORY_TREE;

  /* FIXME: Check return value. */

  memory_physical_allocate (&physical_page, 1, "Global AVL memory tree");

  memory_virtual_map (GET_PAGE_NUMBER (BASE_GLOBAL_MEMORY_TREE),
                      physical_page, 1, PAGE_KERNEL);

  /* FIXME: Check return value. */

  memory_physical_allocate (&physical_page, 1, "Global AVL memory tree");

  memory_virtual_map (GET_PAGE_NUMBER (BASE_GLOBAL_MEMORY_TREE) +
                      GLOBAL_MEMORY_TREE_INTRO_PAGES,
                      physical_page, 1, PAGE_KERNEL);

  /* Fill in the basic data in the tree. */

  global_avl_header->limit_nodes = limit_global_nodes;
  global_avl_header->node_array = global_avl_header->root =
    (avl_node_type *) (BASE_GLOBAL_MEMORY_TREE +
                       (GLOBAL_MEMORY_TREE_INTRO_PAGES * SIZE_PAGE));
  global_avl_header->growable = TRUE;
  global_avl_header->limit_pages_bitmap = GLOBAL_MEMORY_TREE_INTRO_PAGES;
  global_avl_header->limit_pages_array = GLOBAL_MEMORY_TREE_ARRAY_PAGES;
  global_avl_header->pages_allocated_bitmap =
    global_avl_header->pages_allocated_array = 1;
  global_avl_header->number_of_nodes = 1;

  /* Clear the bitmap, marking all slots as free. */

  memory_set_u8 ((u8 *) global_avl_header->bitmap, 0,
                 SIZE_PAGE - sizeof (avl_header_type));

  /* Initialise the tree, starting with an empty entry. */

  avl_node_reset (global_avl_header->root, GET_PAGE_NUMBER (BASE_GLOBAL_HEAP),
                  0, SIZE_IN_PAGES (SIZE_GLOBAL_HEAP), NULL);

  /* And mark that entry as used in the bitmap. */

  global_avl_header->bitmap[0] = 1;

  /* Set up the slab heap. */

  global_slab_heap =
    (slab_heap_type *) (memory_global_allocate_page (1) * SIZE_PAGE);

  /* FIXME: Check return value. */

  memory_physical_allocate (&physical_page, 1, "Global SLAB heap.");

  memory_virtual_map (GET_PAGE_NUMBER (global_slab_heap),
                      physical_page, 1, PAGE_KERNEL);
  slab_heap_init (global_slab_heap);
}

/* Allocates a page region in the global memory area. */

static u32 memory_global_allocate_page (u32 length)
{
  avl_node_type *node;
  avl_node_type *insert_node;
  unsigned int start;

#ifdef CHECK
  avl_debug_tree_check (global_avl_header, global_avl_header->root);
#endif

  /* First check for invalid parameters. */

  if (length == 0)
  {
    DEBUG_HALT ("length must be > 0!");
  }

  /* Get the root node. */

  node = global_avl_header->root;

  while (node != NULL)
  {
    if (node->largest_free_less >= length)
    {
      node = (avl_node_type *) node->less;
    }
    else if (node->free_length >= length)
    {
      /* We found a space big enough. Conquer it! */
      
      if (node->busy_length == 0)
      {
        node->busy_length = length;
        node->free_length -= length;

        avl_update_tree_largest_free (node->parent);
        
        global_memory_left -= length;
        
        start = node->start;
      }
      else
      {
        insert_node = avl_node_allocate (global_avl_header);

        avl_node_reset (insert_node, node->start + node->busy_length,
                        length, node->free_length - length, NULL);
        
        node->free_length = 0;
        avl_update_tree_largest_free (node->parent);
        avl_node_insert (global_avl_header, insert_node);
        global_memory_left -= length;
        start = node->start + node->busy_length;
      }

#ifdef CHECK
  avl_debug_tree_check (global_avl_header, global_avl_header->root);
#endif

      return start;
    }
    else if (node->largest_free_more >= length)
    {
      node = (avl_node_type *) node->more;
    }
    else
    {
      DEBUG_HALT ("Failed to allocate memory.");
    }
  }
  
  DEBUG_HALT ("No block found, but nodes said there was one. Tree broken.");
}

/* Deallocate the given range, startig at page_number. */

static return_type memory_global_deallocate_page (u32 page_number)
{
  avl_node_type *node;
  avl_node_type *adjacent_node;
  unsigned int free_length;
  bool finished = FALSE;
  
#ifdef CHECK
  avl_debug_tree_check (global_avl_header, global_avl_header->root);
#endif
  node = global_avl_header->root;

  while (!finished && node != NULL)
  {
    if (page_number > node->start)
    {
      node = node->more;
    }
    else if (page_number < node->start)
    {
      node = node->less;
    }
    else
    {
      finished = TRUE;
    }
  }

  if (node == NULL)
  {
    DEBUG_MESSAGE (DEBUG, "Area not allocated!");
    
    return RETURN_MEMORY_NOT_ALLOCATED;
  }

  free_length = node->busy_length;

  /* Alright. We have the node to delete. Find the less-adjacent node. */

  if (node->less == NULL)
  {
    adjacent_node = node;

    while (adjacent_node->parent != NULL &&
           adjacent_node->parent->less == adjacent_node)
    {
      adjacent_node = adjacent_node->parent;
    }

    if (adjacent_node != NULL)
    {
      adjacent_node = adjacent_node->parent;
    }
  }
  else
  {
    adjacent_node = node->less;

    while (adjacent_node->more != NULL)
    {
      adjacent_node = adjacent_node->more;
    }
  }

  /* We have the adjacent node or NULL if there was none. */

  if (adjacent_node == NULL)
  {
    node->free_length += node->busy_length;
    node->busy_length = 0;
    
    avl_update_tree_largest_free (node->parent);
  }
  else
  {
    adjacent_node->free_length += node->free_length + node->busy_length;

    avl_update_tree_largest_free (adjacent_node->parent);
    avl_node_delete (global_avl_header, node);
  }

  global_memory_left += free_length;

#ifdef CHECK
  avl_debug_tree_check (global_avl_header, global_avl_header->root);
#endif

  return RETURN_SUCCESS;
}

/* Get the number of pages allocated for the given data block */

static u32 memory_global_get_size (void *data)
{
  avl_node_type *node;
  bool finished = FALSE;
  u32 page_number = GET_PAGE_NUMBER ((u32) data);
  
#ifdef CHECK
  avl_debug_tree_check (global_avl_header, global_avl_header->root);
#endif
  node = global_avl_header->root;

  while (!finished && node != NULL)
  {
    if (page_number > node->start)
    {
      node = node->more;
    }
    else if (page_number < node->start)
    {
      node = node->less;
    }
    else
    {
      finished = TRUE;
    }
  }

  if (node == NULL)
  {
    DEBUG_MESSAGE (DEBUG, "Area not allocated!");
    
    return MAX_U32;
  }

  return node->busy_length;
}

/* Get the amount of global memory currently in use. */

unsigned int memory_global_get_used (void)
{
  return SIZE_GLOBAL_HEAP - global_memory_left;
}

/* Get the amount of global memory currently unallocated. */

unsigned int memory_global_get_free (void)
{
  return global_memory_left;
}

/* Allocate memory from the global heap. */

void *memory_global_allocate (unsigned int length)
{
  int index = slab_heap_index (length);
  slab_block_type *block;

  if (tss_tree_mutex != MUTEX_LOCKED && 
      memory_mutex != MUTEX_LOCKED && initialised)
  {
    DEBUG_HALT ("Code is not properly mutexed.");
  }

  //  mutex_kernel_wait (&memory_mutex);

  DEBUG_MESSAGE (DEBUG, "Called (length = %u)", length);

  /* If index now is -1, it means we tried to allocate more than 1024
     byte, and in this system, that gets rounded to the closest upper
     page boundary. */

  if (index == -1)
  {
    u32 virtual_page = memory_global_allocate_page (SIZE_IN_PAGES (length));
    u32 physical_page;

    /* FIXME: Check return value. */

    memory_physical_allocate (&physical_page, SIZE_IN_PAGES (length),
                              "Global memory data structure");

    /* So, map this memory, and return a pointer to it. */

    memory_virtual_map (virtual_page, physical_page, SIZE_IN_PAGES (length),
                        PAGE_KERNEL);

    DEBUG_MESSAGE (DEBUG, "Leaving through path 1");
    global_memory_left -= PAGE_ALIGN (length);
    //    mutex_kernel_signal (&memory_mutex);
    return (void *) (virtual_page * SIZE_PAGE);
  }
  else
  {
    slab_superblock_type *superblock;
    slab_block_type *next;

    /* Now, we know which entry in the slab heap to use. Check if
       there is already a slab superblock we can use. Otherwise, we
       will have to create one. */

    superblock = global_slab_heap->block[index];

    /* Walk the list of slab superblocks with free blocks. */

    while (superblock != NULL &&
           superblock->header.free_blocks == 0)
    {
      DEBUG_MESSAGE (DEBUG, "superblock = %p", superblock);
      superblock = superblock->header.next_superblock;
    }
    
    /* No, we were out of luck. */

    if (superblock == NULL)
    {
      u32 virtual_page = memory_global_allocate_page (SIZE_IN_PAGES (length));
      u32 physical_page;

      /* FIXME: Check return value. */

      memory_physical_allocate (&physical_page, SIZE_IN_PAGES (length),
                                "Global memory data structure");

      superblock = (slab_superblock_type *) (virtual_page * SIZE_PAGE);
      memory_virtual_map (virtual_page, physical_page, 1, PAGE_KERNEL);

      /* Initialise this newly created slab superblock. */

      slab_superblock_init (superblock, global_slab_heap, index);
      global_slab_heap->block[index] = superblock;
    }
    
    /* Now, we have our superblock. Get the block we want, and update
       the structures. */

    block = superblock->header.free_list;

    if (block == NULL)
    {
      DEBUG_HALT ("The list of free blocks of size %u (real size %u) was NULL.",
                  slab_block_size[index], length);
    }

    next = block->next;

    if (next != NULL)
    {
      next->previous = NULL;
    }
    
    superblock->header.free_list = next;
    superblock->header.free_blocks--;

    DEBUG_MESSAGE (DEBUG, "Leaving through path 2. Block allocated: %x",
                   block);
    //    debug_print ("%p %u ", block, slab_block_size[index]);

    global_memory_left -= slab_block_size[index];
    //    mutex_kernel_signal (&memory_mutex);
    return block;
  }
}

/* Deallocate memory from the global heap. */

return_type memory_global_deallocate (void *data)
{
  slab_superblock_type *superblock = 
    (slab_superblock_type *) ((u32) data & 0xFFFFF000);
  slab_block_type *block = (slab_block_type *) data;
  page_table_entry *page_table;
  int index = slab_heap_index (superblock->header.buffer_size);

  /* FIXME: Make sure that all code that uses this function has locked
     the tss_tree_mutex. */

  //  mutex_kernel_wait (&memory_mutex);

  DEBUG_MESSAGE (DEBUG, "Called (data = %p)", data);

  /* If the data address is page aligned, it is not a slab block, so
     handle it specially. */

  if (data == superblock)
  {
    u32 pages = memory_global_get_size (data);
    u32 physical_page;
    
    if (pages == MAX_U32)
    {
      DEBUG_MESSAGE (DEBUG, "Leaving through path 0");
      return RETURN_MEMORY_NOT_ALLOCATED;
    }

#ifdef DEALLOCATE    
    memory_global_deallocate_page (GET_PAGE_NUMBER ((u32) data));
#endif

    /* Find the physical adress for this virtual address. */
    
    page_table = (page_table_entry *) (BASE_PROCESS_PAGE_TABLES +
                                     ((u32) data) / (4 * MB));
    physical_page = page_table[GET_PAGE_NUMBER ((u32) data) % 1024].page_base;

#ifdef DEALLOCATE    
    memory_physical_deallocate (physical_page);
#endif

    /* FIXME: Unmap this memory, too. Fairly trivial, but we must take
       care of regions spanning several page tables... */
    
    //    mutex_kernel_signal (&memory_mutex);

    DEBUG_MESSAGE (DEBUG, "Leaving through path 1");
    return RETURN_SUCCESS;
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

      slab_superblock_type *new_superblock = global_slab_heap->block[index];
      
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
    
    global_memory_left += slab_block_size[index];
    //    mutex_kernel_signal (&memory_mutex);
    DEBUG_MESSAGE (DEBUG, "Leaving through path 2");

    return RETURN_SUCCESS;
  }
}
