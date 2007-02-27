/* $Id$ */
/* Abstract: Linked list code. */

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

#include <storm/generic/defines.h>
#include <storm/generic/list.h>

void list_insert (list_type **list, list_type *node)
{
  node->previous = NULL;
  node->next = (struct list_type *) *list;

  if (*list != NULL)
  {
    (*list)->previous = (struct list_type *) node;
  }

  *list = node;
}

void list_delete (list_type **list, list_type *node)
{
  list_type *previous = (list_type *) node->previous;
  list_type *next = (list_type *) node->next;

  if (previous != NULL)
  {
    previous->next = (struct list_type *) next;
  }
  else
  {
    *list = next;
  }

  if (next != NULL)
  {
    next->previous = (struct list_type *) previous;
  }
}
