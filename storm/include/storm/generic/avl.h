/* $Id$ */
/* Abstract: AVL function prototypes. */
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
   USA */

#ifndef __STORM_GENERIC_AVL_H__
#define __STORM_GENERIC_AVL_H__

#include <storm/ia32/types.h>

/* Type definitions. */

/* Information about a memory block. */

typedef struct avl_node_type
{
  /* Pointer to left child. */

  struct avl_node_type *less;

  /* Pointer to right child. */

  struct avl_node_type *more;

  /* Pointer to parent. */

  struct avl_node_type *parent;

  /* Which of the child-trees is the highest? */
  
  int balance;

  /* The following is what makes the hifi-eslöf system special. */
  /* Which is the start page of the block? */

  unsigned int start;

  /* How much busy and free space in the block? */

  unsigned int busy_length;
  unsigned int free_length;

  /* These two make the search for a free block very fast. */

  unsigned int largest_free_less;
  unsigned int largest_free_more;
} __attribute__ ((packed)) avl_node_type;

/* AVL header type. Used by all AVL systems. */

typedef struct
{
  /* The maximum number of nodes in this AVL tree. */
  
  unsigned int limit_nodes;

  /* The pointer to the AVL node array. */

  avl_node_type *node_array;

  /* The pointer to the root of the tree. This is *not* the same as
     the previous pointer. They can, and will, differ as soon as the
     tree is rotated. */

  avl_node_type *root;

  /* Is the structures allowed to grow? This is used when the tree is growing
     to avoid recuriveness. */

  bool growable;

  /* How many pages is the two parts at a maximum? */
  
  unsigned int limit_pages_bitmap;
  unsigned int limit_pages_array;

  /* How many pages have we allocated for the nodes. */

  unsigned int pages_allocated_array;
  unsigned int pages_allocated_bitmap;

  /* How many nodes in tree? */
  /* FIXME: Use this in all allocate functions to imediately check if
     memory can be allocated. */

  unsigned int number_of_nodes;

  /* This is a bitmap, indicating which nodes in the array are used. */
  
  u32 bitmap[0];
} avl_header_type;

/* Function prototypes. */

extern void avl_node_reset (avl_node_type *node,
                            unsigned int start,
                            unsigned int busy_length,
                            unsigned int free_length,
                            avl_node_type *parent);

extern avl_node_type *avl_node_allocate (avl_header_type *avl_header);

extern void avl_tree_move (avl_header_type *avl_header,
                           unsigned int delta);

extern void avl_node_insert (avl_header_type *avl_header,
                             avl_node_type *insert_node);

extern void avl_node_delete (avl_header_type *avl_header,
                             avl_node_type *node);

extern void avl_node_split (avl_header_type *avl_header,
                            avl_node_type *node,
                            unsigned int start,
                            unsigned int length);

extern bool avl_tree_check (avl_header_type *avl_header);

#endif /* !__STORM_GENERIC_AVL_H__ */
