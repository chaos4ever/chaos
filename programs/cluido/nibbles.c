/* $Id$ */
/* Abstract: A simple nibbles game. */
/* Authors: Anders Öhrt <doa@chaosdev.org>
            Gustav Sinder <gs@chaosdev.org> */

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

#include "cluido.h"
#include "nibbles.h"

/* Define what key does what. */

#define KEY_LEFT    'a'
#define KEY_RIGHT   'd'
#define KEY_QUIT    'q'
#define KEY_NOTHING '\0'

#define DIRECTION_UP    0
#define DIRECTION_LEFT  1
#define DIRECTION_DOWN  2
#define DIRECTION_RIGHT 3

/* Misc values. */

#define PLAYFIELD_WIDTH   20
#define PLAYFIELD_HEIGHT  10
#define MAX_PLAYERS        8
#define MAX_WORM_LENGTH   40
#define START_WORM_LENGTH  4

#define NIBBLES_VERSION "v0.1a"

#define TIMEOUT 500

enum
{
  EMPTY,
  WALL,
  PLAYER
};

typedef struct
{
  int x_pos, y_pos;
} segment_type;

typedef struct
{
  bool dead;
  bool active;
  int length;
  int direction;
  segment_type body[MAX_WORM_LENGTH];
} worm_type;

static char read_key (void);
static void restart_player (int player_number);
static void move_players (void);
static void redraw_playfield (void);

worm_type player[MAX_PLAYERS];
int playfield[PLAYFIELD_WIDTH][PLAYFIELD_HEIGHT];

void command_nibbles (void)
{
#if FALSE
  int x_pos, y_pos, player_number, segment_number;
  time_type random_seed;
  bool done;
  char key;

  /* Initialise random sequence. */

  system_call_timer_read (&random_seed);
  random_init ((int) random_seed);

  /* Initialise playfield. */

  for (x_pos = 0; x_pos < PLAYFIELD_WIDTH; x_pos++)
  {
    for (y_pos = 0; y_pos < PLAYFIELD_HEIGHT; y_pos++)
    {
      playfield[x_pos][y_pos] = WALL;
    }
  }

  for (x_pos = 2; x_pos < PLAYFIELD_WIDTH - 2; x_pos++)
  {
    for (y_pos = 2; y_pos < PLAYFIELD_HEIGHT - 2; y_pos++)
    {
      playfield[x_pos][y_pos] = EMPTY;
    }
  }

  /* Initialise worms. */

  for (player_number = 0; player_number < MAX_PLAYERS; player_number++)
  {
    player[player_number].dead = TRUE;
    player[player_number].active = FALSE;

    for (segment_number = 0;
         segment_number < MAX_WORM_LENGTH;
         segment_number++)
    {
      player[player_number].body[segment_number].x_pos = -1;
      player[player_number].body[segment_number].y_pos = -1;
    }
  }
  
  console_clear ();
  console_cursor_move (0, 30);

  done = FALSE;

  console_print ("Starting nibbles " NIBBLES_VERSION ".\n");

  /* Let this be a single player game. */

  player[0].active = TRUE;

  while (!done)
  {
    console_clear ();

    key = read_key ();

    for (player_number = 0; player_number < MAX_PLAYERS; player_number++)
    {
      /* If an active player has died, restart him. */

      if (player[player_number].dead &&
          player[player_number].active)
      {
        restart_player (player_number);
      }
    }

    console_print ("Moving players.\n");
    move_players ();

    console_print ("Drawing playfield.\n");
    redraw_playfield ();
  }
#endif
}

static void restart_player (int player_number)
{
  int counter, segment_number, x, y;
  bool restarted = FALSE;

  console_print_formatted ("Restarting player %d\n", player_number);

  /* Try to respawn at most 1000 times. */

  for (counter = 0; counter < 1000 && restarted == FALSE; counter++)
  {
    x = 1 + random (PLAYFIELD_WIDTH - 2);
    y = 1 + random (PLAYFIELD_HEIGHT - 2);

    if (playfield[x][y] == EMPTY)
    {
      player[player_number].dead = FALSE;
      player[player_number].length = START_WORM_LENGTH;
      restarted = TRUE;

      for (segment_number = 0;
           segment_number < START_WORM_LENGTH;
           segment_number++)
      {
        player[player_number].body[segment_number].x_pos = x;
        player[player_number].body[segment_number].y_pos = y;
      }

      for (segment_number = START_WORM_LENGTH;
           segment_number < MAX_WORM_LENGTH;
           segment_number++)
      {
        player[player_number].body[segment_number].x_pos = -1;
        player[player_number].body[segment_number].y_pos = -1;
      }
    }
  }
}

static void move_players (void)
{
  int player_number, segment_number, x_pos, y_pos;

  for (player_number = 0; player_number < MAX_PLAYERS; player_number++)
  {

    /* Move all segments one step back through the worm. */

    for (segment_number = MAX_WORM_LENGTH - 1;
         segment_number > 0;
         segment_number--)
    {
      player[player_number].body[segment_number].x_pos =
        player[player_number].body[segment_number - 1].x_pos;
      player[player_number].body[segment_number].y_pos =
        player[player_number].body[segment_number - 1].y_pos;
    }

    x_pos = player[player_number].body[0].x_pos;
    y_pos = player[player_number].body[0].y_pos;

    switch (player[player_number].direction)
    {
      case DIRECTION_UP:
      {
        y_pos--;
      }
      break;
      case DIRECTION_LEFT:
      {
        x_pos--;
      }
      break;
      case DIRECTION_DOWN:
      {
        y_pos++;
      }
      break;
      case DIRECTION_RIGHT:
      {
        x_pos++;
      }
      break;
    }

    switch (playfield[x_pos][y_pos])
    {
      case EMPTY:
      {
        player[player_number].body[0].x_pos = x_pos;
        player[player_number].body[0].y_pos = y_pos;
      }
      break;
      case WALL:
      {
        player[player_number].dead = TRUE;
      }
      break;
      default:
      {

        /* FIXME: Add frags. */

        player[player_number].dead = TRUE;        
      }
      break;
    }
  }
}

static void redraw_playfield (void)
{
  int x_pos, y_pos;
  char row[PLAYFIELD_WIDTH + 2];

  for (y_pos = 0; y_pos < PLAYFIELD_HEIGHT; y_pos++)
  {
    for (x_pos = 0; x_pos < PLAYFIELD_WIDTH; x_pos++)
    {
      switch (playfield[x_pos][y_pos])
      {
        case EMPTY:
        {
          row[x_pos] = ' ';
          break;
        }
        case WALL:
        {
          row[x_pos] = '#';
          break;
        }
        case PLAYER:
        {
          row[x_pos] = 'o';
          break;
        }
        default:
        {
          row[x_pos] = '?';
          break;
        }
      }
    }  

    row[PLAYFIELD_WIDTH] = '\n';
    row[PLAYFIELD_WIDTH + 1] = '\0';
    console_print (row);
  }
}

/* Read key delay exactly TIMEOUT milliseconds, and returns a key if
   pressed, and KEY_NOTHING if no key was pressed. */

static char read_key (void)
{
#if FALSE
  keyboard_packet_type keyboard_packet;
  message_parameter_type message_parameter;
  time_type timeout_time;
  time_type current_time;
  int key = KEY_NOTHING;

  /* Initialise times. */

  system_call_timer_read (&current_time);
  timeout_time = current_time + TIMEOUT;

  do
  {

    /* If we haven't gotten a key yet, check for one. */

    if (key == KEY_NOTHING)
    {
      message_parameter.block = FALSE;
      message_parameter.length = sizeof (keyboard_packet_type);
      message_parameter.data = &keyboard_packet;
      message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
      message_parameter.class = IPC_CLASS_NONE;

      system_call_mailbox_receive (console_structure.input_mailbox_id,
                                 &message_parameter);
    }

    system_call_timer_read (&current_time);

  } while (current_time < timeout_time);

  if (message_parameter.class == IPC_CONSOLE_KEYBOARD_EVENT &&
      keyboard_packet.key_released == 0 &&
      keyboard_packet.has_character_code == 1)
  {
    return keyboard_packet.character_code[0];
  }
  else
  {
  }
#endif
  return KEY_NOTHING;
}
