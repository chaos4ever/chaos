/* $Id$ */
/* Abstract: Functions used by the AVL tree functions. */
/* Authors: Henrik Hallin <hal@chaosdev.org,
            Anders Öhrt <doa@chaosdev.org> */

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
   USA */

#define DEBUG FALSE

#include <storm/generic/debug.h>
#include <storm/generic/avl.h>
#include <storm/generic/avl_debug.h>
#include <storm/generic/avl_rotate.h>
#include <storm/generic/avl_update.h>
#include <storm/generic/cpu.h>
#include <storm/generic/defines.h>
#include <storm/generic/limits.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/return_values.h>
#include <storm/generic/types.h>

/* Given a new node, this function sets all properties for the node. */

void avl_node_reset (avl_node_type *node, unsigned int start,
                     unsigned int busy_length, unsigned int free_length,
		     avl_node_type *parent)
{
  node->start = start;
  node->busy_length = busy_length;
  node->free_length = free_length;
  node->parent = (struct avl_node_type *) parent;
  node->less = NULL;
  node->more = NULL;
  node->balance = 0;
  node->largest_free_less = 0;
  node->largest_free_more = 0;
}

/* Find a free node in the entry bitmap. This function is also
   responsible for growing the tree when needed. */

avl_node_type *avl_node_allocate (avl_header_type *avl_header)
{
  unsigned index, temp, bit_index = 0;

  /* Find a free entry from the bitmap. */
  
  for (index = 0; index < avl_header->limit_nodes / 32; index++)
  {
    if (avl_header->bitmap[index] != MAX_U32)
    {
      /* We have found a free entry! Whee... */

      temp = avl_header->bitmap[index];

      /* Iterate through until we find a free spot. */
      
      while ((temp & 1) == 1)
      {
        temp /= 2;
        bit_index++;
      }

      /* Mark the found entry as reserved. */

      avl_header->bitmap[index] |= (BIT_VALUE (bit_index));
      avl_header->number_of_nodes++;

      /* FIXME: Really check this function. */

      avl_update_mappings (avl_header);

      return &avl_header->node_array[(index * 32) + bit_index];
    }
  }

  /* No free node. */

  return NULL;
}

/* Free a node in an AVL-tree. This includes freeing the node in the
   bitmap and checking if we can unmap/deallocate a page or two. */
/* FIXME: Write and use this! */

static void node_deallocate (avl_header_type *avl_header __attribute__ ((unused)),
                             avl_node_type *node)
{
  avl_header = avl_header;
  //  avl_header->number_of_nodes--;
  node->start = (unsigned int) -1;
}

/* Insert a node in the tree. Recurses down the tree to find out where
   to put it. */

void avl_node_insert (avl_header_type *avl_header,
                      avl_node_type *insert_node)
{
  avl_node_type *node_parent = NULL;
  avl_node_type *node = avl_header->root;
  avl_node_type *node_child;
  bool finished = FALSE;

  DEBUG_MESSAGE (DEBUG, "Trying to insert (%u, %u, %u)...",
                 insert_node->start, insert_node->busy_length,
                 insert_node->free_length);

  if (insert_node == NULL)
  {
    DEBUG_HALT ("Refusing to insert NULL in tree.");
  }

  /* Start by looping down to the correct place of insertion. */

  while (node != NULL)
  {
    node_parent = node;

    if (insert_node->start < node->start)
    {
      node = (avl_node_type *) node->less;
    }
    else if (insert_node->start > node->start)
    {
      node = (avl_node_type *) node->more;
    }
    else
    {
      DEBUG_HALT ("Node already in tree.");
    }
  }

  /* Tree is the node under which we are to insert the new node. */

  if (insert_node->start < node_parent->start)
  {
    node_parent->less = insert_node;
  }
  else if (insert_node->start > node_parent->start)
  {
    node_parent->more = insert_node;
  }

  insert_node->parent = node_parent;
  
  /* The node is inserted. Loop up the tree to find where, if needed,
     we have to rotate. */

  node = insert_node;
  node_child = NULL;
  node_parent = node->parent;

  /* node_parent is the node we check for rotation as we loop up the tree. */

  while (node_parent != NULL && !finished)
  {
    avl_update_tree_largest_free (node_parent);

    if (node_parent->less == node)
    {
      if (node_parent->balance == -1)
      {
        /* We need to rotate the tree to maintain AVL-properties. */

        if (node->less == node_child)
        {
          avl_update_tree_largest_free
            (avl_rotate_right_single (avl_header, node_parent)->parent);
        }
        else if (node->more == node_child)
        {
          avl_update_tree_largest_free
            (avl_rotate_right_double (avl_header, node_parent)->parent);
        }
        else
        {
          DEBUG_HALT ("Some error occured.");
        }

        finished = TRUE;
      }
      else if (node_parent->balance == 1)
      {
        node_parent->balance = 0;
        finished = TRUE;
      }
      else
      {
        node_parent->balance = -1;
      }
    }
    else if (node_parent->more == node)
    {
      if (node_parent->balance == 1)
      {
        /* We need to rotate the tree to maintain AVL-properties. */

        if (node->more == node_child)
        {
          avl_update_tree_largest_free
            (avl_rotate_left_single (avl_header, node_parent)->parent);
        }
        else if (node->less == node_child)
        {
          avl_update_tree_largest_free
            (avl_rotate_left_double (avl_header, node_parent)->parent);
        }
        else
        {
          DEBUG_HALT ("Some error occured.");
        }

        finished = TRUE;
      }
      else if (node_parent->balance == -1)
      {
        node_parent->balance = 0;
        finished = TRUE;
      }
      else
      {
        node_parent->balance = 1;
      }
    }
    else
    {
      DEBUG_HALT ("Adoption error!");
    }

    node_child = node;
    node = node_parent;
    node_parent = node->parent;
  }

  DEBUG_MESSAGE (DEBUG, "Leaving.");
  //  avl_debug_tree_check (avl_header, avl_header->root);

  return;
}

/* This function loops up the tree from a given node, as long as there
   are things to be updated. One or more rotations might be
   necessary. */

static void tree_rebalance (avl_header_type *avl_header,
                            avl_node_type *tree, int side)
{
  /* When the tree does not shrink anymore, done is set to TRUE. */

  bool done = FALSE;
  avl_node_type *new_root;
  int old_balance;

  //  debug_print ("Rebalancing node %u from side %d.\n", tree->start, side);

  while (!done)
  {
    if (side == -1)
    {      
      if (tree->balance == -1)
      {
        tree->balance = 0;
      }
      else if (tree->balance == 0)
      {
        tree->balance = 1;
        done = TRUE;
      }
      else if (tree->balance == 1)
      {
        /* We need to rotate around tree to maintain AVL-properties. */

        if (tree->more->balance == 1)
        {
          new_root = avl_rotate_left_single (avl_header, tree);
          new_root->balance = 0;
          tree->balance = 0;
          tree = new_root;
        }
        else if (tree->more->balance == 0)
        {
          new_root = avl_rotate_left_single (avl_header, tree);
          new_root->balance = -1;
          tree->balance = 1;
          done = TRUE;
        }
        else if (tree->more->balance == -1)
        {
          old_balance = tree->more->less->balance;
          new_root = avl_rotate_left_double (avl_header, tree);
          new_root->balance = 0;

          if (old_balance == -1)
          {
            tree->balance = 0;
            new_root->more->balance = 1;
          }
          else if (old_balance == 0)
          {
            tree->balance = 0;
            new_root->more->balance = 0;
          }
          else if (old_balance == 1)
          {
            tree->balance = -1;
            new_root->more->balance = 0;
          }
          else
          {
            DEBUG_HALT ("Igelskit");
          }
          tree = new_root;
        }
        else
        {
          DEBUG_HALT ("mirk");
        }
      }
      else
      {
        DEBUG_HALT ("Sug apa.");
      }
    }
    else if (side == 1)
    {
      if (tree->balance == -1)
      {
        /* We need to rotate around tree to maintain AVL-properties. */

        if (tree->less->balance == -1)
        {
          new_root = avl_rotate_right_single (avl_header, tree);
          new_root->balance = 0;
          tree->balance = 0;
          tree = new_root;
        }
        else if (tree->less->balance == 0)
        {
          new_root = avl_rotate_right_single (avl_header, tree);
          new_root->balance = 1;
          tree->balance = -1;
          done = TRUE;
        }
        else if (tree->less->balance == 1)
        {
          old_balance = tree->less->more->balance;
          new_root = avl_rotate_right_double (avl_header, tree);
          new_root->balance = 0;

          if (old_balance == -1)
          {
            tree->balance = 1;
            new_root->less->balance = 0;
          }
          else if (old_balance == 0)
          {
            tree->balance = 0;
            new_root->less->balance = 0;
          }
          else if (old_balance == 1)
          {
            tree->balance = 0;
            new_root->less->balance = 1;
          }
          else
          {
            DEBUG_HALT ("Igelskit");
          }
          tree = new_root;
        }
        else
        {
          DEBUG_HALT ("mirk");
        }
      }
      else if (tree->balance == 0)
      {
        tree->balance = -1;
        done = TRUE;
      }
      else if (tree->balance == 1)
      {
        tree->balance = 0;
      }
      else
      {
        DEBUG_HALT ("IT-ull.");
      }
    }
    else
    {
      DEBUG_HALT ("Invalid value of side.");
    }

    /* Update parent. */

    if (tree->parent != NULL)
    {
      if (tree->parent->more == tree)
      {
        side = 1;
      }
      else if (tree->parent->less == tree)
      {
        side = -1;
      }
      else
      {
        DEBUG_HALT ("Another adoption error.");
      }
      
      tree = tree->parent;
    }
    else
    {
      done = TRUE;
    }
  }
}

/* Delete a node from the tree. */

void avl_node_delete (avl_header_type *avl_header, avl_node_type *node)
{
  int side;

  /* There are four cases we need to handle. */
  /* First case: There are no children. */

  if (node->less == NULL && node->more == NULL)
  {
    if (node->parent == NULL)
    {
      DEBUG_HALT ("There must be at least one node in the tree!");
    }

    if (node->parent->less == node)
    {
      side = -1;
      node->parent->less = NULL;
    }
    else if (node->parent->more == node)
    {
      side = 1;
      node->parent->more = NULL;
    }
    else 
    {
      DEBUG_HALT ("Adoption error again.");
    }

    avl_update_tree_largest_free (node->parent);
    
    tree_rebalance (avl_header, node->parent, side);

    node_deallocate (avl_header, node);
  }

  /* Second case: There is one child on the more side. */

  else if (node->less == NULL && node->more != NULL)
  {
    if (node->parent == NULL)
    {
      avl_header->root = node->more;
      node->more->parent = NULL;

      node_deallocate (avl_header, node);
      return;
    }

    if (node->parent->less == node)
    {
      side = -1;
      node->parent->less = node->more;
    }
    else if (node->parent->more == node)
    {
      side = 1;
      node->parent->more = node->more;
    }
    else 
    {
      DEBUG_HALT ("Adoption error again.");
    }

    node->more->parent = node->parent;

    avl_update_tree_largest_free (node->parent);

    tree_rebalance (avl_header, node->parent, side);

    node_deallocate (avl_header, node);
  }

  /* Third case: There is one child on the less side. */

  else if (node->less != NULL && node->more == NULL)
  {
    if (node->parent == NULL)
    {
      avl_header->root = node->less;
      node->less->parent = NULL;

      node_deallocate (avl_header, node);
      return;
    }

    if (node->parent->less == node)
    {
      side = -1;
      node->parent->less = node->less;
    }
    else if (node->parent->more == node)
    {
      side = 1;
      node->parent->more = node->less;
    }
    else 
    {
      DEBUG_HALT ("Adoption error again.");
    }

    node->less->parent = node->parent;

    avl_update_tree_largest_free (node->parent);

    tree_rebalance (avl_header, node->parent, side);

    node_deallocate (avl_header, node);
  }

  /* Fourth case: There are two children. */

  else if (node->less != NULL && node->more != NULL)
  {
    avl_node_type *node2, *node3, *node4;

    /* The right side is the highest. */

    if (node->balance == 1)
    {
      node2 = node->more;
      node3 = node2->less;

      while (node3 != NULL)
      {
        node2 = node3;
        node3 = node3->less;
      }

      node3 = node2->more;

      node->start = node2->start;
      node->busy_length = node2->busy_length;
      node->free_length = node2->free_length;

      node4 = node2->parent;

      if (node4 == node)
      {
        node->more = node3;

        if (node3 != NULL)
        {
          node3->parent = node;
        }

        node_deallocate (avl_header, node2);

        avl_update_tree_largest_free (node4);

        tree_rebalance (avl_header, node4, 1);
      }
      else
      {
        node4->less = node3;

        if (node3 != NULL)
        {
          node3->parent = node4;
        }

        node_deallocate (avl_header, node2);

        avl_update_tree_largest_free (node4);

        tree_rebalance (avl_header, node4, -1);
      }
    }
    
    /* The two sides are equally high or the left side is the
       highest. */

    else
    {
      node2 = node->less;
      node3 = node2->more;

      while (node3 != NULL)
      {
        node2 = node3;
        node3 = node3->more;
      }

      node3 = node2->less;

      node->start = node2->start;
      node->busy_length = node2->busy_length;
      node->free_length = node2->free_length;

      node4 = node2->parent;

      if (node4 == node)
      {
        node->less = node3;

        if (node3 != NULL)
        {
          node3->parent = node;
        }

        node_deallocate (avl_header, node2);

        avl_update_tree_largest_free (node4);

        tree_rebalance (avl_header, node4, -1);
      }
      else
      {
        node4->more = node3;

        if (node3 != NULL)
        {
          node3->parent = node4;
        }

        node_deallocate (avl_header, node2);

        avl_update_tree_largest_free (node4);

        tree_rebalance (avl_header, node4, 1);
      }
    }
  }
  else
  {
    DEBUG_HALT ("FLUM");
  }
}

/* Move the AVL tree below the given node virtually. Update pointers,
   but don't move data. This one is recursive. */

static void node_move (avl_node_type *node, u32 delta)
{
  u32 left_offset, right_offset, parent_offset;
  
  /* Have we reached end of tree? */

  if (node == NULL)
  {
    return;
  }

  /* Recurse down the tree. */
  
  node_move (node->less, delta);
  node_move (node->more, delta);

  /* Update pointers for node. */

  if (node->less != NULL)
  {
    left_offset = (u32) node->less;
    left_offset += delta;
    node->less = (avl_node_type *) left_offset;
  }

  if (node->more != NULL)
  {
    right_offset = (u32) node->more;
    right_offset += delta;
    node->more = (avl_node_type *) right_offset;
  }

  if (node->parent != NULL)
  {
    parent_offset = (u32) node->parent;
    parent_offset += delta;
    node->parent = (avl_node_type *) parent_offset;
  }
}

/* Move the AVL tree and update the header. */

void avl_tree_move (avl_header_type *avl_header, unsigned int delta)
{
  unsigned int root_offset, node_array_offset;

  node_move (avl_header->root, delta);

  /* Update the pointers in the AVL header. */

  root_offset = (unsigned int) avl_header->root;
  node_array_offset = (unsigned int) avl_header->node_array;

  root_offset += delta;
  node_array_offset += delta;

  avl_header->root = (avl_node_type *) root_offset;
  avl_header->node_array = (avl_node_type *) node_array_offset;
}
