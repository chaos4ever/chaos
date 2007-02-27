/* $Id$ */
/* Abstract: Functions for writing to the console. */
/* Author: Henrik Hallin <hal@chaosdev.org> */

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

#include "config.h"
#include "console_output.h"
#include "console.h"

/* This header file defines a translation map, so we know how to map
   Unicode characters to valid CP437 (or whatever font we are using)
   characters. */

#include "unicode_to_cp437.h"

/* Table for converting between ANSI and EGA colors. */

u8 color_table[] = { 0, 4, 2, 6, 1, 5, 3, 7 };

/* Function:    console_scroll ()
   Purpose:     Scroll a console up requested number of lines.
   Returns:     Nothing.
   Parameters:  Pointer to console-struct for console to scroll. */

/* FIXME: Optimize things like this: Let the outputting function wait
   a bit before scrolling and if multiple lines were written, we can
   call this function with console_scroll (console,
   number_of_lines). This needs the buffer for the console to be
   actual number of lines on screen + maximum scrollable lines per
   call to this function, to hold the text to be displayed at the
   bottom of the screen after the scrolling has been performed. */

static void console_scroll (console_type *console, int number_of_lines)
{
  memory_copy (console->output, console->output + console->width,
               console->width * (console->height - 1) *
               sizeof (character_type));
  memory_set_u16 ((u16 *) (console->output + (console->height - 1) *
                           console->width),
                  ((console->current_attribute) << 8) + ' ', console->width *
                  sizeof (character_type) / sizeof (u16));

  if (console->cursor_saved_y != -1 &&
      console->cursor_saved_y >= number_of_lines)
  {
    console->cursor_saved_y--;
  }

  number_of_lines++;
}

/* Function:    console_set_foreground ()
   Purpose:     Set foreground color to use when outputting text to a console.
   Returns:     Nothing.
   Parameters:  Pointer to console-struct for console.
                chaos color code. */

static void console_set_foreground (console_type *console, u32 color)
{
  console->current_attribute =
    (console->current_attribute & 0xF0) + color_table[color];

  if (console->bold == TRUE)
  {
    console->modified_attribute =
      (console->current_attribute & 0xF0) +
      ((console->current_attribute & 0x0F) + 8);
  }
  else
  {
    console->modified_attribute = console->current_attribute;
  }
}

/* Function:    console_set_background ()
   Purpose:     Set background color to use when outputting text to a console.
   Returns:     Nothing.
   Parameters:  Pointer to console-struct for console.
                chaos color code. */

static void console_set_background (console_type *console, u32 color)
{
  console->current_attribute =
    (console->current_attribute & 0x0F) + (color_table[color] << 4);

  if (console->bold == TRUE)
  {
    console->modified_attribute =
      (console->current_attribute & 0xF0) +
      ((console->current_attribute & 0x0F) + 8);
  }
  else
  {
    console->modified_attribute = console->current_attribute;
  }
}

static void console_kill_line (console_type *console, int argument)
{
  switch (argument)
  {
    case 0:
    {
      /* Clear line from cursor position. */
      
      memory_set_u16 ((u16 *) ((u32) console->output +
                               (console->cursor_y * console->width +
                                console->cursor_x) * sizeof (character_type)),
                      (console->modified_attribute << 8) + ' ',
                      (console->width - console->cursor_x) *
                      sizeof (character_type) / 2);
      break;
    }
    
    case 1:
    {
      /* Clear from start of line to cursor position (inclusive). */
      
      memory_set_u16 ((u16 *) ((u32) console->output +
                               console->cursor_y * console->width *
                               sizeof (character_type)),
                      (console->modified_attribute << 8) + ' ',
                      console->cursor_x *
                      sizeof (character_type) / 2);
      break;
    }
    
    case 2:
    {
      /* Clear whole line. */
      
      memory_set_u16 ((u16 *) ((u32) console->output +
                               console->cursor_y * console->width *
                               sizeof (character_type)),
                      (console->modified_attribute << 8) + ' ',
                      console->width * sizeof (character_type) / 2);
      break;
    }
  }
}

static void console_kill_screen (console_type *console, int argument)
{
  switch (argument)
  {
    case 0:
    {
      unsigned int cursor_address =
        (console->cursor_y * console->width + console->cursor_x) *
        sizeof (character_type);
      
      /* Clear screen from cursor position (inclusive) to
         end. */
      
      memory_set_u16
        ((u16 *) ((u32) console->output + cursor_address),
         (console->current_attribute << 8),
         console->width * console->height *
         sizeof (character_type) / 2 - cursor_address);
      break;
    }
    
    case 1:
    {
      unsigned int cursor_address =
        (console->cursor_y * console->width + console->cursor_x) *
        sizeof (character_type) / 2;
      
      /* Clear screen from start to cursor position
         (inclusive). */
      
      memory_set_u16 ((u16 *) console->output,
                      (console->current_attribute << 8),
                      console->width * console->height *
                      sizeof (character_type) / 2 - cursor_address);
      break;
    }
    
    case 2:
    {
      /* Clear the screen entire screen. */
      
      memory_set_u16 ((u16 *) console->output,
                      (console->current_attribute << 8),
                      console->width * console->height *
                      sizeof (character_type) / 2);
      break;
    }
  }
}

static void console_cursor_up (console_type *console, int argument)
{
  if ((int) (console->cursor_y - argument) < 0)
  {
    console->cursor_y = 0;
  }
  else
  {
    console->cursor_y -= argument;
  }
}

static void console_cursor_down (console_type *console, int argument)
{
  if ((console->cursor_y + argument) > console->height)
  {
    console->cursor_y = console->height;
  }
  else
  {
    console->cursor_y += argument;
  }
}

static void console_cursor_right (console_type *console, int argument)
{
  if ((console->cursor_x + argument) > console->width)
  {
    console->cursor_x = console->width;
  }
  else
  {
    console->cursor_x += argument;
  }
}

static void console_cursor_left (console_type *console, int argument)
{
  if ((int) (console->cursor_x - argument) < 0)
  {
    console->cursor_x = 0;
  }
  else
  {
    console->cursor_x -= argument;
  }
}

static void console_cursor_move (console_type *console, int x, int y)
{
  x--;
  y--;

  if (x < 0)
  {
    console->cursor_x = 0;
  }
  else if (x > console->width)
  {
    console->cursor_x = console->width;
  }
  else
  {
    console->cursor_x = x;
  }

  if (y < 0)
  {
    console->cursor_y = 0;
  }
  else if (y > console->height)
  {
    console->cursor_y = console->height;
  }
  else
  {
    console->cursor_y = y;
  }
}

static void console_n (console_type *console, int argument)
{
  switch (argument)
  {
    case 6:
    {
      /* FIXME: What to do here? */
      
      console_output (console,
                      "\nchaos Console Server. Copyright chaos development 1999-2000.\nStatus: OK.\n");
    }
  }
}

/* Function:    console_output ()
   Purpose:     Output a string to a console and take care of all
                ANSI escape sequences. This function also converts
                from UTF-8 to the best character set the console allows.
   Returns:     Nothing.
   Parameters:  Pointer to console-struct for console.
                Pointer to string to output. */

/* FIXME: Write small inline functions for all escape sequences and
   call them, so code won't have to be duplicated. */

void console_output (console_type *console, const char *string)
{
  unsigned int string_index = 0;
  int old_cursor_x = console->cursor_x;
  int old_cursor_y = console->cursor_y;
  
  /* Modify the attribute according to the flags. */
  
  if (console->bold == TRUE)
  {
    console->modified_attribute = ((console->current_attribute & 0xF0) +
                                   ((console->current_attribute & 0x0F) + 8));
  }
  else
  {
    console->modified_attribute = console->current_attribute;
  }  
  
  while (string[string_index] != '\0')
  {
    switch (console->state)
    {
      /* The string is, so far, nice and full of printable
         characters. */

      case CONSOLE_STATE_CHARACTER:
      {
        switch (string[string_index])
        {
          /* Escape. */
          
          case '\e':
          {
            console->state = CONSOLE_STATE_PREESCAPE;
            
            break;
          }
          
          /* New line. */
          
          case '\n':
          {
            console->cursor_x = 0;
            console->cursor_y++;
            
            if (console->cursor_y == console->height)
            {
              console_scroll (console, 1);
              console->cursor_y--;
            }
            break;
          }
          
          /* Carriage return. */
          
          case '\r':
          {
            console->cursor_x = 0;
            break;
          }
          
          /* Ordinary printable character. */
          
          case 32 ... 255:
          {
            /* FIXME: inverse, underline etc... Support all features
               (except maybe blink) if in graphic mode. */
            
            int buffer_index;
            unsigned int length;
            ucs2_type ucs2;

            /* If this is an UTF-8 character, convert it to UCS-2. */

            if (unicode_utf8_to_ucs2 (&ucs2, (u8 *) &string[string_index],
                                      &length) !=
                UNICODE_RETURN_SUCCESS)
            {
              break;
            }

            string_index += length - 1;

            buffer_index = (console->cursor_y * console->width + 
                            console->cursor_x);
            console->output[buffer_index].attribute =
              console->modified_attribute;

            /* The ASCII characters are always the same. */

            if (ucs2 >= 0x80)
            {
              console->output[buffer_index].character = translation[ucs2];
            }
            else
            {
              console->output[buffer_index].character = ucs2;
            }
            console->cursor_x++;
            
            if (console->cursor_x == console->width)
            {
              console->cursor_x = 0;
              console->cursor_y++;
              
              if (console->cursor_y == console->height)
              {
                console_scroll (console, 1);
                console->cursor_y--;
              }
            }
            break;
          }

          /* Unsupported character. */
          
          default:
          {
            break;
          }
        }
        break;
      }

      /* Are we initiating an escape sequence? */

      case CONSOLE_STATE_PREESCAPE:
      {
        if (string[string_index] == '[')
        {
          console->state = CONSOLE_STATE_ESCAPE;
        }
        else
        {
          console->state = CONSOLE_STATE_CHARACTER;
        }
        break;
      }

      /* The last characters were the start of an escape sequence. */

      case CONSOLE_STATE_ESCAPE:
      {
        switch (string[string_index])
        {
          /* Numeric. */

          case '0' ... '9':
          {
            console->numeric_argument[console->numeric_argument_index] =
              string[string_index] - '0';
            console->state = CONSOLE_STATE_NUMERIC;
            break;
          }

          /* Escape commands without numeric argument follows. */

          /* Clear screen from cursor position (inclusive). */

          case 'J':
          {
            console_kill_screen (console, 0);

            break;
          }

          /* Save cursor position. */
          
          case 's':
          {          
            console->cursor_saved_x = console->cursor_x;
            console->cursor_saved_y = console->cursor_y;
            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }
          
          /* Restore cursor position. */
          
          case 'u':
          {
            if (console->cursor_saved_x != -1)
            {
              console->cursor_x = console->cursor_saved_x;
              console->cursor_y = console->cursor_saved_y;
            }
            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }
        
          /* Erase from cursor position to end of line. */
          
          case 'K':
          {
            console_kill_line (console, 0);

            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }

          /* Move one line up. */

          case 'A':
          {
            console_cursor_up (console, 1);

            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }

          /* Move one line down. */

          case 'B':
          {
            console_cursor_down (console, 1);

            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }

          /* Move one step to the right. */

          case 'C':
          {
            console_cursor_right (console, 1);

            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }

          /* Move one step to the left. */

          case 'D':
          {
            console_cursor_left (console, 1);

            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }

          /* Unknown escape command. */

          default:
          {
            break;
          }
        }
        break;
      }

      case CONSOLE_STATE_NUMERIC:
      {
        switch (string[string_index])
        {
          /* Numeric. */

          case '0' ... '9':
          {
            console->numeric_argument[console->numeric_argument_index] *= 10;
            console->numeric_argument[console->numeric_argument_index] +=
              (string[string_index] - '0');
            break;
          }
          
          /* Another numeric value will follow. Save the current. */

          case ';':
          {
            console->numeric_argument_index++;
            console->numeric_argument[console->numeric_argument_index] = 0;
            break;
          }

          /* Escape command that take numeric argument(s) follows. */

          case 'J':
          {
            console_kill_screen (console, console->numeric_argument[0]);

            console->numeric_argument_index = 0;
            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }
          
          /* Clear line. */

          case 'K':
          {
            console_kill_line (console, console->numeric_argument[0]);

            console->numeric_argument_index = 0;
            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }

          /* Device status report. */

          case 'n':
          {
            console_n (console, console->numeric_argument[0]);

            console->numeric_argument_index = 0;
            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }

          /* Cursor absolute move. */
          /* NOTE: This is 1-indexed!!! */

          case 'f':
          case 'H':
          {
            if (console->numeric_argument_index == 1)
            {
              console_cursor_move (console, console->numeric_argument[0],
                                   console->numeric_argument[1]);
            }

            console->numeric_argument_index = 0;
            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }

          /* Cursor up. */
          
          case 'A':
          {              
            if (console->numeric_argument_index == 0)
            {
              console_cursor_up (console, console->numeric_argument[0]);
            }

            console->numeric_argument_index = 0;
            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }
          
          /* Cursor down. */
          
          case 'B':
          { 
            if (console->numeric_argument_index == 0)
            {
              console_cursor_down (console, console->numeric_argument[0]);
            }

            console->numeric_argument_index = 0;
            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }
          
          /* Cursor right. */
          
          case 'C':
          {
            if (console->numeric_argument_index == 0)
            {
              console_cursor_right (console, console->numeric_argument[0]);
            }

            console->numeric_argument_index = 0;
            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }
          
          /* Cursor left. */
          
          case 'D':
          {
            if (console->numeric_argument_index == 0)
            {
              console_cursor_left (console, console->numeric_argument[0]);
            }

            console->numeric_argument_index = 0;
            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }
          
          /* Tabulation stuff. */

          case 'g':
          {
            break;
          }

          /* Set attribute. */
          
          case 'm':
          {
            unsigned int counter;

            for (counter = 0; counter < console->numeric_argument_index + 1;
                 counter++)
            {
              switch (console->numeric_argument[counter])
              {
                /* Foreground color. */
                
                case 30 ... 37:
                {
                  console_set_foreground
                    (console, console->numeric_argument[counter] - 30);
                  break;
                }
                
                /* Background color. */
                
                case 40 ... 47:
                {
                  console_set_background
                    (console, console->numeric_argument[counter] - 40);
                  break;
                }
                
                /* All attributes off. */
                
                case 0:
                {
                  console->bold = FALSE;
                  console->underline = FALSE;
                  console->inverse = FALSE;
                  console->blink = FALSE;
                  break;
                }
                
                /* Turn on bold. */
                
                case 1:
                {
                  if (console->bold == FALSE)
                  {
                    console->bold = TRUE;
                    console->modified_attribute =
                      (console->current_attribute & 0xf0) +
                      ((console->current_attribute & 0x0f) + 8);
                  }
                  break;
                }
                
                /* Turn on underlined text. */
                
                case 4:
                {
                  console->underline = TRUE;
                  break;
                }
                
                /* Turn on blink. */
                
                case 5:
                {
                  console->blink = TRUE;
                  break;
                }
                
                /* Turn on inverse. */
                
                case 7:
                {
                  console->inverse = TRUE;
                  break;
                }

                default:
                {
                  break;
                }
              }
            }

            console->numeric_argument_index = 0;
            console->state = CONSOLE_STATE_CHARACTER;
            break;
          }
          
          /* Unknown escape command. */

          default:
          {
            break;
          }
        }
        break;
      }

      /* Unknown state. */

      default:
      {
        break;
      }
    }

    string_index++;
  }

  /* Check if the cursor position was updated. If it was, move the
     physical cursor. */
  
  if (has_video &&
      current_console == console && 
      (old_cursor_x != console->cursor_x || 
       old_cursor_y != console->cursor_y))
  {
    video_cursor_type video_cursor;
    message_parameter_type message_parameter;

    video_cursor.x = console->cursor_x;
    video_cursor.y = console->cursor_y;
    message_parameter.data = &video_cursor;
    message_parameter.block = FALSE;
    message_parameter.length = sizeof (video_cursor_type);
    message_parameter.protocol = IPC_PROTOCOL_VIDEO;
    message_parameter.message_class = IPC_VIDEO_CURSOR_PLACE;

    ipc_send (video_structure.output_mailbox_id, &message_parameter);
  }
}

