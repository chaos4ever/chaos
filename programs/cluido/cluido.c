/* $Id$ */
/* Abstract: cluido is the Command Line User Interface (CLUI) of the
   chaos operating system. It serves a somewhat similar function as
   the AmigaOS CLI, COMMAND.COM in DOS, CMD.EXE in OS/2 and Windows
   NT, or the Bourne Again SHell in GNU systems. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

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
   USA. */

#include "cluido.h"
#include "list.h"

/* Define to get debug information. */

#undef DEBUG

char working_directory[256] = "//";
char user_name[50];
char host_name[IPV4_HOST_NAME_LENGTH] = "localhost";
environment_type environment[16] = 
{
  {
    { "prompt" },
    { "\\e[1;37m\\u\\e[0;37m@\\e[1;37m\\h\\e[0;37m [\\e[1;37m\\w\\e[0;37m]\\e[1;33m " }
  },
  {
    { "prefix" },
    { "\\e[0;37m" }
  },
  { 
    { "postfix" },
    { "" }
  },
  {
    { '\0' },
    { '\0' }
  }
};

log_structure_type log_structure;

static void (*command_pointer)(int number_of_arguments, char **argument);

static list_type *command_list_root = NULL;
static char *argument_start[16];
static char arguments[512];
static arguments_type parsed_command =
{
  0, argument_start, (char *) &arguments 
};

ipc_structure_type vfs_structure;

/* Link a new command into the history list. */

static void command_list_link (void *data)
{
  list_type *new_node;

  memory_allocate ((void **) &new_node, sizeof (list_type));

  if (command_list_root == NULL)
  {
    command_list_root = new_node;
    new_node->previous = new_node->next = NULL;
  }
  else
  {
    command_list_root->previous = (struct list_type *) new_node;
    new_node->next = (struct list_type *) command_list_root;
    command_list_root = new_node;
    new_node->previous = NULL;
  }

  new_node->data = data;
}

/* Remove a previously typed command from the history list. */

static void command_list_unlink (list_type *node)
{
  memory_deallocate ((void **) &node->data);

  if (node->next != NULL)
  {
    ((list_type *) node->next)->previous = node->previous;
  }

  if (node->previous != NULL)
  {
    ((list_type *) node->previous)->next = node->next;
  }

  if (command_list_root == node)
  {
    command_list_root = (list_type *) node->next;
  }

  memory_deallocate ((void **) &node);
}

/* Remove empty entries from the command history list. */

static void command_list_cleanup (void)
{
  list_type *node = command_list_root;
  list_type *next_node;
  unsigned int non_space_characters, search_position;
  char *string;

  while (node != NULL)
  {
    search_position = non_space_characters = 0;
    string = node->data;

    while (string[search_position] != '\0' &&
           non_space_characters == 0)
    {
      if (string[search_position] != ' ')
      {
        non_space_characters++;
      }
      search_position++;
    }

    if (non_space_characters == 0)
    {
      next_node = (list_type *) node->next;
      command_list_unlink (node);
      node = next_node;
    }
    else
    {
      node = (list_type *) node->next;
    }
  }
}

/* Get the value for the given environment variable. Returns NULL if
   not found. */

char *environment_get (char *key)
{
  int index;

  for (index = 0; index < 16 && environment[index].key[0] != '\0'; index++)
  {
    if (string_compare (key, environment[index].key) == 0)
    {
      break;
    }
  }

  /* Did we fall out? */

  if (index == 16 || environment[index].key[0] == '\0')
  {
    return NULL;
  }
  else
  {
    return environment[index].value;
  } 
}

/* Parse command arguments. */

static int arguments_parse (char *source, arguments_type *destination)
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

/* Update the currently edited text on screen. This one also sets the
   cursor correctly. */

static void string_input_update (char *string, unsigned int cursor_position,
                                 bool string_shrinks)
{
  char temporary;

  console_print_formatted (&console_structure, "\e[u%s%s", string,
                           string_shrinks ? " " : "");
  temporary = string[cursor_position];
  string[cursor_position] = '\0';
  console_print_formatted (&console_structure, "\e[u%s", string);
  string[cursor_position] = temporary;
}

/* Get a whole line of input from the user. */

static void string_input
  (unsigned int max_length, bool do_syntax_check, bool command_list,
   char *buffer)
{
  unsigned int input_string_position = 0;
  unsigned int input_string_length = 0;
  unsigned int input_string_characters = 0;
  keyboard_packet_type keyboard_packet;
  int event_type;
  char *temporary_string;
  list_type *command_list_pointer;
  char *string;

  /* Save cursor position. */

  console_print (&console_structure, "\e[s");

  memory_allocate ((void **) &temporary_string, max_length);

  if (command_list)
  {
    memory_allocate ((void **) &string, max_length);
    command_list_link (string);
    command_list_pointer = command_list_root;
  }
  else
  {
    command_list_pointer = NULL;
  }
  
  /* Allocate memory for scratchpad. */

  memory_allocate ((void **) &string, max_length);

  while (!(keyboard_packet.key_pressed &&
           keyboard_packet.has_special_key &&
           keyboard_packet.special_key == IPC_KEYBOARD_SPECIAL_KEY_ENTER))
  {
    console_event_wait (&console_structure, &keyboard_packet, 
                        &event_type, TRUE);
    
    /* Was the event caused by the keyboard? */

    if (event_type == CONSOLE_EVENT_KEYBOARD &&
        keyboard_packet.key_pressed)
    {
      if (keyboard_packet.has_character_code)
      {
        if (keyboard_packet.right_control_down ||
            keyboard_packet.left_control_down)
        {
          switch (keyboard_packet.character_code[0])
          {
            /* Beginning of line. */

            case 'a':
            case 'A':
            {
              input_string_position = 0;
              string_input_update (string, input_string_position, FALSE);
              break;
            }

            /* Previous character. */

            case 'b':
            case 'B':
            {
              if (input_string_position > 0)
              {
                input_string_position -=
                  unicode_utf8_previous_character_length 
                    (string, input_string_position);
                string_input_update (string, input_string_position, FALSE);
              }
              break;
            }
            
            /* Delete the current character. */
 
            case 'd':
            case 'D':
            {
              if (input_string_position != input_string_length)
              {
                unsigned int character_length =
                  unicode_utf8_next_character_length (string,
                                                 input_string_position);
                unsigned int new_position = (input_string_position +
                                             character_length);
                
                string_copy (&string[input_string_position],
                             &string[new_position]);
                
                input_string_length -= character_length;
                input_string_characters--;

                string_input_update (string, input_string_position, TRUE);
              }
              break;
            }

            /* End of line. */
 
            case 'e':
            case 'E':
            {
              input_string_position = input_string_length;
              string_input_update (string, input_string_position, FALSE);
              break;
            }

            /* Next character. */

            case 'f':
            case 'F':
            {
              if (input_string_position < input_string_length)
              {
                input_string_position +=
                  unicode_utf8_next_character_length
                    (string, input_string_position);
                string_input_update (string, input_string_position, FALSE);
              }
              break;
            }

            /* Delete the line. */

            case 'u':
            case 'U':
            {
              unsigned int index;

              for (index = 0; index < input_string_characters; index++)
              {
                temporary_string[index] = ' ';
              }
              temporary_string[input_string_characters] = '\0';

              input_string_length = 0;
              input_string_position = 0;
              input_string_characters = 0;
              string[0] = '\0';

              string_input_update (temporary_string, 0, FALSE);
              string_input_update (string, input_string_position, FALSE);
              break;
            }
          }
        }

        /* Regular printable characters. */
        
        else if (input_string_length +
                 string_length (keyboard_packet.character_code) <
                 max_length)
        {
          string_copy (temporary_string, &string[input_string_position]);
          string_copy (&string[input_string_position], 
                       keyboard_packet.character_code);
          input_string_position +=
            string_length (keyboard_packet.character_code);
          string_copy (&string[input_string_position],
                       temporary_string);
          input_string_length +=
            string_length (keyboard_packet.character_code);
          input_string_characters++;

          string_input_update (string, input_string_position, FALSE);
        }
      }

      /* Non-printable characters. */

      else
      {
        switch (keyboard_packet.special_key)
        {
          /* Backspace. */
          
          case IPC_KEYBOARD_SPECIAL_KEY_BACK_SPACE: 
          {
            if (input_string_position > 0)
            {
              unsigned int character_length =
                unicode_utf8_previous_character_length
                  (string, input_string_position);
              unsigned int new_position = (input_string_position -
                                           character_length);

              string_copy (&string[new_position],
                           &string[input_string_position]);
              input_string_position = new_position;

              input_string_length -= character_length;
              input_string_characters--;

              string_input_update (string, input_string_position, TRUE);
            }
            break;
          }

          /* Delete. */
          
          case IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_DELETE:
          {
            if (input_string_position != input_string_length)
            {
              unsigned int character_length =
                unicode_utf8_next_character_length
                  (string, input_string_position);
              unsigned int new_position = (input_string_position +
                                           character_length);
              
              string_copy (&string[input_string_position],
                           &string[new_position]);
              
              input_string_length -= character_length;
              input_string_characters--;

              string_input_update (string, input_string_position, TRUE);
            }
            break;
          }
          
          /* Home. */
          
          case IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_7:
          {
            input_string_position = 0;
            string_input_update (string, input_string_position, FALSE);
            
            break;
          }
          
          /* End. */
          
          case IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_1:
          {
            input_string_position = input_string_length;
            string_input_update (string, input_string_position, FALSE);
            
            break;
          }
          
          /* Tab. */
          
          case IPC_KEYBOARD_SPECIAL_KEY_TAB:
          {
            int counter;

            if (do_syntax_check)
            {
              for (counter = 0; counter < number_of_commands; counter++)
              {
                if (string_compare_max (command[counter].name,
                                        string, input_string_position) == 0)
                {
                  string_copy (string, command[counter].name);
                  input_string_length = string_length (string);
                  input_string_position = input_string_length;
                  string_input_update (string, input_string_position, FALSE);
                  break;
                }
              }
            }              

            break;
          }
          
          /* Left arrow. */

          case IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_4:
          {
            if (input_string_position > 0)
            {
              input_string_position -=
                unicode_utf8_previous_character_length
                  (string, input_string_position);
              string_input_update (string, input_string_position, FALSE);
            }
            break;
          }

          /* Right arrow. */

          case IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_6:
          {
            if (input_string_position < input_string_length)
            {
              input_string_position +=
                unicode_utf8_next_character_length
                  (string, input_string_position);
              string_input_update (string, input_string_position, FALSE);
            }
            break;
          }
        
          /* Up arrow. */
          
          case IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_8:
          {
            unsigned int index;

            if (command_list &&
                command_list_pointer->next != NULL)
            {
              for (index = 0; index < input_string_characters; index++)
              {
                temporary_string[index] = ' ';
              }
              temporary_string[input_string_characters] = '\0';

              /* Save the current string. */

              string_copy (command_list_pointer->data, string);

              /* In with the previous (next ;) one. */

              command_list_pointer = (list_type *) command_list_pointer->next;

              /* FIXME: No overflow allowed. */

              string_copy (string, command_list_pointer->data);
              
              input_string_length = string_length (string);
              input_string_characters = unicode_utf8_string_characters (string);
              input_string_position = input_string_length;

              string_input_update (temporary_string, 0, FALSE);
              string_input_update (string, input_string_position, FALSE);
            }
            break;
          }
          
          /* Down arrow. */
          
          case IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_2:
          {
            unsigned int index;

            if (command_list &&
                command_list_pointer->previous != NULL)
            {
              for (index = 0; index < input_string_characters; index++)
              {
                temporary_string[index] = ' ';
              }
              temporary_string[input_string_characters] = '\0';

              /* Save the current string. */

              string_copy (command_list_pointer->data, string);
            
              command_list_pointer = (list_type *) command_list_pointer->previous;
              string_copy_max (string, command_list_pointer->data, max_length);
              
              input_string_length = string_length (string);
              input_string_characters = unicode_utf8_string_characters (string);
              input_string_position = input_string_length;

              string_input_update (temporary_string, 0, FALSE);
              string_input_update (string, input_string_position, FALSE);
            }
            break;
          }

          /* Anything else. */
          
          default:
          {
            break;
          }
        }
      }
    }
  }

  string_copy (buffer, string);

  if (command_list)
  {
    string_copy (command_list_pointer->data, string);
    command_list_cleanup ();
  }
  
  memory_deallocate ((void **) &string);
  memory_deallocate ((void **) &temporary_string);

  console_print_formatted (&console_structure, "\e[u%s", buffer);
}

/* Print a string with some escape code magic. */

void prompt_print (char *input)
{
  char output[512];
  int input_position = 0;
  int output_position = 0;

  /* Make sure we have an input. */

  if (input == NULL)
  {
    return;
  }

  memory_set_u8 (output, 0, 512);
  while (input[input_position] != '\0' && output_position < 512)
  {
    if (input[input_position] == '\\')
    {
      if (input[input_position + 1] != '\0')
      {
        switch (input[input_position + 1])
        {
          /* Escape. */

          case 'e':
          {
            output[output_position] = '\e';
            output_position++;
            break;
          }

          /* System host name. */

          case 'h':
          {
            string_copy (&output[output_position], host_name);
            output_position += string_length (host_name);
            break;
          }

          /* New line. */

          case 'n':
          {
            output[output_position] = '\n';
            output_position++;
            break;
          }

          /* Current user name. */

          case 'u':
          {
            string_copy (&output[output_position], user_name);
            output_position += string_length (user_name);
            break;
          }

          /* Current working directory. */

          case 'w':
          {
            string_copy (&output[output_position], working_directory);
            output_position += string_length (working_directory);
            break;
          }
        }
        input_position += 2;
      }
    }
    else
    {
      output[output_position] = input[input_position];
      output_position++;
      input_position++;
    }
  }
  
  console_print (&console_structure, output);
}

/* Run the given command. */

void run (char *command_string)
{
  unsigned int words;

  /* Split the string into words. */
  
  memory_set_u8 (parsed_command.arguments, 0, 512);
  words = arguments_parse (command_string, &parsed_command);
  
  if (words > 0)
  {
    char *first_word = parsed_command.argument_start[0];
    int counter;
    
    for (counter = 0; counter < number_of_commands; counter++)
    {
      if (!string_compare (first_word, command[counter].name))
      {
        command_pointer = command[counter].pointer;
        prompt_print (environment_get ("prefix"));
        command_pointer (parsed_command.number_of_arguments,
                         parsed_command.argument_start);
        prompt_print (environment_get ("postfix"));
        break;
      }
    }
    
    /* If this command does not exist, tell the user politely so. */
    
    if (counter == number_of_commands)
    {
      prompt_print (environment_get ("prefix"));
      console_print (&console_structure,
                     "Unknown command. Try '?' or 'help' for help.\n");
      prompt_print (environment_get ("postfix"));
    }
  }
}

/* Main loop. */

void main_loop (void)
{
  char user_name_string[50];

  system_thread_name_set ("Command loop");

  /* Before we begin, we have to know which user's config files to read. */
  /* FIXME: This functionality should _NOT_ lie in this program. */

  console_print (&console_structure, "\
I now need to know who you are, so I can read your personal configuration
files from your home directory. Please enter your username, or just press
enter if you don't care: ");

  string_input (50, FALSE, FALSE, user_name_string);

  if (string_length (user_name_string) == 0)
  {
    string_copy (user_name, "nobody");
  }
  else
  {
    string_copy (user_name, user_name_string);
  }

  console_print (&console_structure, "\n\n");

  /* FIXME: Read the .cluidorc file... ;) */

  /* Main loop. */

  while (TRUE)
  {
    char command_string[500];

    prompt_print (environment_get ("prompt"));
    string_input (500, TRUE, TRUE, command_string);
    console_print (&console_structure, "\n");

    if (command_string[0] != '\0')
    {
      run (command_string);
    }
  }
}
