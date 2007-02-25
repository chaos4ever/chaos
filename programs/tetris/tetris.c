/* $Id$ */
/* Abstract: A simple tetris game. */
/* Authors: Henrik Hallin <hal@chaosdev.org>
            Anders Öhrt <doa@chaosdev.org>
            Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development */

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
#include "tetris.h"

static void draw_playfield (void);
static u16 read_key (void);
static bool collides (int current_piece, int current_rotation,
                      int piece_x, int piece_y);
static void fixate_piece_on_playfield (int current_piece, int current_rotation,
                                       int piece_x, int piece_y);
static void paste_piece_to_playfield (int current_piece, int current_rotation,
                                      int piece_x, int piece_y);

const piece_type piece[] =
{
  /* ##
      ## */

  {
    {
      {
        {  0, 0, 0, 0 },
        {  1, 1, 0, 0 },
        {  0, 1, 1, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 0, 1, 0 },
        {  0, 1, 1, 0 },
        {  0, 1, 0, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 0, 0, 0 },
        {  1, 1, 0, 0 },
        {  0, 1, 1, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 0, 1, 0 },
        {  0, 1, 1, 0 },
        {  0, 1, 0, 0 },
        {  0, 0, 0, 0 }
      }
    }
  },

  /*  ##
     ##  */ 
    
  {
    {
      {
        {  0, 0, 0, 0 },
        {  0, 1, 1, 0 },
        {  1, 1, 0, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 1, 0, 0 },
        {  0, 1, 1, 0 },
        {  0, 0, 1, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 0, 0, 0 },
        {  0, 1, 1, 0 },
        {  1, 1, 0, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 1, 0, 0 },
        {  0, 1, 1, 0 },
        {  0, 0, 1, 0 },
        {  0, 0, 0, 0 }
      }
    }
  },

  /* #### */ 
    
  {
    {
      {
        {  0, 0, 0, 0 },
        {  1, 1, 1, 1 },
        {  0, 0, 0, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 1, 0, 0 },
        {  0, 1, 0, 0 },
        {  0, 1, 0, 0 },
        {  0, 1, 0, 0 }
      },
      {
        {  0, 0, 0, 0 },
        {  1, 1, 1, 1 },
        {  0, 0, 0, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 1, 0, 0 },
        {  0, 1, 0, 0 },
        {  0, 1, 0, 0 },
        {  0, 1, 0, 0 }
      }
    }
  },

  /* ##
     ## */ 
    
  {
    {
      {
        {  0, 0, 0, 0 },
        {  0, 1, 1, 0 },
        {  0, 1, 1, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 0, 0, 0 },
        {  0, 1, 1, 0 },
        {  0, 1, 1, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 0, 0, 0 },
        {  0, 1, 1, 0 },
        {  0, 1, 1, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 0, 0, 0 },
        {  0, 1, 1, 0 },
        {  0, 1, 1, 0 },
        {  0, 0, 0, 0 }
      }
    }
  },

  /* ###
      #  */
    
  {
    {
      {
        {  0, 0, 0, 0 },
        {  1, 1, 1, 0 },
        {  0, 1, 0, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 1, 0, 0 },
        {  0, 1, 1, 0 },
        {  0, 1, 0, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 1, 0, 0 },
        {  1, 1, 1, 0 },
        {  0, 0, 0, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 1, 0, 0 },
        {  1, 1, 0, 0 },
        {  0, 1, 0, 0 },
        {  0, 0, 0, 0 }
      }
    }
  },

  /* ###
       # */
    
  {
    {
      {
        {  0, 0, 0, 0 },
        {  1, 1, 1, 0 },
        {  0, 0, 1, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 1, 1, 0 },
        {  0, 1, 0, 0 },
        {  0, 1, 0, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  1, 0, 0, 0 },
        {  1, 1, 1, 0 },
        {  0, 0, 0, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 1, 0, 0 },
        {  0, 1, 0, 0 },
        {  1, 1, 0, 0 },
        {  0, 0, 0, 0 }
      }
    }
  },

  /* ###
     #   */
    
  {
    {
      {
        {  0, 0, 0, 0 },
        {  1, 1, 1, 0 },
        {  1, 0, 0, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 1, 0, 0 },
        {  0, 1, 0, 0 },
        {  0, 1, 1, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  0, 0, 1, 0 },
        {  1, 1, 1, 0 },
        {  0, 0, 0, 0 },
        {  0, 0, 0, 0 }
      },
      {
        {  1, 1, 0, 0 },
        {  0, 1, 0, 0 },
        {  0, 1, 0, 0 },
        {  0, 0, 0, 0 }
      }
    }
  }
};

#define NUMBER_OF_PIECES (sizeof (piece) / sizeof (piece_type))
#define LEFT (IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_4 << 8)
#define RIGHT (IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_6 << 8)
#define ROTATE (IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_8 << 8)
#define DOWN (IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_2 << 8)
#define QUIT (IPC_KEYBOARD_SPECIAL_KEY_ESCAPE << 8)
#define NOTHING 0xFFFF
#define TIMEOUT 500

int playfield[PLAYFIELD_WIDTH][PLAYFIELD_HEIGHT];

/* The tetris game. */

void tetris (void)
{
  int current_piece = -1, current_rotation = 0;
  int piece_x = -1, piece_y = -1;
  int x_pos, y_pos;
  bool done = FALSE;
  time_type random_seed;
  time_type timeout_time;
  time_type current_time;
  u16 key;

  console_clear (&console_structure);

  /* Initialise the playfield. */

  for (y_pos = 0; y_pos < PLAYFIELD_HEIGHT; y_pos++)
  {
    for (x_pos = 0; x_pos < PLAYFIELD_WIDTH; x_pos++)
    {
      playfield[x_pos][y_pos] = 1;
    }
  }

  for (y_pos = 0; y_pos < PLAYFIELD_HEIGHT - 1; y_pos++)
  {
    for (x_pos = 1; x_pos < PLAYFIELD_WIDTH - 1; x_pos++)
    {
      playfield[x_pos][y_pos] = 0;
    }
  }

  /* Initialise random sequence. */

  system_call_timer_read (&random_seed);

  random_init ((int) random_seed);

  /* Initialise timeout value. */

  system_call_timer_read (&current_time);
  timeout_time = current_time + TIMEOUT;

  /* Redraw the playfield. */
  
  draw_playfield ();

  /* Main loop. */

  while (!done)
  {
    /* Do we need to create a new piece? */

    if (current_piece == -1)
    {
      current_piece = random (NUMBER_OF_PIECES);
      current_rotation = 0;
      piece_x = (PLAYFIELD_WIDTH / 2) - 2;
      piece_y = 0;

      if (collides (current_piece, current_rotation,
                    piece_x - 1, piece_y))
      {
        fixate_piece_on_playfield (current_piece, current_rotation,
                                   piece_x, piece_y);        
        done = TRUE;
        continue;
      }
    }

    /* Loop, waiting for input. */

    do
    {
      system_call_timer_read (&current_time);
      key = read_key ();
    } while (current_time < timeout_time &&
             key == NOTHING);

    if (current_time >= timeout_time)
    {
      timeout_time += TIMEOUT;
    }

    /* Try to move the piece according to input. */

    switch (key)
    {
      case LEFT:
      {
        if (!(collides (current_piece, current_rotation,
                        piece_x - 1, piece_y)))
        {
          piece_x--;
        }
        break;
      }

      case RIGHT:
      {
        if (!(collides (current_piece, current_rotation,
                        piece_x + 1, piece_y)))
        {
          piece_x++;
        }
        break;
      }

      case DOWN:
      {        
        if (!(collides (current_piece, current_rotation,
                        piece_x, piece_y + 1)))
        {
          piece_y++;
        }
        break;
      }

      case ROTATE:
      {
        if (!(collides (current_piece, (current_rotation + 1) % 4,
                        piece_x, piece_y)))
        {
          current_rotation++;
          current_rotation %= 4;
        }
        break;
      }

      case QUIT:
      {
        done = TRUE;
        continue;
        break;
      }

      case NOTHING:
      {
        /* If we pressed nothing, the move is due to a timeout. */

        if (!(collides (current_piece, current_rotation,
                        piece_x, piece_y + 1)))
        {
          piece_y++;          
        }
        else
        {
          fixate_piece_on_playfield (current_piece, current_rotation,
                                     piece_x, piece_y);

          /* Now, check if a whole line is filled and if so, remove
             it. */

          {
            bool whole_line;
            int move_x, move_y;

            for (y_pos = 0; y_pos < PLAYFIELD_HEIGHT - 1; y_pos++)
            {
              whole_line = TRUE;

              for (x_pos = 1; x_pos < PLAYFIELD_WIDTH - 1; x_pos++)
              {
                if (playfield[x_pos][y_pos] != 1)
                {
                  whole_line = FALSE;
                }
              }

              if (whole_line)
              {
                for (move_y = y_pos; move_y > 0; move_y--)
                {
                  for (move_x = 1; move_x < PLAYFIELD_WIDTH - 1; move_x++)
                  {
                    playfield[move_x][move_y] = playfield[move_x][move_y - 1];
                  }
                }

                for (move_x = 1; move_x < PLAYFIELD_WIDTH - 1; move_x++)
                {
                  playfield[move_x][0] = 0;
                }
              }
            }
          }

          current_piece = -1;
        }
        break;
      }
    }

    if (current_piece != -1)
    {
      paste_piece_to_playfield (current_piece, current_rotation,
                                piece_x, piece_y);
    }

    draw_playfield ();
  }

  console_print_formatted (&console_structure, 
                           "Thank you for playing %s %s.\n",
                           PACKAGE_NAME, PACKAGE_VERSION);
}

static void draw_playfield (void)
{
  int x_pos, y_pos;

  char field[(PLAYFIELD_WIDTH + 1) * PLAYFIELD_HEIGHT + 1];

  /* FIXME: Put this into the same string as the field to minimise IPC. */

  console_cursor_move (&console_structure, 1, 1);

  for (y_pos = 0; y_pos < PLAYFIELD_HEIGHT; y_pos++)
  {
    for (x_pos = 0; x_pos < PLAYFIELD_WIDTH; x_pos++)
    {
      switch (playfield[x_pos][y_pos])
      {
        case 0:
        {
          field[x_pos + y_pos * (PLAYFIELD_WIDTH + 1)] = ' ';
          break;
        }

        case 1:
        {
          field[x_pos + y_pos * (PLAYFIELD_WIDTH + 1)] = '#';
          break;
        }

        case 2:
        {
          field[x_pos + y_pos * (PLAYFIELD_WIDTH + 1)] = 'O';
          break;
        }

        default:
        {
          field[x_pos + y_pos * (PLAYFIELD_WIDTH + 1)] = '?';
          break;
        }
      }
    }

    field[PLAYFIELD_WIDTH + y_pos * (PLAYFIELD_WIDTH + 1)] = '\n';
  }

  field[PLAYFIELD_HEIGHT * (PLAYFIELD_WIDTH + 1)] = 0;
  console_print (&console_structure, field);
}

static u16 read_key (void)
{
  keyboard_packet_type keyboard_packet;
  int event_type;

  keyboard_packet.has_character_code = 0;

  console_event_wait (&console_structure, &keyboard_packet,
                      &event_type, FALSE);
  
  if (event_type == CONSOLE_EVENT_KEYBOARD &&
      !keyboard_packet.key_pressed)
  {
    if (keyboard_packet.has_character_code == 1)
    {
      return (keyboard_packet.character_code[0]);
    }
    else if (keyboard_packet.has_special_key == 1)
    {
      return keyboard_packet.special_key << 8;
    }
  }
  else
  {
    return NOTHING;
  }
  return NOTHING;
}


static bool collides (int current_piece, int current_rotation,
                      int piece_x, int piece_y)
{
  int x_pos, y_pos;

  for (y_pos = 0; y_pos < 4; y_pos++)
  {
    for (x_pos = 0; x_pos < 4; x_pos++)
    {
      /* Make sure we're checking inside the playfield (we might miss
         the playfield if the piece was recently put on it). */

      if (piece_x + x_pos >= 0 &&
          piece_y + x_pos >= 0)
      {
        if (piece[current_piece].block[current_rotation][y_pos][x_pos] != 0 &&
            playfield[piece_x + x_pos][piece_y + y_pos] == 1)
        {
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}


static void fixate_piece_on_playfield (int current_piece, int current_rotation,
                                       int piece_x, int piece_y)
{
  int x_pos, y_pos;

  for (y_pos = 0; y_pos < 4; y_pos++)
  {
    for (x_pos = 0; x_pos < 4; x_pos++)
    {
      if (piece[current_piece].block[current_rotation][y_pos][x_pos]
          != 0)
      {
        /* Make sure we're checking inside the playfield (we might
           miss the playfield if the piece was recently put on it). */

        if (piece_x + x_pos >= 0 &&
            piece_y + x_pos >= 0)
        {
          playfield[x_pos + piece_x][y_pos + piece_y] = 1;
        }
      }
    }
  }        
}


static void paste_piece_to_playfield (int current_piece, int current_rotation,
                                      int piece_x, int piece_y)
{
  int x_pos, y_pos;

  /* Remove all old blocks. */

  for (y_pos = 0; y_pos < PLAYFIELD_HEIGHT - 1; y_pos++)
  {
    for (x_pos = 1; x_pos < PLAYFIELD_WIDTH - 1; x_pos++)
    {
      if (playfield[x_pos][y_pos] == 2)
      {
        playfield[x_pos][y_pos] = 0;
      }
    }
  }

  /* Paste in the new piece. */

  for (y_pos = 0; y_pos < 4; y_pos++)
  {
    for (x_pos = 0; x_pos < 4; x_pos++)
    {
      if (piece[current_piece].block[current_rotation][y_pos][x_pos]
          != 0)
      {
        if (piece_x + x_pos >= 0 &&
            piece_y + x_pos >= 0)
        {
          playfield[x_pos + piece_x][y_pos + piece_y] = 2;
        }
      }
    }
  }        
}
