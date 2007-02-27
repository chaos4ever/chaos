/* $Id$ */
/* Abstract: Functions used primarily when debugging the kernel. */
/* Authors: Per Lundberg <plundis@chaosdev.org> 
            Henrik Hallin <hal@chaosdev.org> */

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

#include <storm/generic/defines.h>
#include "config.h"

#if !OPTION_RELEASE

#include <storm/generic/debug.h>
#include <storm/generic/types.h>
#include <storm/generic/memory.h>

#include <stdarg.h>

static u32 text_attribute = DEBUG_ATTRIBUTE_TEXT;
static u32 background_attribute = DEBUG_ATTRIBUTE_BACKGROUND;
static u32 volatile x_position = 0;
static u32 volatile y_position = 0;
debug_screen_type *screen = (debug_screen_type *) BASE_SCREEN;

void debug_init (void)
{
  /* Clear the screen. */

  memory_set_u16 ((u16 *) screen, background_attribute * 256 + ' ',
                  DEBUG_SCREEN_WIDTH * DEBUG_SCREEN_HEIGHT *
                  sizeof (debug_screen_type));
}

/* Put a character at the given position. */

static inline void put_character (int x, int y, char character,
                                  int character_attribute)
{
  int index = (y * DEBUG_SCREEN_WIDTH) + x;

  screen[index].character = character;
  screen[index].attribute = character_attribute;
}

/* Basic printing function. */

static int print_simple (const char *string)
{
  u32 index;

  /* Handle the NULL string. */

  if (string == NULL)
  {
    print_simple ("(null)");
    return 0;
  }

  for (index = 0; string[index] != '\0'; index++)
  {
    put_character (x_position, y_position, string[index],
                   text_attribute);
    x_position++;

    if (x_position == DEBUG_SCREEN_WIDTH)
    {
      x_position = 0;
      y_position++;

      /* If we're passing the last line on screen, scroll everything
         upwards one line. */
  
      if (y_position >= DEBUG_SCREEN_HEIGHT)
      {
        y_position = DEBUG_SCREEN_HEIGHT - 1;
        memory_copy ((void *) screen, (void *) &screen[DEBUG_SCREEN_WIDTH],
                     (DEBUG_SCREEN_WIDTH * (DEBUG_SCREEN_HEIGHT - 1)) * 2);
        memory_set_u16 ((void *) &screen[DEBUG_SCREEN_WIDTH *
                                         (DEBUG_SCREEN_HEIGHT - 1)],
                        background_attribute * 256 + ' ', DEBUG_SCREEN_WIDTH);
      }
    }
  }

  return index;
}

/* Convert a decimal number to a string. */

static void decimal_string (char *string, int number)
{
  int index = 0;
  const char decimal_character[] = "0123456789";
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
      temp[index] = decimal_character[number % 10];
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
   digits with zeroes. */

static void hex_string (char *string, unsigned int number)
{
  int index;
  const char hex_character[] = "0123456789ABCDEF";

  for (index = 0; index < 8; index++)
  {
    /* Ugly solution? I don't know. Rewrite it cleaner if you can. */
        
    string[index] = hex_character[(number >> (4 * (7 - index))) % 16];
  }
  string[8] = 0;
}

/* Print a formatted string to screen. Only used for debugging. This
   is NOT a POSIX compliant function. */

void debug_print (const char *format_string, ...)
{
  va_list va_arguments;
  int index = 0;

  va_start (va_arguments, format_string);

  if (format_string == NULL)
  {
    print_simple ("debug_print: format_string == NULL.\n");
    return;
  }

  /* Main parser loop. */

  while (format_string[index] != 0)
  {
    switch (format_string[index])
    {
      /* Percent sign means we get a formatting code as the next
         character. */

      case '%':
      {
        index++;

        switch (format_string[index])
        {
          /* Character. */

          case 'c':
          {
            char character = va_arg (va_arguments, char);

            put_character (x_position, y_position, character,
                           text_attribute);
            
            /* Move the cursor. */
            
            x_position++;
            
            if (x_position >= DEBUG_SCREEN_WIDTH)
            {
              y_position++;
              x_position = 0;
            }

            break;
          }

          /* String. */
          
          case 's':
          {
            print_simple (va_arg (va_arguments, char *));

            break;
          }
          
          /* Signed number. */

          case 'd':
          {
            char string[11];
            signed number = va_arg (va_arguments, signed);

            /* If the number is negative, print it with a hyphen before. */
            
            if (number < 0)
            {
              string[0] = '-';
              decimal_string (string + 1, -number);
            }
            else
            {
              decimal_string (string, number);
            }

            print_simple (string);
            break;
          }

          /* Unsigned number. */

          case 'u':
          {
            char string[11];

            decimal_string (string, va_arg (va_arguments, unsigned));
            print_simple (string);

            break;
          }

          /* Hexadecimal number. Padded to eight characters with
             zeroes. */

          case 'p':
          case 'x':
          {
            char string[9];

            hex_string (string, va_arg (va_arguments, unsigned));
            print_simple (string);

            break;
          }

          /* Anything other is printed out in plain. */

          default:
          {
            put_character (x_position, y_position, format_string[index],
                           text_attribute);
            
            /* Move the cursor. */
            
            x_position++;
            
            if (x_position >= DEBUG_SCREEN_WIDTH)
            {
              y_position++;
              x_position = 0;
            }

            break;
          }
        }
        break;
      }

      /* New line. */

      case '\n':
      {
        y_position++;
        x_position = 0; 

        break;
      }

      /* All regular characters are just printed out. */

      default:
      {
        put_character (x_position, y_position, format_string[index],
                       text_attribute);

        /* Move the cursor. */

        x_position++;

        if (x_position >= DEBUG_SCREEN_WIDTH)
        {
          y_position++;
          x_position = 0;
        }
      }
    }

    /* If we're passing the last line on screen, scroll everything
       upwards one line. */

    if (y_position == DEBUG_SCREEN_HEIGHT)
    {
      y_position--;
      memory_copy ((void *) screen, (void *) &screen[DEBUG_SCREEN_WIDTH],
                   (DEBUG_SCREEN_WIDTH * (DEBUG_SCREEN_HEIGHT - 1)) * 2);
      memory_set_u16 ((void *) &screen[DEBUG_SCREEN_WIDTH *
                                       (DEBUG_SCREEN_HEIGHT - 1)],
                      background_attribute * 256 + ' ', DEBUG_SCREEN_WIDTH);
    }

    index++;
  }

  va_end (va_arguments);
}

/* Display a bogus crash screen, if we prefer that. */

void debug_crash_screen (const char *message, thread_type *thread)
{
  debug_print ("Alert: %s!\n", message);

  debug_print ("Causing process:\n");

  debug_print ("  Process: %s (ID %u).\n",
               ((process_type *) thread->process)->name,
               thread->process->id);
  debug_print ("  Thread: %s (ID %u).\n",
               thread->name, thread->id);
  debug_print ("  Process was dispatched %u times.\n", thread->timeslices);
  debug_print ("  Exception occured at address %x:%x\n",
               thread->cpu_task->cs, thread->cpu_task->eip);

  debug_print ("Registers:\n");
  debug_print ("  EAX: 0x%x EBX: 0x%x ECX: 0x%x EDX: 0x%x\n",
               thread->cpu_task->eax, thread->cpu_task->ebx,
               thread->cpu_task->ecx, thread->cpu_task->edx);
  debug_print ("  ESP: 0x%x EBP: 0x%x ESI: 0x%x EDI: 0x%x\n",
               thread->cpu_task->esp, thread->cpu_task->ebp,
               thread->cpu_task->esi, thread->cpu_task->edi);
  debug_print ("   DS: 0x%x  ES: 0x%x  FS: 0x%x  GS: 0x%x\n",
               thread->cpu_task->ds, thread->cpu_task->es,
               thread->cpu_task->fs, thread->cpu_task->gs);
  debug_print ("  CR2: 0x%x CR3: 0x%x              EFLAGS: 0x%x\n",
               cpu_get_cr2 (), thread->cpu_task->cr3,
               thread->cpu_task->eflags);
}

#endif /* !OPTION_RELEASE */
