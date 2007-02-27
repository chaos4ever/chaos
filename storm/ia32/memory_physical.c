/* $Id$ */
/* Abstract: Memory allocation functions on page level. */
/* Authors: Henrik Hallin <hal@chaosdev.org>,
            Anders Öhrt <doa@chaosdev.org>,
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

/* This is the low level page allocation system. It is based on the
   hifi-eslöf algorithms. On level zero, we have a bitmap which tells
   us which entries in the level one array is used. On level one, we
   have the hifi-eslöf which contains all the memory blocks. A memory
   block describes one or more occupied pages and a set of free
   pages. */

/* Define if you are debugging this module. */

#define DEBUG   FALSE

/* If CHECK is defined, all operations will perform checks on the
   structure of all of the AVL tree, all the time. This is really
   time-consuming, because of the way the algorithm works, so it is
   only to be used when you suspect something is trashing your AVL
   tree. */

#undef CHECK

#include <storm/generic/avl.h>
#include <storm/generic/avl_debug.h>
#include <storm/generic/avl_update.h>
#include <storm/generic/cpu.h>
#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/limits.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/multiboot.h>
#include <storm/generic/return_values.h>
#include <storm/generic/string.h>
#include <storm/generic/thread.h>
#include <storm/generic/types.h>

u32 page_avl_base = BASE_PAGE_AVL;
avl_header_type *page_avl_header;
u32 pages_left, physical_pages;
u32 page_avl_pages;

/* Initialise the page allocation system. */

void memory_physical_init (void)
{
  unsigned int page_avl_intro_pages, page_avl_array_pages, counter;
    
  /* Calculate the number of physical pages. */

  physical_pages = (1024 + multiboot_info.memory_upper) / 4;

  /* FIXME: Align to a 32-page boundary, as long as GRUB reports
     strange values. How do we handle this properly? */

  physical_pages &= ~31;

  pages_left = physical_pages;

  page_avl_intro_pages = SIZE_IN_PAGES (sizeof (avl_header_type) +
                                        (physical_pages / 8));
  page_avl_array_pages = SIZE_IN_PAGES (sizeof (avl_node_type) *
                                        physical_pages);
  page_avl_pages = page_avl_intro_pages + page_avl_array_pages;
  
  /* Put the AVL header where it should be. */

  page_avl_header = (avl_header_type *) (page_avl_base);

  /* Fill in the values in the AVL header. */

  page_avl_header->limit_nodes = physical_pages;
  page_avl_header->number_of_nodes = 1;
  page_avl_header->limit_pages_bitmap = page_avl_intro_pages;
  page_avl_header->limit_pages_array = page_avl_array_pages;
  page_avl_header->root = page_avl_header->node_array =
    (avl_node_type *) (page_avl_base + page_avl_intro_pages * SIZE_PAGE);
  page_avl_header->growable = FALSE;

  /* For now, all of the data is mapped. */

  page_avl_header->pages_allocated_bitmap = page_avl_intro_pages;
  page_avl_header->pages_allocated_array = page_avl_array_pages;

  /* Clear the bitmap, marking all slots as free. */

  memory_set_u8 ((u8 *) page_avl_header->bitmap, 0, physical_pages / 8);

  /* Initialise the tree. All memory is free. */
 
  avl_node_reset (page_avl_header->root, 0, 0, physical_pages, NULL);

  /* And mark that entry as used in the bitmap. */

  page_avl_header->bitmap[0] = 1;
   
  /* Now, let's do it like they do on the discovery channel. Mark the
     reserved memory as reserved. */

  memory_physical_reserve (GET_PAGE_NUMBER (BASE_GDT),
                           SIZE_IN_PAGES (SIZE_GDT_IDT));

  memory_physical_reserve (GET_PAGE_NUMBER (BASE_KERNEL_TSS),
                           SIZE_IN_PAGES (SIZE_KERNEL_TSS));

  memory_physical_reserve (GET_PAGE_NUMBER (BASE_KERNEL_STACK),
                           SIZE_IN_PAGES (SIZE_KERNEL_STACK));

  memory_physical_reserve (GET_PAGE_NUMBER (BASE_MODULE_NAME), 
                           SIZE_IN_PAGES (SIZE_MODULE_NAME));
  
  /* Reserve the memory for the kernel. */
  
  memory_physical_reserve (GET_PAGE_NUMBER (BASE_KERNEL),
                           SIZE_IN_PAGES ((u32) &_end - BASE_KERNEL));

  /* If we have more than 16 megs of RAM, we need to allocate DMA
     buffers to avoid lots of trouble later. FIXME: Should be
     configurable via command line switch too. */

  DEBUG_MESSAGE (DEBUG, "Reserving memory for DMA...");
  
  /* FIXME: For now, we have to do this unconditionally. */
  /*  if (physical_pages * SIZE_PAGE > 16 * MB) */
  {
    memory_physical_reserve (GET_PAGE_NUMBER (BASE_DMA),
                             SIZE_IN_PAGES (SIZE_DMA));
  }
  
  /* The memory between 640k and 1024k are 'reserved' for various ISA
     plug in cards and other junk. Nevertheless, if we mess with it,
     it may mess with us, so we'd better not. */

  DEBUG_MESSAGE (DEBUG, "Reserving high memory...");

  memory_physical_reserve (GET_PAGE_NUMBER (BASE_UPPER),
                           SIZE_IN_PAGES (SIZE_UPPER));

  /* Reserve the memory used by the AVL system. */

  DEBUG_MESSAGE (DEBUG, "Reserving physical memory AVL-data...");

  memory_physical_reserve (GET_PAGE_NUMBER ((u32) page_avl_header),
                           page_avl_pages);
  
  /* And of course the server images too. */

  DEBUG_MESSAGE (DEBUG, "Reserving memory for servers...");

  for (counter = 0; counter < multiboot_info.number_of_modules; counter++)
  {
    memory_physical_reserve 
      (GET_PAGE_NUMBER (multiboot_module_info[counter].start),
       SIZE_IN_PAGES (multiboot_module_info[counter].end -
                      multiboot_module_info[counter].start));
  } 
}

/* Reserve a region, so it won't get allocated later. */

return_type memory_physical_reserve (unsigned int start,
                                     unsigned int length)
{
  avl_node_type *node = page_avl_header->root;
  avl_node_type *insert_node;

  DEBUG_MESSAGE (DEBUG, "start = %u, length = %u", start, length);

#ifdef CHECK
  avl_debug_tree_check (page_avl_header, page_avl_header->root);
#endif

  /* Main loop. Continue until the end of the tree, or until we find a
     match. */

  while (node != NULL)
  {
    /* Is the page we're looking for on the left side? */

    if (start < node->start)
    {
      node = (avl_node_type *) node->less;
    }

    /* Is it on the right side? */
    
    else if (start >=
             (node->start + node->busy_length + node->free_length))
    {
      node = (avl_node_type *) node->more;
    }

    /* Is it in this entry? */

    else if ((start >= node->start + node->busy_length) &&
             ((start + length) <=
              (node->start + node->busy_length + node->free_length)))
    {
      insert_node = avl_node_allocate (page_avl_header);
      avl_node_reset (insert_node, start, length,
                      node->start + node->busy_length +
                      node->free_length - start - length, NULL);

      node->free_length = start - node->start - node->busy_length;

      //    debug_print ("ulltass2000, %u\n", __LINE__);
      avl_update_tree_largest_free (node->parent);

      avl_node_insert (page_avl_header, insert_node);

      pages_left -= length;

#ifdef CHECK
      avl_debug_tree_check (page_avl_header, page_avl_header->root);
#endif

      return RETURN_SUCCESS;
    }

    /* I smell something rotten in the land of England... */

    else
    {
      DEBUG_HALT ("You tried to reserve something that was already taken.");
      return RETURN_PAGE_NOT_FOUND;
    }
  }
  
  DEBUG_HALT ("Didn't find a matching page range. This shouldn't ever happen.");
  return RETURN_PAGE_NOT_FOUND;
}

/* Allocate some pages. */

return_type memory_physical_allocate (u32 *page, unsigned int length, 
                                      char *description __attribute__ ((unused)))
{
  avl_node_type *node = page_avl_header->root;
  avl_node_type *insert_node;

  if (tss_tree_mutex != MUTEX_LOCKED && 
      memory_mutex != MUTEX_LOCKED && initialised)
  {
    DEBUG_HALT ("Code is not properly mutexed.");
  }

  //debug_print ("Called for: %s\n", description);

#ifdef CHECK
  avl_debug_tree_check (page_avl_header, page_avl_header->root);
#endif

  /* Check if there is enough free memory. */

  if (length > pages_left)
  {
    DEBUG_HALT ("Not enough free physical pages left.");
  }

  /* First check for invalid parameters. */

  if (length == 0)
  {
    DEBUG_HALT ("Pages must be > 0!");
  }

  /* FIXME: Best choice? Take the biggest/lowest block? In this case,
     it really doesn't matter too much, but memory_virtual_allocate
     should always take the lowest available block to reduce memory
     usage. */

  while (node != NULL)
  {
    if (node->largest_free_less >= length)
    {
      DEBUG_MESSAGE (DEBUG, "less");
      node = (avl_node_type *) node->less;
    }
    else if (node->free_length >= length)
    {
      DEBUG_MESSAGE (DEBUG, "found");

      if (node->busy_length == 0)
      {
        node->busy_length = length;
        node->free_length -= length;

        DEBUG_MESSAGE (DEBUG, "Special case!\n");

        //        debug_print ("ulltass2000, %u", __LINE__);
        avl_update_tree_largest_free (node->parent);

        pages_left -= length;

        DEBUG_MESSAGE (DEBUG, "Exiting");

#ifdef CHECK
        avl_debug_tree_check (page_avl_header, page_avl_header->root);
#endif
        *page = node->start;
        return STORM_RETURN_SUCCESS;
      }
      else
      {
        insert_node = avl_node_allocate (page_avl_header);
        avl_node_reset (insert_node, node->start + node->busy_length,
                        length, node->free_length - length, NULL);
        //        debug_print ("evald = %u\n", node->start + node->busy_length);
        
        node->free_length = 0;
        
        //    debug_print ("ulltass2000, %u", __LINE__);
        avl_update_tree_largest_free (node->parent);
        
        avl_node_insert (page_avl_header, insert_node);
        
        pages_left -= length;

        DEBUG_MESSAGE (DEBUG, "Exiting");

#ifdef CHECK
        avl_debug_tree_check (page_avl_header, page_avl_header->root);
#endif
        *page = node->start + node->busy_length;
        return STORM_RETURN_SUCCESS;
      }
    }
    else if (node->largest_free_more >= length)
    {
      DEBUG_MESSAGE (DEBUG, "more");
      node = (avl_node_type *) node->more;
    }
    else
    {
      debug_print ("%u/%u pages allocated.\n", physical_pages - pages_left,
                   physical_pages);
      DEBUG_HALT ("Failed to allocate a page.");
    }
  }

  DEBUG_HALT ("No block found, but nodes said there was one. Tree broken.");
}

/* Free a previously reserved or allocated region. */

return_type memory_physical_deallocate (unsigned int start)
{
  avl_node_type *node = page_avl_header->root;
  avl_node_type *adjacent_node;
  unsigned int free_length;
  bool finished = FALSE;
  
#ifdef CHECK
  avl_debug_tree_check (page_avl_header, page_avl_header->root);
#endif

  while (!finished && node != NULL)
  {
    if (start > node->start)
    {
      node = node->more;
    }
    else if (start < node->start)
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
    
    //    debug_print ("ulltass2000, %u", __LINE__);
    avl_update_tree_largest_free (node->parent);
  }
  else
  {
    adjacent_node->free_length += node->free_length + node->busy_length;

    //    debug_print ("ulltass2000, %u", __LINE__);
    avl_update_tree_largest_free (adjacent_node->parent);

    avl_node_delete (page_avl_header, node);
  }

  pages_left += free_length;

#ifdef CHECK
  avl_debug_tree_check (page_avl_header, page_avl_header->root);
#endif

  return RETURN_SUCCESS;
}

/* Get the number of total physical pages. */

u32 memory_physical_get_number_of_pages (void)
{
  return physical_pages;
}

/* Get the number of free physical pages. */

u32 memory_physical_get_free (void)
{
  return pages_left;
}

/* Get the number of used physical pages. */

u32 memory_physical_get_used (void)
{
  return (physical_pages - pages_left);
}
