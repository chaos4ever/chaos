/* $Id$ */
/* Abstract: String library functions. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */
/* Copyright 2007 chaos development. */

#include <system/system.h>
#include <string/string.h>

#include <stdarg.h>

/* Flags used by string_print_va. */

/* Pad with zero */

#define ZERO_PAD        1

/* Unsigned/signed long. */

#define SIGN            2

/* Show plus. */

#define PLUS            4

/* Space if plus. */

#define SPACE           8

/* Left justified */

#define LEFT            16

/* 0x. */

#define SPECIAL         32

/* Use 'ABCDEF' instead of 'abcdef'. */

#define LARGE           64

/* Converts a string to a number. */

return_type string_to_number (const char *string, int *number,
                              unsigned int *characters)
{
  unsigned int index = 0;
  unsigned int base = 10;
  bool negative = FALSE;

  /* Make sure we are not null. */

  if (string == NULL)
  {
    return STRING_RETURN_INVALID_ARGUMENT;
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

        case 'X':
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
      while ((string[index] == '0' ||
              string[index] == '1') &&
             string[index] != '\0')
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
      while (string[index] >= '0' &&
             string[index] <= '9' &&
             string[index] != '\0')
      {
        *number *= 10;
        if ('0' <= string[index] && string[index] <= '9')
        {
          *number += string[index] - '0';
        }
        else
        {
          return STRING_RETURN_INVALID_ARGUMENT;
        }
        index++;
      }
      break;
    }

    /* Hexadecimal. */

    case 16:
    {
      while (((string[index] >= '0' &&
               string[index] <= '9') ||
              (string[index] >= 'a' &&
               string[index] <= 'f') ||
              (string[index] >= 'A' &&
               string[index] <= 'F')) &&
             string[index] != '\0')
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
          return STRING_RETURN_INVALID_ARGUMENT;
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

  /* If we're not interested in the number of characters slurped by
     this function, we might have set characters to NULL. */

  if (characters != NULL)
  {
    *characters = index;
  }

  return STRING_RETURN_SUCCESS;
}

/* Converts a number to a string. */

static char *number_to_string (char *string, unsigned long number, int base,
                               int size, int precision, int flags)
{
  char pad_character, sign = 0, tmp_string[66];
  const char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
  int tmp_index = 0;
  int real_index = 0;

  /* Sometimes we prefer to print things with large hexadecimals. */

  if ((flags & LARGE) != 0)
  {
    digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  }

  /* Pad to left? */

  if ((flags & LEFT) != 0)
  {
    flags &= ~ZERO_PAD;
  }

  /* We only support base 2 to 36. */

  if (base < 2 || base > 36)
  {
    return 0;
  }
  pad_character = ((flags & ZERO_PAD) != 0) ? '0' : ' ';

  /* Do we have a sign character? If so, add it. */

  if ((flags & SIGN) != 0) 
  {
    if ((long) number < 0) 
    {
      sign = '-';
      number = -((long) number);
      size--;
    }
    else if ((flags & PLUS) != 0) 
    {
      sign = '+';
      size--;
    }
    else if ((flags & SPACE) != 0) 
    {
      sign = ' ';
      size--;
    }
  }

  if ((flags & SPECIAL) != 0) 
  {
    if (base == 16)
    {
      size -= 2;
    }
    else if (base == 8)
    {
      size--;
    }
  }

  if (number == 0)
  {
    tmp_string[tmp_index] = '0';
    tmp_index++;
  }
  else 
  {
    while (number != 0)
    {
      tmp_string[tmp_index] = digits[number % base];
      number /= base;
      tmp_index++;
    }
  }

  /* Okay, we have the string, but in reversed order. Lets do some
     more magic before we reverse it. */

  if (tmp_index > precision)
  {
    precision = tmp_index;
  }

  size -= precision;

  if (!((flags & (ZERO_PAD + LEFT)) != 0))
  {
    while (size > 0)
    {
      string[real_index] = ' ';
      real_index++;
      size--;
    }
  }

  /* If we have a sign character, prepend it. */

  if (sign != 0)
  {
    string[real_index] = sign;
    real_index++;
  }

  /* Do we want 0x or 0 before the number? */

  if ((flags & SPECIAL) != 0)
  {
    if (base == 8)
    {
      string[real_index] = '0';
      real_index++;
    }
    else if (base == 16) 
    {
      string[real_index] = '0';
      string[real_index + 1] = digits[33];
      real_index += 2;
    }
  }
  
  /* We might want to pad characters a little. */

  if (!((flags & LEFT) != 0))
  {
    while (size > 0)
    {
      string[real_index] = pad_character;
      real_index++;
      size--;
    }
  }

  while (tmp_index < precision)
  {
    string[real_index] = '0';
    real_index++;
    precision--;
  }

  /* Copy the string. */

  while (tmp_index > 0)
  {
    tmp_index--;
    string[real_index] = tmp_string[tmp_index];
    real_index++;
  }

  while (size > 0)
  {
    string[real_index] = ' ';
    real_index++;
    size--;
  }

  string[real_index] = '\0';

  return string;
}

#if FALSE
/* Convert a decimal number to a string. */

static void decimal_string (char *string, int number)
{
  int index = 0;
  const char decimal[] = "0123456789";
  char temp[11];
  int length;

  if (number == 0)
  {
    string[0] = '0';
    string[1] = '\0';
  }
  else
  {
    while (number != 0)
    {
      temp[index] = decimal[number % 10];
      number /= 10;
      index++;
    }
    
    length = index;
    
    for (index = 0; index < length; index++)
    {
      string[index] = temp[length - index - 1];
    }
    string[length] = 0;
  }
}

/* Convert a number to an hexadecimal string notation. Padded to eight
   digits with zereos. */

static void hex_string (char *string, unsigned int number)
{
  int index;
  const char hex[] = "0123456789ABCDEF";

  for (index = 0; index < 8; index++)
  {
    /* Ugly solution? I don't know. Rewrite it cleaner if you can. */
        
    string[index] = hex[(number >> (4 * (7 - index))) % 16];
  }
  string[8] = 0;
}

#endif

/* Prints the given data to the output string, a la printf and
   friends. */

return_type string_print (char *output, const char *format_string, ...)
{
  va_list arguments;
  return_type return_value;
  
  va_start (arguments, format_string);
  return_value = string_print_va (output, format_string, arguments);
  va_end (arguments);
  return return_value;
}

/* Print a string using the previously initialised va_list
   structure. */

return_type string_print_va (char *output, const char *format_string,
                             va_list arguments)
{
  int input_index = 0;
  int output_index = 0;
  int flags;
  int field_width;
  bool done;

  /* Minimum number of digits for integers; maximum number of chars
     for string. */

  int precision;		

  if (format_string == NULL)
  {
    output[output_index] = '\0';
    return STRING_RETURN_INVALID_ARGUMENT;
  }

  /* Main parser loop. */

  while (format_string[input_index] != '\0')
  {
    switch (format_string[input_index])
    {
      /* Percent sign means we get a formatting code as the next
         character. */

      case '%':
      {
        input_index++;
        flags = 0;
        done = FALSE;

        /* Now, some formatting stuff can be given. Let's parse
           it. */

        while (!done)
        {
          switch (format_string[input_index])
          {
            case '-':
            {
              flags |= LEFT;
              break;
            }
            
            case '+':
            {
              flags |= PLUS;
              break;
            }
            
            case ' ':
            {
              flags |= SPACE;
              break;
            }
            
            case '#':
            {
              flags |= SPECIAL;
              break;
            }

            case '0':
            {
              flags |= ZERO_PAD;
              break;
            }
            
            default:
            {
              done = TRUE;
              break;
            }
          }

          if (!done)
          {
            input_index++;
          }
        }

        /* Okay, time for a 'field width' mayhaps? */
        
        field_width = -1;
        if (format_string[input_index] >= '0' && 
            format_string[input_index] <= '9')
        {
          unsigned int characters;
          
          string_to_number (&format_string[input_index], &field_width,
                            &characters);
          input_index += characters;
        }

        /* Perhaps a 'precision' too? */

        precision = -1;
        if (format_string[input_index] == '.') 
        {
          input_index++;

          if (format_string[input_index] >= '0' &&
              format_string[input_index] <= '9')
          {
            unsigned int characters;

            string_to_number (&format_string[input_index], &precision,
                              &characters);
            input_index += characters;
          }

          if (precision < 0)
            precision = 0;
        }

        /* After this, we might get on or more 'l' characters. They
           can be ignored for now. */

        while (format_string[input_index] == 'l')
        {
          input_index++;
        }
        
        /* Now, find which type of print out this is. */

        switch (format_string[input_index])
        {
          /* String. */
          
          case 's':
          {
            char *temporary_string = va_arg (arguments, char *);
            int index = 0;
            int length = string_length (temporary_string);

            if (temporary_string == NULL)
            {
              temporary_string = (char *) "(null)";
            }            

            /* If this string should be right-padded, just do it. */

            if (!((flags & LEFT) == LEFT))
            {
              for (; length < field_width; field_width--)
              {
                output[output_index++] = ' ';
              }
            }

            while (temporary_string[index] != '\0')
            {
              output[output_index++] = temporary_string[index++];
            }

            /* Left-padded, just do it. */

            for (; length < field_width; field_width--)
            {
              output[output_index++] = ' ';
            }
            
            break;
          }
          
          /* Signed number. */
        
          case 'd':
          {
            char string[11];
            signed number = va_arg (arguments, signed);
            int index = 0;
            
            flags |= SIGN;
            number_to_string (string, number, 10, field_width, precision, 
                              flags);
            
            while (string[index] != '\0')
            {
              output[output_index++] = string[index++];
            }
            
            break;
          }
          
          /* Unsigned number. */
          
          case 'u':
          {
            char string[11];
            int index = 0;
            
            number_to_string (string, va_arg (arguments, unsigned), 10,
                              field_width, precision, flags);
            
            while (string[index] != '\0')
            {
              output[output_index++] = string[index++];
            }
            
            break;
          }
          
          /* Hexadecimal number. */
          
          case 'p':
          case 'X':
          {
            flags |= LARGE;
          }

          case 'x':
          {
            char string[9];
            int index = 0;
            
            number_to_string (string, va_arg (arguments, unsigned), 16,
                              field_width, precision, flags);
            
            while (string[index] != '\0')
            {
              output[output_index++] = string[index++];
            }
            
            break;
          }
          
          /* %% generates one percent sign. */

          case '%':
          {
            output[output_index++] = '%';;
            break;
          }

          /* Ignore anything else - it is simply unsupported. */

          default:
          {
            break;
          }
        }
        break;
      }
      
      /* All regular characters are just printed out. */
      
      default:
      {
        output[output_index++] = format_string[input_index];
        break;
      }
      
    }
    input_index++;
  }    

  output[output_index] = '\0';
  
  return STRING_RETURN_SUCCESS;
}

