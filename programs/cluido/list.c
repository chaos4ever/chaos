/* $Id$ */
/* Copyright 1999 chaos development */

/* Description: Definition of data structures and functiosn used by
   the caps-build configuration file parser */
/* Author: Per Lundberg <plundis@chaosdev.org> */
   
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

#include <defs.h>
#include <memory.h>
#include <printf.h>

#include "list.h"

list_type *list_create (void)
{
  list_type *new_entry;

  memory_allocate (sizeof (list_type), (void **) &new_entry);

  new_entry->next = NULL;
  new_entry->previous = NULL;
  new_entry->data = NULL;

  return new_entry;
}

list_type *list_insert (list_type *list, void *data)
{
  list_type *new_entry;

  memory_allocate (sizeof (list_type), (void **) &new_entry);
  new_entry->next = (struct list_type *) list;
  new_entry->previous = NULL;
  list->previous = (struct list_type *) new_entry;
  new_entry->data = data;

  return new_entry;
}

void *list_head (list_type *list)
{
  return list->data;
}

list_type *list_tail (list_type *list)
{
  list_type *temp = (list_type *) list->next;

  return temp;
}

void list_destroy (list_type *list)
{
  while (list != NULL)
  {
    list_type *temp_list_pointer = list;

    memory_deallocate (list->data);
    (list_type *) list = (list_type *) list->next;
    memory_deallocate (temp_list_pointer);
  }
}
