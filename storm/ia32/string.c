/* $Id$ */
/* Abstract: String routines. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999 chaos development. */

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

#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/return_values.h>
#include <storm/generic/string.h>
#include <storm/generic/types.h>

/* Converts a string to a number. */

return_type string_to_number (char *string, int *number)
{
  unsigned int index = 0;
  unsigned int base = 10;
  bool negative = FALSE;

  /* Make sure we are not null. */

  if (string == NULL)
  {
    return RETURN_INVALID_ARGUMENT;
  }

  /* Is this a negative number? */

  if (string[index] == '-')
  {
    negative = TRUE;
    index++;
  }

  /* Check which base this is. Hexadecimal, perhaps? */

  switch (string[index])
  {
    case '0':
    {
      index++;

      switch (string[index])
      {
        /* Hexadecimal. */

        case 'x':
        {
          base = 16;
          index++;
          break;
        }

        case 'b':
        {
          base = 2;
          index++;
          break;
        }
      }
      break;
    }
  }

  /* Null the number. */

  *number = 0;

  switch (base)
  {
    /* Binary. */

    case 2:
    {
      while (string[index] != '\0')
      {
        *number *= 2;
        *number += string[index] - '0';
        index++;
      }
      break;
    }
    
    /* Decimal. */

    case 10:
    {
      while (string[index] != '\0')
      {
        *number *= 10;
        if ('0' <= string[index] && string[index] <= '9')
        {
          *number += string[index] - '0';
        }
        else
        {
          return RETURN_INVALID_ARGUMENT;
        }
        index++;
      }
      break;
    }

    /* Hexadecimal. */

    case 16:
    {
      while (string[index] != '\0')
      {
        *number *= 16;

        if ('0' <= string[index] && string[index] <= '9')
        {
          *number += string[index] - '0';
        }
        else if (string[index] >= 'a' && string[index] <= 'f')
        {
          *number += string[index] - 'a' + 10;
        }
        else if (string[index] >= 'A' && string[index] <= 'F')
        {
          *number += string[index] - 'A' + 10;
        }
        else
        {
          return RETURN_INVALID_ARGUMENT;
        }
        index++;
      }

      break;
    }
  }   

  if (negative)
  {
    *number = 0 - *number;
  }

  return RETURN_SUCCESS;
}
