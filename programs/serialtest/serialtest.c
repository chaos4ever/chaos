/* $Id$ */
/* Abstract: Terminator 2000. */
/* Authors: Martin Alvarez <malvarez@aapsa.es>
            Per Lundberg <plundis@chaosdev.org> */

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

#define BAUDRATE_NUM 9
#define PORT_NUM 2

static int baudrates [] = { 300, 1200, 2400, 4800, 9600, 19200,
                            38400, 57600, 115200 };

ipc_structure_type ipc_structure1;
ipc_structure_type ipc_structure2;
log_structure_type log_structure;
console_structure_type console_structure;
volatile bool lock = FALSE;
bool config1 = TRUE;
bool config2 = TRUE;
u8 port_num = SERIAL_TTYS0;
tag_type empty_tag = 
{
  0, 0, ""
};

int main (void)
{
  keyboard_packet_type keyboard_packet;
  int event_type;
  bool local_echo = FALSE;
  int baudrate_num = 4;

  log_init (&log_structure, PACKAGE_NAME, &empty_tag);
  
  system_call_process_name_set (PACKAGE_NAME);

  console_init (&console_structure, &empty_tag);
  console_open (&console_structure, 80, 50, 4, VIDEO_MODE_TYPE_TEXT);
  console_use_keyboard (&console_structure, TRUE, CONSOLE_KEYBOARD_NORMAL);
  console_clear (&console_structure);
  console_print (&console_structure,
                 "Terminator 2000 - The final terminator.\n\n");
    
  if (system_call_thread_create () == STORM_RETURN_THREAD_NEW)
  {
    unsigned char buffer[100];
    int length = 1;
    
    while (TRUE)
    {
      if (config1 != FALSE)
      {
        while (lock == TRUE)
        {
          system_call_dispatch_next ();
        }
        lock = TRUE;

        if (serial_init (&ipc_structure1, &empty_tag) != SERIAL_RETURN_SUCCESS)
        {
          log_print (&log_structure, LOG_URGENCY_EMERGENCY,
                     "Could not establish connection to a serial service.");
          return -1;
        }
        config1 = FALSE;
        lock = FALSE;
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "COM %d opened rx.", port_num+1);
      }

      if (serial_read (&ipc_structure1, buffer, length) != SERIAL_RETURN_SUCCESS)
      {
        log_print (&log_structure, LOG_URGENCY_ERROR,
                   "Error reading from port.");
      }
      else
      {
        buffer[length] = 0;
        console_print (&console_structure, buffer);
      }
    }
  }

  while (TRUE)
  {
    bool send = TRUE;
    bool configure = FALSE;
    char key;

    if (config2 != FALSE)
    {
      if (serial_init (&ipc_structure2, &empty_tag) != SERIAL_RETURN_SUCCESS)
      {
        log_print (&log_structure, LOG_URGENCY_EMERGENCY,
                   "Could not establish connection to a serial service.");
        return -1;
      }
      config2 = FALSE;
      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "COM %d opened tx.", port_num+1);
    }

    console_event_wait (&console_structure, &keyboard_packet,
                        &event_type, TRUE);

    if (keyboard_packet.key_released == 0)
    {
      if (keyboard_packet.has_character_code == 1)
      {
        key = keyboard_packet.character_code[0];
      }
      else if (keyboard_packet.has_special_key == 1)
      {
        switch (keyboard_packet.special_key[0])
        {
          case IPC_KEYBOARD_SPECIAL_KEY_ESCAPE:
          {
            log_print (&log_structure, LOG_URGENCY_DEBUG, "Local echo toggled.");
            local_echo = !local_echo;
            send = FALSE;
            break;
          }

          case IPC_KEYBOARD_SPECIAL_KEY_F10:
          {
            if (++port_num >= PORT_NUM)
            {
              port_num = 0;
            }

            send = FALSE;
            config2 = TRUE;

            while (lock == TRUE)
            {
              system_call_dispatch_next ();
            }
            lock = TRUE;
            config1 = TRUE;
            lock = FALSE;
            break;
          }

          case IPC_KEYBOARD_SPECIAL_KEY_F11:
          {
            if (--baudrate_num < 0)
            {
              baudrate_num = BAUDRATE_NUM - 1;
            }

            send = FALSE;
            configure = TRUE;
            break;
          }

          case IPC_KEYBOARD_SPECIAL_KEY_F12:
          {
            if (++baudrate_num >= BAUDRATE_NUM)
            {
              baudrate_num = 0;
            }

            send = FALSE;
            configure = TRUE;
            break;
          }

          case IPC_KEYBOARD_SPECIAL_KEY_TAB:
          {
            key = '\t';
            break;
          }

          case IPC_KEYBOARD_SPECIAL_KEY_BACK_SPACE:
          {
            key = 8;
            break;
          }
          
          case IPC_KEYBOARD_SPECIAL_KEY_ENTER:
          {
            key = '\n';
            break;
          }
        }
      }

      if (send)
      {
        if (serial_write (&ipc_structure2, &key, 1) != SERIAL_RETURN_SUCCESS)
        {
          log_print (&log_structure, LOG_URGENCY_ERROR, "Error writing to port.");
        }
        else
        {
          if (local_echo)
          {         
            char buffer[] = { key, 0 };
            console_print (&console_structure, buffer);
          }
        }
      }

      if (configure != FALSE)
      {
        if (serial_config_line (&ipc_structure2, baudrates[baudrate_num], 8, 
            0, SERIAL_PARITY_NONE) != SERIAL_RETURN_SUCCESS)
        {
          log_print (&log_structure, LOG_URGENCY_ERROR, "Error configuring port.");
        }
        else
        {
          log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "%d 8/N/1 selected.", baudrates[baudrate_num]);
        }
      }
    }
  }

  return 0;
}
