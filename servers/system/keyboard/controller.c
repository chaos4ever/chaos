/* $Id$ */
/* Abstract: Common code for the keyboard server. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

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

#include "common.h"
#include "controller.h"
#include "keyboard.h"
#include "mouse.h"

#include <ipc/ipc.h>
#include <log/log.h>
#include <system/system.h>

/* This reads the controller status port, and does the appropriate
   action. */

u8 handle_event (void)
{
  u8 status = controller_read_status ();
  unsigned int work;
  
  for (work = 0;
       (work < 10000) && ((status & CONTROLLER_STATUS_OUTPUT_BUFFER_FULL) != 0);
       work++) 
  {
    u8 scancode;
    
    scancode = controller_read_input ();
 
#if FALSE
    /* Ignore error bytes. */

    if ((status & (CONTROLLER_STATUS_GENERAL_TIMEOUT |
                   CONTROLLER_STATUS_PARITY_ERROR)) == 0)
#endif
    {
      if ((status & CONTROLLER_STATUS_MOUSE_OUTPUT_BUFFER_FULL) != 0)
      {
        mouse_handle_event (scancode);
      }
      else
      {
        keyboard_handle_event (scancode);
      }
    }
    
    status = controller_read_status ();
    
  }

  if (work == 10000)
  {
    //    log_print (&log_structure, LOG_URGENCY_ERROR,
    //          "Keyboard controller jammed.");
  }
  
  return status;
}

/* Wait for keyboard controller input buffer to drain.
 
   Quote from PS/2 System Reference Manual:
   
     "Address hex 0060 and address hex 0064 should be written only
     when the input-buffer-full bit and output-buffer-full bit in the
     Controller Status register are set 0."  */

void controller_wait (void)
{
  unsigned long timeout;
  
  for (timeout = 0; timeout < CONTROLLER_TIMEOUT; timeout++)
  {
    /* handle_event () will handle any incoming events while we wait
       -- keypresses or mouse movement. */
    
    unsigned char status = handle_event ();
                
    if ((status & CONTROLLER_STATUS_INPUT_BUFFER_FULL) == 0)
    {
      return;
    }
    
    /* Sleep for one millisecond. */

    system_sleep (1);
  }
  
  //  log_print (&log_structure, LOG_URGENCY_ERROR, "Keyboard timed out[1]");
}

/* Wait for input from the keyboard controller. */

int controller_wait_for_input (void)
{
  int timeout;

  for (timeout = 0; timeout < KEYBOARD_INIT_TIMEOUT; timeout++)
  {
    int return_value = controller_read_data ();

    if (return_value >= 0)
    {
      return return_value;
    }

    system_sleep (1);
  }
  return -1;
}

/* Write a command word to the keyboard controller. */

void controller_write_command_word (u8 data)
{
  controller_wait ();
  controller_write_command (data);
}

/* Write an output word to the keyboard controller. */

void controller_write_output_word (u8 data)
{
  controller_wait ();
  controller_write_output (data);
}

/* Empty the keyboard input buffer. */

void keyboard_clear_input (void)
{
  int max_read;

  for (max_read = 0; max_read < 100; max_read++)
  {
    if (controller_read_data () == KEYBOARD_NO_DATA)
    {
      break;
    }
  }
}

/* Read data from the keyboard controller. */

int controller_read_data (void)
{
  int return_value = KEYBOARD_NO_DATA;
  u8 status;
  
  status = controller_read_status ();
  if ((status & CONTROLLER_STATUS_OUTPUT_BUFFER_FULL) == 
      CONTROLLER_STATUS_OUTPUT_BUFFER_FULL)
  {
    u8 data = controller_read_input ();
    
    return_value = data;
    if ((status & (CONTROLLER_STATUS_GENERAL_TIMEOUT |
                   CONTROLLER_STATUS_PARITY_ERROR)) != 0)
    {
      return_value = KEYBOARD_BAD_DATA;
    }
  }
  return return_value;
}
