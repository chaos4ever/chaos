/* $Id$ */
/* Abstract: Linked list code. */
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

#include <list/list.h>
#include <memory/memory.h>

/* Insert a node into the list. */

return_type list_node_insert (list_type **list, list_type *node)
{
  /* Check the input arguments. */

  if (list == NULL || node == NULL)
  {
    return LIST_RETURN_INVALID_ARGUMENT;
  }

  node->previous = NULL;
  node->next = (struct list_type *) *list;

  if (*list != NULL)
  {
    (*list)->previous = (struct list_type *) node;
  }

  *list = node;

  return LIST_RETURN_SUCCESS;
}

/* Delete a node from a list. */

return_type list_node_delete (list_type **list, list_type *node)
{
  list_type *next, *previous;

  /* Check the input arguments. */

  if (list == NULL || node == NULL)
  {
    return LIST_RETURN_INVALID_ARGUMENT;
  }

  previous = (list_type *) node->previous;
  next = (list_type *) node->next;

  /* First entry in the list? */

  if (previous == NULL)
  {
    *list = next;
  }
  else
  {
    previous->next = (struct list_type *) next;
  }

  if (next != NULL)
  {
    next->previous = (struct list_type *) previous;
  }

  memory_deallocate ((void **) &node);

  return LIST_RETURN_SUCCESS;
}
