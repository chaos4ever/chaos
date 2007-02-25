/* $Id$ */
/* Abstract: Stream play test program. */
/* Author: Erik Moren <nemo@chaosdev.org> */

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

#include "config.h"
#include "wavefile.h"
#include "wavefile2.h"

#define BUFFER_SIZE 32768

ipc_structure_type ipc_structure;
log_structure_type log_structure;
console_structure_type console_structure;

void fill_buffer(u8 *data);

unsigned int current_buffer = 0;

tag_type empty_tag =
{
  0, 0, ""
};

int main (void)
{
  sound_message_type *sound_message;
  log_init (&log_structure, PACKAGE_NAME, &empty_tag);

  system_call_process_name_set (PACKAGE_NAME);

  console_init (&console_structure, &empty_tag);
  console_open (&console_structure, 80, 50, 4, VIDEO_MODE_TYPE_TEXT);
  console_use_keyboard (&console_structure, TRUE, CONSOLE_KEYBOARD_NORMAL);
  console_clear (&console_structure);
  console_print (&console_structure, "'Streaming' two samples");

  if (sound_init (&ipc_structure, &empty_tag) != SOUND_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Could not establish connection to a sound service.");
    return -1;
  }

  log_print(&log_structure, LOG_URGENCY_EMERGENCY,
            "Found sound service and established connection.");

  /* Allocate memory for the buffer. Remember to allocate BUFFER_SIZE
     _and_ sizeof (sound_message_type), because a sound_message is also
     sent every time */
  memory_allocate ((void **) &sound_message,
                   sizeof (sound_message_type) + BUFFER_SIZE);

  sound_message->length = BUFFER_SIZE;
  sound_message->frequency = 11000;
  sound_message->bits = 8;

  while (TRUE)
  {
    fill_buffer(sound_message->data);

    if (sound_play_stream(&ipc_structure, sound_message)
        != SOUND_RETURN_SUCCESS)
    {
      log_print (&log_structure, LOG_URGENCY_EMERGENCY,
                 "Could not play the sample as wanted.");
      return -1;
    }

  }

  return 0;
}

void fill_buffer(u8 *buffer)
{
    /* This function is responsible of filling the buffer with
       BUFFER_SIZE data each time. This data could be part of a wave-file,
       module-file, decoded mp3-stream or whatever. This example just
       switches between two different samples simulating 'new' data each
       time fill_buffer() is called. */

    if (current_buffer == 0)
    {
      memory_copy (buffer, wavefile, BUFFER_SIZE);
      current_buffer = 1;
    }
    else
    {
      memory_copy (buffer, wavefile2, BUFFER_SIZE);
      current_buffer = 0;
    }
}

