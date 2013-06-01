/* Abstract: Routines for parsing command line arguments. */
/* Author: Per Lundberg <per@halleluja.nu>
           Henrik Hallin <hal@chaosdev.org> */

/* Copyright 1999-2000, 2013 chaos development. */

#include <storm/generic/arguments.h>
#include <storm/current-arch/types.h>
#include <storm/current-arch/string.h>

char arguments_kernel[MAX_KERNEL_PARAMETER_LENGTH];

static int find_number_of_arguments (char *source);

/* Split the command line parameters in words (separated by one or
   more spaces). */

extern "C"
u32 arguments_parse (char *source, char *destination, u32 delta)
{
  int args = find_number_of_arguments(source);

  /* Save this value. */
  
  *((u32 *) destination) = args;

  /* Now, go through the string again and copy the parameters into the
     data area and set the pointers correctly. */
  /* FIXME: Avoid pointer arithmetic. We should have a pure structure
     for this. I'm not sure if it is possible to write this in a clean
     and nice way at all. */

  auto word_pointer = (char **) (destination + 4);
  auto word = (char *) (destination + 4 + sizeof (char *) * args);

  /* OK, the initial pointers are setup. */

  for (int position = 0, arg = 0; source[position] != '\0'; position++)
  {
    while (source[position] == ' ')
    {
      position++;
    }

    if (source[position] != '\0')
    {
      /* Find out the length of the word. */

      int length = 0;
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

static int find_number_of_arguments (char *source)
{
  int position = 0, result = 0;

  while (source[position] != '\0')
  {
    if (source[position] != ' ')
    {
      result++;

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

  return result;
}
