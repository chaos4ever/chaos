/* Abstract: Sample play test program. */
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

ipc_structure_type ipc_structure;
log_structure_type log_structure;
console_structure_type console_structure;

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
  console_print (&console_structure,
                 "Just a single sample...\n\n");

  if (sound_init (&ipc_structure, &empty_tag) != SOUND_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Could not establish connection to a sound service.");
    return -1;
  }

  log_print(&log_structure, LOG_URGENCY_EMERGENCY,
            "Found sound service and established connection.");

  memory_allocate ((void **) &sound_message,
                   sizeof (sound_message_type) + 65536);
  sound_message->length = 65536;
  sound_message->frequency = 11000;
  sound_message->bits = 8;
  memory_copy (sound_message->data, wavefile, 65536);


  if (sound_play_sample(&ipc_structure, sound_message)
      != SOUND_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Could not play the sample as wanted.");
    return -1;
  }

  console_print(&console_structure, "play_sample acknowledge.");

  return 0;
}

