// Abstract: String routines.
// Author: Per Lundberg <per@halleluja.nu>

// © Copyright 1999 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015 chaos development

#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/return_values.h>
#include <storm/generic/string.h>
#include <storm/generic/types.h>

// Converts a string to a number.
return_type string_to_number(char *string, int *number)
{
    unsigned int index = 0;
    unsigned int base = 10;
    bool negative = FALSE;

    // Make sure we are not null.
    if (string == NULL)
    {
        return RETURN_INVALID_ARGUMENT;
    }

    // Is this a negative number?
    if (string[index] == '-')
    {
        negative = TRUE;
        index++;
    }

    // Check which base this is. Hexadecimal, perhaps?
    switch (string[index])
    {
        case '0':
        {
            index++;

            switch (string[index])
            {
                // Hexadecimal.
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

    // Null the number.
    *number = 0;

    switch (base)
    {
        // Binary.
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

        // Decimal.
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

        // Hexadecimal.
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
