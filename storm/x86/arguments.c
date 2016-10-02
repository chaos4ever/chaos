/* Abstract: Routines for parsing command line arguments. */

/* Copyright 1999-2000, 2013 chaos development. */

#include <storm/types.h>
#include <storm/x86/arguments.h>
#include <storm/x86/string.h>

char arguments_kernel[MAX_KERNEL_PARAMETER_LENGTH];

/* Split the command line parameters in words (separated by one or
   more spaces). */

uint32_t arguments_parse (char *source, char *destination, uint32_t delta)
{
  uint32_t args = 0, position, arg, length;
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
  
  *((uint32_t *) destination) = args;

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
