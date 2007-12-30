/* $Id$ */
/* Abstract: Linked list code. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */
/* Copyright 2007 chaos development. */

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

  list_type **node_pointer = &node;
  memory_deallocate ((void **) node_pointer);

  return LIST_RETURN_SUCCESS;
}
