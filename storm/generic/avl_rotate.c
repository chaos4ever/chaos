/* $Id$ */
/* Abstract: AVL rotation functions. */
/* Author: Henrik Hallin <hal@chaosdev.org> */

/* Copyright 2000 chaos development. */

/* This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#include <storm/generic/avl.h>
#include <storm/generic/avl_rotate.h>
#include <storm/generic/avl_debug.h>
#include <storm/current-arch/debug.h>
#include <storm/current-arch/defines.h>
#include <storm/current-arch/memory.h>
#include <storm/current-arch/types.h>

/* Define as TRUE to get lots of debug information. */

#define DEBUG FALSE

avl_node_type *avl_rotate_right_single (avl_header_type *avl_header,
                                        avl_node_type *tree)
{
  char debug_string[MAX_LOG_MESSAGE_LENGTH];
  avl_node_type *node1 = (avl_node_type *) tree->less;
  avl_node_type *node2 = tree;
  avl_node_type *tree2 = (avl_node_type *) node1->more;

  memory_copy (debug_string, __FUNCTION__, MAX_LOG_MESSAGE_LENGTH);
  avl_debug_log_write (debug_string);

  //  debug_print ("RS on node %u.\n", tree->start);
  DEBUG_MESSAGE (DEBUG, "node1 %p.", node1);
  DEBUG_MESSAGE (DEBUG, "node2 %p.", node2);
  DEBUG_MESSAGE (DEBUG, "tree2 %p.", tree2);
  
  DEBUG_MESSAGE (DEBUG, "node_rotate_left_side_single: called on 0x%x.",
                 tree->start);
  
  if (node2->parent == NULL)
  {
    /* We're at the very top of the world. */
    
    avl_header->root = node1;
    node1->parent = NULL;
  }
  else
  {
    /* Left or right? */
    
    if (((avl_node_type *) node2->parent)->less ==
        (struct avl_node_type *) node2)
    {
      ((avl_node_type *) node2->parent)->less =
        (struct avl_node_type *) node1;	    
    }
    else if (((avl_node_type *) node2->parent)->more ==
             (struct avl_node_type *) node2)
    {
      ((avl_node_type *) node2->parent)->more =
        (struct avl_node_type *) node1;
    }
    else
    {
      DEBUG_HALT ("Parent node broken!");
    }
  }
  
  /* Parent updated. Go on. */
  
  node1->more = (struct avl_node_type *) node2;
  node2->less = (struct avl_node_type *) tree2;
  
  node1->parent = node2->parent;
  node2->parent = (struct avl_node_type *) node1;

  if (tree2 != NULL)
  {
    tree2->parent = (struct avl_node_type *) node2;
  }  

  /* Tree rotated, set balance. */
  
  node1->balance = 0;
  node2->balance = 0;
  
  /* Done. Update largest_free_*. */

  node2->largest_free_less = node1->largest_free_more;

  node1->largest_free_more = MAX_OF_THREE (node2->free_length,
                                           node2->largest_free_less,
                                           node2->largest_free_more);

  return node1;
}

avl_node_type *avl_rotate_left_single (avl_header_type *avl_header,
                                       avl_node_type *tree)
{
  char debug_string[MAX_LOG_MESSAGE_LENGTH];
  avl_node_type *node1 = tree;
  avl_node_type *node2 = (avl_node_type *) node1->more;
  avl_node_type *tree2 = (avl_node_type *) node2->less;

  memory_copy (debug_string, __FUNCTION__, MAX_LOG_MESSAGE_LENGTH);
  avl_debug_log_write (debug_string);

  //  debug_print ("LS on node %u.\n", tree->start);
  DEBUG_MESSAGE (DEBUG, "node1 %p.", node1);
  DEBUG_MESSAGE (DEBUG, "node2 %p.", node2);
  DEBUG_MESSAGE (DEBUG, "tree2 %p.", tree2);

  DEBUG_MESSAGE (DEBUG, "node_rotate_right_side_single: called on 0x%x.",
                 tree->start);

  if (node1->parent == NULL)
  {
    /* We're at the very top of the world. */

    avl_header->root = node2;

    /* FIXME: experimental! */

    node2->parent = NULL;
  }
  else
  {
    /* Left or right? */

    if (((avl_node_type *) node1->parent)->less ==
        (struct avl_node_type *) node1)
    {
      /* We're the left child of our parent. */

      ((avl_node_type *) node1->parent)->less =
        (struct avl_node_type *) node2;
    }
    else if (((avl_node_type *) node1->parent)->more ==
             (struct avl_node_type *) node1)
    {
      /* We're the right child of our parent. */

      ((avl_node_type *) node1->parent)->more =
        (struct avl_node_type *) node2;
    }
    else
    {
      /* We're not our parent's child! */

      DEBUG_HALT ("Adoption error.");
    }
  }

  /* Parent updated. Go on. */

  node1->more = (struct avl_node_type *) tree2;
  node2->less = (struct avl_node_type *) node1;

  node2->parent = node1->parent;
  node1->parent = (struct avl_node_type *) node2;

  if (tree2 != NULL)
  {
    tree2->parent = (struct avl_node_type *) node1;
  }

  /* Tree rotated, set balance. */

  node1->balance = 0;
  node2->balance = 0;

  /* Done. Update largest_free_*. */

  node1->largest_free_more = node2->largest_free_less;

  node2->largest_free_less = MAX_OF_THREE (node1->free_length,
                                           node1->largest_free_less,
                                           node1->largest_free_more);

  return node2;
}

avl_node_type *avl_rotate_right_double (avl_header_type *avl_header,
                                        avl_node_type *tree)
{
  char debug_string[MAX_LOG_MESSAGE_LENGTH];
  avl_node_type *node3 = tree;
  avl_node_type *node1 = (avl_node_type *) node3->less;
  avl_node_type *node2 = (avl_node_type *) node1->more;
  avl_node_type *tree2 = (avl_node_type *) node2->less;
  avl_node_type *tree3 = (avl_node_type *) node2->more;

  memory_copy (debug_string, __FUNCTION__, MAX_LOG_MESSAGE_LENGTH);
  avl_debug_log_write (debug_string);

  //  debug_print ("RD on node %u.\n", tree->start);
  DEBUG_MESSAGE (DEBUG, "node1 %p.", node1);
  DEBUG_MESSAGE (DEBUG, "node2 %p.", node2);
  DEBUG_MESSAGE (DEBUG, "node3 %p.", node3);
  DEBUG_MESSAGE (DEBUG, "tree2 %p.", tree2);
  DEBUG_MESSAGE (DEBUG, "tree3 %p.", tree3);

  DEBUG_MESSAGE (DEBUG, "node_rotate_left_side_double: called on 0x%x.",
                 tree->start);

  if (node3->parent == NULL)
  {
    /* We're at the very top of the world. */

    avl_header->root = node2;
    node2->parent = NULL;
  }
  else
  {
    /* Left or right? */
	
    if (((avl_node_type *) node3->parent)->less ==
        (struct avl_node_type *) node3)
    {
      /* We're the left child of our parent. */

      ((avl_node_type *) node3->parent)->less =
        (struct avl_node_type *) node2;
    }
    else if (((avl_node_type *) node3->parent)->more ==
             (struct avl_node_type *) node3)
    {
      /* We're the right child of our parent. */

      ((avl_node_type *) node3->parent)->more =
        (struct avl_node_type *) node2;
    }
    else
    {
      /* We're not our parent's child! */

      DEBUG_HALT ("Adoption error.");
    }
  }

  /* Parent updated. Go on. */

  node2->less = (struct avl_node_type *) node1;
  node2->more = (struct avl_node_type *) node3;
  node1->more = (struct avl_node_type *) tree2;
  node3->less = (struct avl_node_type *) tree3;

  node2->parent = node3->parent;
  node1->parent = (struct avl_node_type *) node2;
  node3->parent = (struct avl_node_type *) node2;

  if (tree2 != NULL)
  {
    tree2->parent = (struct avl_node_type *) node1;
  }
  if (tree3 != NULL)
  {
    tree3->parent = (struct avl_node_type *) node3;
  }

  /* Tree rotated, set balance. */
    
  if (node2->balance == -1)
  {
    node1->balance = 0;
    node3->balance = 1;
  } 
  else if (node2->balance == 1)
  {
    node1->balance = -1;
    node3->balance = 0;
  }
  else if (node2->balance == 0)
  {
    node1->balance = 0;
    node3->balance = 0;
  }
  else
  {
    DEBUG_HALT ("node_rotate_left_side_double: Bad balance.");
  }

  node2->balance = 0;

  /* Done. Update largest_free_*. */

  node1->largest_free_more = node2->largest_free_less;
  node3->largest_free_less = node2->largest_free_more;

  node2->largest_free_less = MAX_OF_THREE (node1->free_length,
                                           node1->largest_free_less,
                                           node1->largest_free_more);

  node2->largest_free_more = MAX_OF_THREE (node3->free_length,
                                           node3->largest_free_less,
                                           node3->largest_free_more);

  return node2;
}

avl_node_type *avl_rotate_left_double (avl_header_type *avl_header,
                                       avl_node_type *tree)
{
  char debug_string[MAX_LOG_MESSAGE_LENGTH];
  avl_node_type *node1 = tree;
  avl_node_type *node3 = (avl_node_type *) node1->more;
  avl_node_type *node2 = (avl_node_type *) node3->less;
  avl_node_type *tree2 = (avl_node_type *) node2->less;
  avl_node_type *tree3 = (avl_node_type *) node2->more;

  memory_copy (debug_string, __FUNCTION__, MAX_LOG_MESSAGE_LENGTH);
  avl_debug_log_write (debug_string);

  //  debug_print ("LD on node %u.\n", tree->start);
  DEBUG_MESSAGE (DEBUG, "node1 %p.", node1);
  DEBUG_MESSAGE (DEBUG, "node2 %p.", node2);
  DEBUG_MESSAGE (DEBUG, "node3 %p.", node3);
  DEBUG_MESSAGE (DEBUG, "tree2 %p.", tree2);
  DEBUG_MESSAGE (DEBUG, "tree3 %p.", tree3);

  DEBUG_MESSAGE (DEBUG, "node_rotate_right_side_double: called on 0x%x.",
                 tree->start);

  if (node1->parent == NULL)
  {
    /* We're at the very top of the world. */

    avl_header->root = node2;
    node2->parent = NULL;
  }
  else
  {
    /* Left or right? */
	
    if (((avl_node_type *) node1->parent)->less ==
        (struct avl_node_type *) node1)
    {
      /* We're the left child of our parent. */

      ((avl_node_type *) node1->parent)->less =
        (struct avl_node_type *) node2;
    }
    else if (((avl_node_type *) node1->parent)->more ==
             (struct avl_node_type *) node1)
    {
      /* We're the right child of our parent. */

      ((avl_node_type *) node1->parent)->more =
        (struct avl_node_type *) node2;
    }
    else
    {
      /* We're not our parent's child! */

      DEBUG_HALT ("Adoption error.");
    }
  }

  /* Parent updated. Go on. */

  node2->less = (struct avl_node_type *) node1;
  node2->more = (struct avl_node_type *) node3;
  node1->more = (struct avl_node_type *) tree2;
  node3->less = (struct avl_node_type *) tree3;

  node2->parent = node1->parent;
  node1->parent = (struct avl_node_type *) node2;
  node3->parent = (struct avl_node_type *) node2;

  if (tree2 != NULL)
  {
    tree2->parent = (struct avl_node_type *) node1;
  }
  if (tree3 != NULL)
  {
    tree3->parent = (struct avl_node_type *) node3;
  }

  /* Tree rotated, set balance. */

  if (node2->balance == -1)
  {
    node1->balance = 0;
    node3->balance = 1;
  }
  else if (node2->balance == 1)
  {
    node1->balance = -1;
    node3->balance = 0;
  }
  else if (node2->balance == 0)
  {
    node1->balance = 0;
    node3->balance = 0;
  }
  else
  {
    DEBUG_HALT ("Bad balance.");
  }

  node2->balance = 0;

  /* Done. Update largest_free_*. */

  node1->largest_free_more = node2->largest_free_less;
  node3->largest_free_less = node2->largest_free_more;

  node2->largest_free_less =  MAX_OF_THREE (node1->free_length,
                                            node1->largest_free_less,
                                            node1->largest_free_more);
  
  node2->largest_free_more = MAX_OF_THREE (node3->free_length,
                                           node3->largest_free_less,
                                           node3->largest_free_more);

  return node2;
}
