/* $Id$ */
/* Abstract: AVL debugging functions. */
/* Author: Henrik Hallin <hal@chaosdev.org> */

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

#define DEBUG FALSE

#include <storm/generic/avl.h>
#include <storm/generic/avl_debug.h>
#include <storm/ia32/defines.h>
#include <storm/ia32/debug.h>
#include <storm/ia32/memory.h>

/* Dump information about the page allocation system. */

static void tree_dump (avl_node_type *tree)
{
  if (tree == NULL)
  {
    debug_print (", (NULL)");
    return;
  }

  debug_print ("\n(%u, %u, %u) lfl: %u lfr: %u b: %d",
               tree->start,
               tree->busy_length,
               tree->free_length,
               tree->largest_free_less,
               tree->largest_free_more,
               tree->balance);

  if (tree->parent == NULL)
  {
    debug_print (" p: (null)");
  }
  else
  {
    debug_print (" p: %u", tree->parent->start);
  }

  tree_dump ((avl_node_type *) tree->less);
  tree_dump ((avl_node_type *) tree->more);
}

/* Dump the contents of the the AVL tree on screen. */

void avl_debug_tree_dump (avl_node_type *tree)
{
  DEBUG_MESSAGE (DEBUG, "Dumping AVL tree:");
  tree_dump (tree);
  debug_print ("\n");
  DEBUG_MESSAGE (DEBUG, "Done.");
}

static unsigned int get_largest_free (avl_node_type *node)
{
  if (node == NULL)
  {
    return 0;
  }

  return MAX_OF_THREE (get_largest_free (node->less),
                       node->free_length,
                       get_largest_free (node->more));
}

static unsigned int tree_height (avl_node_type *tree)
{
  if (tree == NULL)
  {
    return 0;
  }

  return 1 + MAX_OF_TWO (tree_height (tree->less), tree_height (tree->more));
}

void avl_debug_tree_check (avl_header_type *avl_header, avl_node_type *tree)
{
  if (tree == NULL)
  {
    return;
  }

  if (get_largest_free (tree->less) != tree->largest_free_less)
  {
    avl_debug_tree_dump (tree);
    avl_debug_log_dump ();
    DEBUG_HALT ("Largest free less failed for node %u.", tree->start);
  }

  if (get_largest_free (tree->more) != tree->largest_free_more)
  {
    avl_debug_tree_dump (tree);
    avl_debug_log_dump ();
    DEBUG_HALT ("Largest free more failed for node %u.", tree->start);
  }

  if (tree->more != NULL && tree->more->parent != tree)
  {
    avl_debug_tree_dump (tree);
    avl_debug_log_dump ();
    DEBUG_HALT ("More child without parent.");
  }

  if (tree->less != NULL && tree->less->parent != tree)
  {
    avl_debug_tree_dump (tree);
    avl_debug_log_dump ();
    DEBUG_HALT ("Less child without parent.");
  }

  if (tree_height (tree->less) == tree_height (tree->more) + 1 && tree->balance != -1)
  {
    avl_debug_tree_dump (tree);
    avl_debug_log_dump ();
    DEBUG_HALT ("Balance failed for node %u.", tree->start);
  }

  if (tree_height (tree->less) + 1 == tree_height (tree->more) && tree->balance != 1)
  {
    avl_debug_tree_dump (tree);
    avl_debug_log_dump ();
    DEBUG_HALT ("Balance failed for node %u.", tree->start);
  }

  if (tree_height (tree->less) == tree_height (tree->more) && tree->balance != 0)
  {
    avl_debug_tree_dump (tree);
    avl_debug_log_dump ();
    DEBUG_HALT ("Balance failed for node %u.", tree->start);
  }

  avl_debug_tree_check (avl_header, tree->less);
  avl_debug_tree_check (avl_header, tree->more);
}

static char debug_log [MAX_LOG_SIZE][MAX_LOG_MESSAGE_LENGTH];
static int log_entries = 0;
static int current_log_position = 0;

#define min(x, y) ((x) < (y) ? (x) : (y))

void avl_debug_log_write (char *string)
{
  memory_copy (debug_log [current_log_position],
               string, MAX_LOG_MESSAGE_LENGTH);
  
  current_log_position++;
  log_entries++;

  current_log_position %= MAX_LOG_SIZE;
  log_entries = min (log_entries, MAX_LOG_SIZE);
}

void avl_debug_log_dump (void)
{
  return;
#if FALSE
  int count = 0, entry;
  
  debug_print ("avl_debug_log_dump: Dumping %d log entries:\n",
               log_entries);    
  
  for (entry = current_log_position; entry >= 0; entry--, count++)
  {
    debug_print ("%d|%gd : %s\n", count, entry, debug_log [entry]);
  }
  
  for (entry = log_entries - 1; entry > current_log_position;
       entry--, count++)
  {
    debug_print ("%d|%d : %s\n", count, entry, debug_log [entry]);
  }
#endif
}
