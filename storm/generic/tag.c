// Abstract: Tag item management.
// Author: Per Lundberg <per@halleluja.nu>

// Copyright 2000, 2013 chaos development.

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
