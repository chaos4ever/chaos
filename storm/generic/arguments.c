/* $Id$ */
/* Abstract: Routines for parsing command line arguments. */

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

#include <storm/generic/arguments.h>
#include <storm/current-arch/types.h>
#include <storm/current-arch/string.h>

char arguments_kernel[MAX_KERNEL_PARAMETER_LENGTH];

/* Split the command line parameters in words (separated by one or
   more spaces). */

u32 arguments_parse (char *source, char *destination, u32 delta)
{
  u32 args = 0, position, arg, length;
  char **word_pointer, *word;

  /* First of all, find out how many parameters we have. */

  position = 0;

  while (source[position] != '\0')
  {
    if (source[position] != ' ')
    {
      args++;
      while (source[position] != ' ' && source[position] != '\0')
      {
        position++;
      }
    }
    else
    {
      position++;
    }
  }

  /* Save this value. */
  
  *((u32 *) destination) = args;

  /* Now, go through the string again and copy the parameters into the
     data area and set the pointers correctly. */
  /* FIXME: Avoid pointer arithmetic. We should have a pure structure
     for this. I'm not sure if it is possible to write this in a clean
     and nice way at all. */

  word_pointer = (char **) (destination + 4);
  word = (char *) (destination + 4 + sizeof (char *) * args);

  /* OK, the initial pointers are setup. */

  for (position = 0, arg = 0; source[position] != '\0'; position++)
  {
    while (source[position] == ' ')
    {
      position++;
    }

    if (source[position] != '\0')
    {
      /* Find out the length of the word. */

      length = 0;
      while (source[position + length] != ' ' &&
             source[position + length] != '\0')
      {
        length++;
      }

      string_copy_max (word, &source[position], length);

      position += length - 1;
      word[length] = 0;
      word_pointer[arg] = word + delta;
      word += length + 1;
      arg++;
    }
    else break;
  }

  return args;
}
