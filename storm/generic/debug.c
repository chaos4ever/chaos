/* $Id$ */
/* Abstract: Functions used primarily when debugging the kernel. */
/* Authors: Per Lundberg <plundis@chaosdev.org> 
            Henrik Hallin <hal@chaosdev.org> */

/* Copyright 2000 chaos development. */
/* Copyright 2007 chaos development. */

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

#include <storm/current-arch/timer.h>
#include <storm/generic/avl_debug.h>
#include <storm/generic/dataarea.h>
#include <storm/generic/debug.h>
#include <storm/generic/defines.h>
#include <storm/generic/irq.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/multiboot.h>
#include <storm/generic/port.h>
#include <storm/generic/process.h>
#include <storm/generic/string.h>
#include <storm/generic/thread.h>
#include <storm/generic/types.h>
#include <config.h>

#include <stdarg.h>

#if !OPTION_RELEASE

/* Global variables. */

u32 debug_text_attribute = DEBUG_ATTRIBUTE_TEXT;
u32 debug_background_attribute = DEBUG_ATTRIBUTE_BACKGROUND;

/* Should debug messages go to the kernel log? (Not implemented fully
   yet..) */

bool debug_log_enable = FALSE;

/* Local variables. */

static u32 volatile x_position = 0;
static u32 volatile y_position = 0;
static debug_screen_type *screen = (debug_screen_type *) BASE_SCREEN;
static char input_string[DEBUG_MAX_INPUT + 1];

/* The kernel-level debugger uses a Swedish translation map. If you
   want to change it, you'll have to hack those tables. */

static const char keyboard_map[] =
{
  '\0', '\0', '1',  '2',  '3',  '4',  '5',  '6',
  '7',  '8',  '9',  '0',  '+',  '\0', '\0', '\0',
  'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
  'o',  'p',  '\0', '\0', '\0', '\0', 'a',  's',
  'd',  'f',  'g',  'h',  'j',  'k',  'l',  '\0',
  '\0', '\0', '\0', '\'', 'z',  'x',  'c',  'v',
  'b',  'n',  'm',  ',',  '.',  '-',  '\0', '*',
  '\0', ' ',  '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '-',  '\0', '\0', '\0', '+',  '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '<',  '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0'
};

static const char keyboard_map_shift[] =
{
  '\0', '\0', '!',  '\'', '#',  '$',  '%',  '&',
  '/',  '(',  ')',  '=',  '?',  '`',  '\0', '\0',
  'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
  'O',  'P',  '\0', '^',  '\0', '\0', 'A',  'S',
  'D',  'F',  'G',  'H',  'J',  'K',  'L',  '\0',
  '\0', '\0', '\0', '*',  'Z',  'X',  'C',  'V',
  'B',  'N',  'M',  ';',  ':',  '_',  '\0', '*',
  '\0', ' ',  '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '-',  '\0', '\0', '\0', '+',  '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '>',  '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0'
};

static const char keyboard_map_altgr[] =
{
  '\0', '\0', '\0', '@',  '\0', '$',  '\0', '\0',
  '{',  '[',  ']',  '}',  '\\', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '~',  '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', ' ',  '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '|',  '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0'
};

static unsigned int shift_state = 0;
static char command_arguments[512];
static char *argument_start[16];
static debug_arguments_type parsed_command =
{
  0, argument_start, (char *) &command_arguments 
};

static void (*command_pointer)(int number_of_arguments, char **argument);

static void command_dump (int arguments, char **argument);
static void command_help (int arguments, char **argument);
static void command_reboot (int arguments, char **argument) __attribute__ ((noreturn));
static void command_sanity (int arguments, char **argument);

/* Commandos available in the debugger. */

static debug_command_type command[] =
{
  { "?", "", "Display help about available commands.", command_help },
  { "help", "", "Display help about available commands.", command_help },
  { "dump", "BASE [LENGTH]", "Dump memory, starting at BASE, LENGTH dwords.",
    command_dump },
  { "quit", "", "Abort the debugger (and possibly crash).",
    NULL },
  { "reboot", "", "Reboot the computer.", command_reboot },
  { "sanity", "", "Perform some basic sanity checks of the system.", 
    command_sanity },
};

static int number_of_commands = (sizeof (command) / 
                                 sizeof (debug_command_type));

/* Dump some contents of memory. */

void debug_memory_dump (u32 *memory, u32 length)
{
  unsigned int index;

  for (index = 0; index < length; index++)
  {
    if ((index % 8) == 0)
    {
      debug_print ("\n");
    }

    debug_print ("%x ", memory[index]);
  }

  debug_print ("\n");
}

/* Dump memory. */

void command_dump (int number_of_arguments, char **argument)
{
  int length;
  int base;

  if (number_of_arguments != 2 &&
      number_of_arguments != 3)
  {
    debug_print ("Invalid number of arguments. Try 'help %s'.\n", argument[0]);
    return;
  }

  string_to_number (argument[1], &base);

  if (number_of_arguments == 3)
  {
    string_to_number (argument[2], &length);
  }
  else
  {
    length = 1;
  }

  debug_print ("%u %s\n", length, argument[2]);
  debug_memory_dump ((void *) base, length);
}

/* Show a list of the available commands, with a short description */

void command_help (int number_of_arguments, char **argument)
{
   int counter;

  if (number_of_arguments == 2)
  {
    for (counter = 0; counter < number_of_commands; counter++)
    {
      if (string_compare (command[counter].name, argument[1]) == 0)
      {
        debug_print ("SYNOPSIS: %s %s\n\n  DESCRIPTION: %s\n",
                     command[counter].name, command[counter].arguments, 
                     command[counter].description);
        break;
      }
    }
    
    if (counter == number_of_commands)
    {
      debug_print ("Unknown command. Try 'help'.\n");
    }
  }
  else
  {
    debug_print ("Available commands. (try 'help command' for help about a specific command)\n\n");

    for (counter = 0; counter < number_of_commands; counter++)
    {
      debug_print ("  %s\n", command[counter].name);
    }
  }
}

/* Reboot the computer. */

void command_reboot (int number_of_arguments __attribute__ ((unused)),
                     char **argument __attribute__ ((unused)))
{
  cpu_reset ();
}

/* Sanity check. */

void command_sanity (int number_of_arguments __attribute__ ((unused)),
                     char **argument __attribute__ ((unused)))
{
  bool okay = TRUE;

  /* Here, we check everything that could have been broken. */

  avl_debug_tree_check (page_avl_header, page_avl_header->root);
  avl_debug_tree_check (global_avl_header, global_avl_header->root);
  if (tss_tree_mutex == MUTEX_LOCKED)
  {
    debug_print ("TSS tree mutex was locked.");
    okay = FALSE;
  }

  if (okay)
  {
    debug_print ("The system seems basically okay.\n");
  }
}

/* Initalize the debug system. */

void debug_init (void)
{
  int counter;

  x_position = dataarea.x_position;
  y_position = dataarea.y_position;

  for (counter = 0; counter < dataarea.x_size * dataarea.y_size; counter++)
  {
    screen[counter].attribute = debug_background_attribute;
  }
}

/* FIXME: This function isn't really generic. Move somewhere
   else... */

/* Move the cursor to the given position. Only moves the physical
   cursor; x_position and y_position is untouched. */

static void place_cursor (unsigned int x, unsigned int y)
{
  u16 c;

  if (x < dataarea.x_size && y < dataarea.y_size)
  {
    c = y * dataarea.x_size + x;

    /* Cursor position high. */

    port_out_u8 (0x3D4, 0x0E);                  
    port_out_u8 (0x3D5, c >> 8);

    /* Cursor position lo. */

    port_out_u8 (0x3D4, 0x0F);
    port_out_u8 (0x3D5, c);
  }
}

/* Put a character at the given position. */

static inline void put_character (int x, int y, int character,
                                  int character_attribute)
{
  screen[((y * dataarea.x_size) + x)].character = character;
  screen[((y * dataarea.x_size) + x)].attribute = character_attribute;
}

/* Basic printing function. */

static int print_simple (const char *string)
{
  u32 index;

  /* Handle the NULL string. */

  if (string == NULL)
  {
    debug_print_simple ("(null)");
    return 0;
  }

  for (index = 0; string[index] != '\0'; index++)
  {
    put_character (x_position, y_position, string[index],
                   debug_text_attribute);
    x_position++;

    if (x_position == dataarea.x_size)
    {
      x_position = 0;
      y_position++;

      /* If we're passing the last line on screen, scroll everything
         upwards one line. */
  
      if (y_position >= dataarea.y_size)
      {
        y_position  = dataarea.y_size - 1;
        memory_copy ((void *) screen, (void *) &screen[dataarea.x_size],
                     (dataarea.x_size * (dataarea.y_size - 1)) * 2);
        memory_set_u16 ((void *) &screen[dataarea.x_size * (dataarea.y_size - 1)],
                        debug_background_attribute * 256 + ' ', dataarea.x_size);
      }
    }
  }

  return index;
}

/* Print a string to screen with no parsing at all. */
/* FIXME: Create a print_simple which doesn't move the cursor. */

void debug_print_simple (const char *string)
{
  //  cpu_interrupts_disable ();

  /* If we're trying to write the NULL string, screw ourselves,
     hippies. */

  if (string == NULL)
  {
    print_simple ("(null)");
    //    cpu_interrupts_enable ();
    return;
  }

  debug_print (string);

  x_position = 0;
  y_position++;

  /* If we're passing the last line on screen, scroll everything
     upwards one line. */
  
  if (y_position >= dataarea.y_size)
  {
    y_position  = dataarea.y_size - 1;
    memory_copy ((void *) screen, (void *) &screen[dataarea.x_size],
                 (dataarea.x_size * (dataarea.y_size - 1)) * 2);
    memory_set_u16 ((void *) &screen[dataarea.x_size * (dataarea.y_size - 1)],
                    debug_background_attribute * 256 + ' ', dataarea.x_size);
  }
  
  place_cursor (x_position, y_position);
  //  cpu_interrupts_enable ();
}

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

/* Print a formatted string to screen. Only used for debugging. This
   is NOT a POSIX compliant function. */

void debug_print (const char *format_string, ...)
{
  va_list va_arguments;
  int index = 0;

  //  cpu_interrupts_disable ();
  va_start (va_arguments, format_string);

  if (format_string == NULL)
  {
    print_simple ("debug_print: format_string == NULL.\n");
    //    cpu_interrupts_enable ();
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
            char character = (char) va_arg (va_arguments, int);

            put_character (x_position, y_position, character,
                           debug_text_attribute);
            
            /* Move the cursor. */
            
            x_position++;
            
            if (x_position >= dataarea.x_size)
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
                           debug_text_attribute);
            
            /* Move the cursor. */
            
            x_position++;
            
            if (x_position >= dataarea.x_size)
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
                       debug_text_attribute);

        /* Move the cursor. */

        x_position++;

        if (x_position >= dataarea.x_size)
        {
          y_position++;
          x_position = 0;
        }
      }
    }

    /* If we're passing the last line on screen, scroll everything
       upwards one line. */

    if (y_position == dataarea.y_size)
    {
      y_position--;
      memory_copy ((void *) screen, (void *) &screen[dataarea.x_size],
                   (dataarea.x_size * (dataarea.y_size - 1)) * 2);
      memory_set_u16 ((void *) &screen[dataarea.x_size * (dataarea.y_size - 1)],
                      debug_background_attribute * 256 + ' ', dataarea.x_size);
    }

    index++;
  }

  place_cursor (x_position, y_position);

  va_end (va_arguments);
  //  cpu_interrupts_enable ();
}

#ifdef NO_CRASH_SPLASH

/* Display a bogus crash screen, if we prefer that. */

void debug_crash_screen (const char *message,
                         volatile storm_tss_type *dump_tss)
{
  debug_print ("Kernel:\n");
  debug_print ("  Alert: %s!\n", message);

  debug_print ("Causing process:\n");

  debug_print ("  Process: %s (PROCESS ID %u).\n",
               ((process_info_type *) dump_tss-> process_info)->name,
               dump_tss->process_id);
  debug_print ("  Thread: %s (THREAD ID %u).\n",
               dump_tss->thread_name, dump_tss->thread_id);
  debug_print ("  Process was dispatched %u times.\n", dump_tss->timeslices);
  debug_print ("  Exception occured at address %x:%x\n",
               dump_tss->cs, dump_tss->eip);
  debug_print ("Registers:\n");

  debug_print ("  EAX: 0x%x EBX: 0x%x ECX: 0x%x EDX: 0x%x\n",
               dump_tss->eax, dump_tss->ebx, dump_tss->ecx, dump_tss->edx);
  debug_print ("  ESP: 0x%x EBP: 0x%x ESI: 0x%x EDI: 0x%x\n",
               dump_tss->esp, dump_tss->ebp, dump_tss->esi, dump_tss->edi);
  debug_print ("   DS: 0x%x  ES: 0x%x  FS: 0x%x  GS: 0x%x\n",
               dump_tss->ds, dump_tss->es, dump_tss->fs, dump_tss->gs);
  debug_print ("  CR2: 0x%x CR3: 0x%x              EFLAGS: 0x%x\n",
               cpu_get_cr2 (), dump_tss->cr3, dump_tss->eflags);
}

#else

/* Display a real crasch screen, including memory and process information. */

void debug_crash_screen (const char *message,
                         volatile storm_tss_type *dump_tss)
{
  u32 saved_attribute;
  unsigned int seconds = timeslice / hz;
  //  u16 page_directory_entry, page_table_entry;
  //  pagedirectory_entry_pagetable *page_directory;
  //  pagetable_entry *page_table;

  memory_set_u16 ((void *) screen, DEBUG_ATTRIBUTE_CRASH * 256 + ' ',
                  dataarea.x_size * dataarea.y_size);
  place_cursor (0, 0);
  x_position = 0;
  y_position = 0;
  
  saved_attribute = debug_text_attribute;
  debug_text_attribute = DEBUG_ATTRIBUTE_CRASH;
  debug_print ("Kernel:\n");

  debug_print ("  Alert: %s!\n", message);
  debug_print ("  Version: %s %s.\n", PACKAGE, VERSION);
  debug_print ("  Uptime: %u timeslices (%u h, %u m and %u s).\n",
               timeslice, seconds / (60 * 60), (seconds / 60) % 60,
               seconds % 60);

  debug_print ("Memory:\n");

  debug_print ("  Physical memory: %u KByte used. %u KByte free.\n",
               memory_physical_get_used () * 4,
               memory_physical_get_free () * 4);
  debug_print ("  Global memory: %u KByte used. %u KByte free.\n",
               memory_global_get_used () / 1024,
               memory_global_get_free () / 1024);

  debug_print ("Causing process:\n");

  debug_print ("  Process: %s (PROCESS ID %u).\n",
               dump_tss->process_name, dump_tss->process_id);
  debug_print ("  Thread: %s (THREAD ID %u).\n",
               dump_tss->thread_name, dump_tss->thread_id);
  debug_print ("  Process was dispatched %u times.\n", dump_tss->timeslices);
  debug_print ("  Exception occured at address %x:%x\n",
               dump_tss->cs, dump_tss->eip);

  debug_print ("Registers:\n");

  debug_print ("  EAX: 0x%x EBX: 0x%x ECX: 0x%x EDX: 0x%x\n",
               dump_tss->eax, dump_tss->ebx, dump_tss->ecx, dump_tss->edx);
  debug_print ("  ESP: 0x%x EBP: 0x%x ESI: 0x%x EDI: 0x%x\n",
               dump_tss->esp, dump_tss->ebp, dump_tss->esi, dump_tss->edi);
  debug_print ("   DS: 0x%x  ES: 0x%x  FS: 0x%x  GS: 0x%x\n",
               dump_tss->ds, dump_tss->es, dump_tss->fs, dump_tss->gs);
  debug_print ("  CR2: 0x%x CR3: 0x%x              EFLAGS: 0x%x\n",
               cpu_get_cr2 (), dump_tss->cr3, dump_tss->eflags);

#if FALSE
  debug_print ("Stack:\n");

  if (memory_mutex == MUTEX_UNLOCKED)
  {
    debug_print ("  %u KByte of stack. Dumping from ESP:\n", 
                 dump_tss->stack_pages * 4);

    /* Map the process' page directory. */
    
    page_directory_entry = dump_tss->esp / (SIZE_PAGE * 1024);
    page_table_entry = (dump_tss->esp / SIZE_PAGE) % 1024;
    debug_print ("%x %x\n", page_directory_entry, page_table_entry);
    memory_virtual_map (GET_PAGE_NUMBER (BASE_PROCESS_TEMPORARY),
                        dump_tss->cr3, 1, PAGE_KERNEL);
    page_directory = (pagedirectory_entry_pagetable *) BASE_PROCESS_TEMPORARY;
    memory_virtual_map (GET_PAGE_NUMBER (BASE_PROCESS_TEMPORARY) + 1,
                        page_directory[page_directory_entry].pagetable_base, 1,
                        PAGE_KERNEL);
    page_table = (pagetable_entry *) BASE_PROCESS_TEMPORARY + SIZE_PAGE;
    memory_virtual_map (GET_PAGE_NUMBER (BASE_PROCESS_TEMPORARY),
                        page_table[page_table_entry].page_base, 1,
                        PAGE_KERNEL);
                        
    debug_memory_dump ((u32 *) BASE_PROCESS_TEMPORARY +
                       dump_tss->esp % SIZE_PAGE, SIZE_PAGE -
                       (dump_tss->esp % SIZE_PAGE) < 20 * 4 ?
                       (SIZE_PAGE - (dump_tss->esp % SIZE_PAGE) / 4) :
                       20);
  }
  else
  {
    debug_print ("  Couldn't dump stack since memory mutex was locked.\n");
  }
#endif
   
  debug_text_attribute = saved_attribute;
}

#endif

/* Parse an input line. */

static int debug_line_parse (char *source, 
                             debug_arguments_type *destination)
{
  int number_of_arguments = 0, position, argument, length;
  char *word;

  /* First of all, find out how many arguments we have. */

  position = 0;

  while (source[position] != '\0')
  {
    if (source[position] == '"')
    {
      do
      {
        position++;
      } while (source[position] != '\0' &&
               source[position] != '"');

      /* The string is broken. */

      if (source[position] == '\0')
      {
        return 0;
      }
      position++;
      number_of_arguments++;
    }
    else if (source[position] != ' ')
    {
      number_of_arguments++;

      while (source[position] != ' ' && source[position] != 0)
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
  
  destination->number_of_arguments = number_of_arguments;

  /* Now, go through the string again and copy the parameters into the
     data area and set the pointers correctly. */

  word = destination->arguments;

  /* Okay, the initial pointers are setup. */

  for (position = 0, argument = 0; source[position]; position++)
  {
    while (source[position] == ' ')
    {
      position++;
    }

    if (source[position] == '"')
    {
      /* Find out the length of the string. */

      position++;
      length = 0;

      while (source[position + length] != '"' &&
             source[position + length] != '\0')
      {
        length++;
      }

      string_copy_max (word, &source[position], length);

      position += length - 1;
      word[length] = 0;
      destination->argument_start[argument] = word;
      word += length + 1;
      argument++;
    }
    else if (source[position] != '\0')
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
      destination->argument_start[argument] = word;
      word += length + 1;
      argument++;
    }
    else
    {
      break;
    }
  }

  return number_of_arguments;
}

/* Run the kernel-level debugger. */
/* FIXME: This is not really generic... */

void debug_run (void)
{
  u8 character_code;
  char character;
  unsigned int where;
  unsigned int words;
  u32 esp = cpu_get_esp ();
  
  debug_print ("Stack pointer: %x\n", esp);

  while (TRUE)
  {
    debug_print ("\n(sdb) ");
    character = '\0';
    where = 0;

    while (character != '\n')
    {
      /* Wait for input. */
      
      while ((port_in_u8 (DEBUG_KEYBOARD_STATUS) & 
              DEBUG_KEYBOARD_INPUT_FULL) == 0);
      
      character_code = port_in_u8 (DEBUG_KEYBOARD_DATA);

      /* Is this a 'release' scan code or a regular key press? */
      
      if (character_code < 0x80)
      {
        switch (character_code)
        {
          case DEBUG_SCAN_CODE_LEFT_SHIFT:
          case DEBUG_SCAN_CODE_RIGHT_SHIFT:
          {
            shift_state |= DEBUG_SHIFT;
            break;
          }

          case DEBUG_SCAN_CODE_ALT:
          {
            shift_state |= DEBUG_ALT;
            break;
          }

          case DEBUG_SCAN_CODE_ENTER:
          {
            character = '\n';
            break;
          }

          case DEBUG_SCAN_CODE_BACK_SPACE:
          {
            if (where > 0)
            {
              where--;
              input_string[where] = '\0';
              x_position--;
              put_character (x_position, y_position, ' ',
                             debug_text_attribute);
              place_cursor (x_position, y_position);
            }
            break;
          }

          /* Anything else is a regular key. */

          default:
          {
            if ((shift_state & DEBUG_SHIFT) == DEBUG_SHIFT)
            {
              character = keyboard_map_shift[character_code];
            }
            else if ((shift_state & DEBUG_ALT) == DEBUG_ALT)
            {
              character = keyboard_map_altgr[character_code];
            }
            else
            {
              character = keyboard_map[character_code];
            }
            
            if (character != '\0')
            {
              input_string[where] = character;
              where++;
              debug_print ("%c", character);
            }

            break;
          }
        }       
      }
      else
      {
        character_code &= 0x7F;

        switch (character_code)
        {
          case DEBUG_SCAN_CODE_LEFT_SHIFT:
          case DEBUG_SCAN_CODE_RIGHT_SHIFT:
          {
            shift_state &= ~DEBUG_SHIFT;
            break;
          }

          case DEBUG_SCAN_CODE_ALT:
          {
            shift_state &= ~DEBUG_ALT;
            break;
          }
        }
      }
    }

    input_string[where] = '\0';

    /* Now, parse this command line... */
    
    memory_set_u8 ((u8 *) parsed_command.arguments, 0, 512);
    words = debug_line_parse (input_string, &parsed_command);

    if (words > 0)
    {
      int counter;
      char *first_word = parsed_command.argument_start[0];

      for (counter = 0; counter < number_of_commands; counter++)
      {
        if (!string_compare (first_word, command[counter].name))
        {
          /* Special case for handling the 'quit' command. */

          if (command[counter].pointer == NULL)
          {
            return;
          }

          debug_print ("\n");
          command_pointer = command[counter].pointer;
          command_pointer (parsed_command.number_of_arguments,
                           parsed_command.argument_start);
          debug_print ("\n");
          break;
        }
      }
      
      if (counter == number_of_commands)
      {
        debug_print ("\nUnknown command. Try '?' or 'help' for help.\n\n");
      }
    }
  }
}

#else /* OPTION_RELEASE */

void debug_print_simple (char *string __attribute__ ((unused)))
{
}

#endif /* !OPTION_RELEASE */
