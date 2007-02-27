/* $Id$ */
/* Abstract: Tag item management. */
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

#include <storm/generic/tag.h>

/* Get the total length of this tag list. */

unsigned int tag_get_length (tag_type *tag)
{
  unsigned int length = 0;
  tag_type *where = tag;

  while (where->type != 0)
  {
    length += where->length + sizeof (unsigned int) * 2;
    where = (tag_type *) ((u32) where + (u32) where->length + 
                          sizeof (unsigned int) * 2);
  }

  length += sizeof (unsigned int) * 2;

  return length;
}

/* Perform a 'masked comparison' of the two tags. That is, all the
   tags in the second list must be present in the first one for this
   function to return true. */

bool tag_masked_compare (tag_type *tag, tag_type *tag_mask)
{
  tag_type *mask = tag_mask;

  while (mask->type != 0)
  {
    tag_type *where = tag;

    while (where->type != 0)
    {
      if (where->type == mask->type)
      {
        break;
      }
      where = (tag_type *) ((u32) where + (u32) where->length + 
                            sizeof (unsigned int) * 2);
    }

    if (where->type == 0)
    {
      return FALSE;
    }

    mask = (tag_type *) ((u32) mask + (u32) mask->length +
                         sizeof (unsigned int) * 2);
  }

  return TRUE;
}
