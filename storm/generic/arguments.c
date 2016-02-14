// Abstract: Routines for parsing command line arguments.
// Author: Per Lundberg <per@chaosdev.io>
//         Henrik Hallin <hal@chaosdev.org>

// © Copyright 1999-2000, 2013 chaos development.

#include <storm/generic/arguments.h>
#include <storm/current-arch/types.h>
#include <storm/current-arch/string.h>

char arguments_kernel[MAX_KERNEL_PARAMETER_LENGTH];

static int find_number_of_arguments (char *source);

// Split the command line parameters in words (separated by one or more spaces).
u32 arguments_parse(char *source, char *destination, u32 delta)
{
    int number_of_arguments = find_number_of_arguments(source);

    *((u32 *) destination) = number_of_arguments;

    // Now, go through the string again and copy the parameters into the data area and set the pointers correctly.
    // FIXME: Avoid pointer arithmetic. We should have a pure structure for this. I'm not sure if it is possible to write this
    // in a clean and nice way at all at the moment...
    char **word_pointer = (char **) (destination + 4);
    char *word = (char *) (destination + 4 + sizeof (char *) * number_of_arguments);

    for (int position = 0, current_arg = 0; source[position] != '\0'; position++)
    {
        while (source[position] == ' ')
        {
            position++;
        }

        if (source[position] != '\0')
        {
            int word_length = 0;
            while (source[position + word_length] != ' ' &&
                   source[position + word_length] != '\0')
            {
                word_length++;
            }

            string_copy_max(word, &source[position], word_length);

            position += word_length - 1;
            word[word_length] = 0;
            word_pointer[current_arg] = word + delta;
            word += word_length + 1;

            current_arg++;
        }
        else break;
    }

    return number_of_arguments;
}

static int find_number_of_arguments(char *source)
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
