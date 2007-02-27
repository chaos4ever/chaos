/* $Id$ */
/* Abstract: Keyboard server for chaos. */
/* Authors: Per Lundberg <plundis@chaosdev.org>,
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

/* Parts of this file was inspired by the Linux keyboard support. */

#include "config.h"

/* FIXME: Set this to a dummy map, and let the boot-server set the
   right key map. Or something. */
/* A Swedish translation map, for now. */

#include "keyboard_maps/british.h"
#include "keyboard_maps/swedish.h"
#include "keyboard_maps/dvorak.h"
#include "keyboard_maps/us.h"

#include "common.h"
#include "controller.h"
#include "keyboard.h"
#include "mouse.h"
#include "scan_code.h"

/* The keyboard maps convert keys to standard UTF-8 sequences. */

static const char **keyboard_map = swedish_keyboard_map;
static const char **keyboard_map_shift = swedish_keyboard_map_shift;
static const char **keyboard_map_altgr = swedish_keyboard_map_altgr;

/* We need to create an array of 16 bytes, for storing the currently
   pressed keys in. (128 scan codes / 8). */

static volatile u8 keyboard_pressed_keys[16];

/* State of the *lock-keys. */

static volatile u8 keyboard_state_scroll = 0x0F;
static volatile u8 keyboard_state_num = 0x0F;
static volatile u8 keyboard_state_caps = 0x0F;

/* The shift state. */

static volatile unsigned int shift_state = 0;

static volatile mailbox_id_type keyboard_target_mailbox_id = MAILBOX_ID_NONE;

/* Is a keyboard connected? */

static bool keyboard_exists = TRUE;

/* Used only by send_data - set by keyboard_interrupt. */

static volatile int reply_expected = 0;
static volatile int acknowledge = 0;
static volatile int resend = 0;

/* Conversion table from keyboard scan codes to the standardised
   'special key' codes, which are generic between all platforms. */

static u8 special_key_conversion[] =
{
  [SCAN_CODE_ESCAPE] = IPC_KEYBOARD_SPECIAL_KEY_ESCAPE,
  [SCAN_CODE_BACK_SPACE] = IPC_KEYBOARD_SPECIAL_KEY_BACK_SPACE,
  [SCAN_CODE_TAB] = IPC_KEYBOARD_SPECIAL_KEY_TAB,
  [SCAN_CODE_ENTER] = IPC_KEYBOARD_SPECIAL_KEY_ENTER,
  [SCAN_CODE_CONTROL] = IPC_KEYBOARD_SPECIAL_KEY_CONTROL,
  [SCAN_CODE_LEFT_SHIFT] = IPC_KEYBOARD_SPECIAL_KEY_LEFT_SHIFT,
  [SCAN_CODE_RIGHT_SHIFT] = IPC_KEYBOARD_SPECIAL_KEY_RIGHT_SHIFT,
  [SCAN_CODE_PRINT_SCREEN] = IPC_KEYBOARD_SPECIAL_KEY_PRINT_SCREEN,
  [SCAN_CODE_ALT] = IPC_KEYBOARD_SPECIAL_KEY_ALT,
  [SCAN_CODE_SPACE_BAR] = IPC_KEYBOARD_SPECIAL_KEY_SPACE_BAR,
  [SCAN_CODE_CAPS_LOCK] = IPC_KEYBOARD_SPECIAL_KEY_CAPS_LOCK,
  [SCAN_CODE_F1] = IPC_KEYBOARD_SPECIAL_KEY_F1,
  [SCAN_CODE_F2] = IPC_KEYBOARD_SPECIAL_KEY_F2,
  [SCAN_CODE_F3] = IPC_KEYBOARD_SPECIAL_KEY_F3,
  [SCAN_CODE_F4] = IPC_KEYBOARD_SPECIAL_KEY_F4,
  [SCAN_CODE_F5] = IPC_KEYBOARD_SPECIAL_KEY_F5,
  [SCAN_CODE_F6] = IPC_KEYBOARD_SPECIAL_KEY_F6,
  [SCAN_CODE_F7] = IPC_KEYBOARD_SPECIAL_KEY_F7,
  [SCAN_CODE_F8] = IPC_KEYBOARD_SPECIAL_KEY_F8,
  [SCAN_CODE_F9] = IPC_KEYBOARD_SPECIAL_KEY_F9,
  [SCAN_CODE_F10] = IPC_KEYBOARD_SPECIAL_KEY_F10,
  [SCAN_CODE_NUM_LOCK] = IPC_KEYBOARD_SPECIAL_KEY_NUM_LOCK,
  [SCAN_CODE_SCROLL_LOCK] = IPC_KEYBOARD_SPECIAL_KEY_SCROLL_LOCK,
  [SCAN_CODE_NUMERIC_7] = IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_7,
  [SCAN_CODE_NUMERIC_8] = IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_8,
  [SCAN_CODE_NUMERIC_9] = IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_9,
  [SCAN_CODE_NUMERIC_MINUS] = IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_MINUS,
  [SCAN_CODE_NUMERIC_4] = IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_4,
  [SCAN_CODE_NUMERIC_5] = IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_5,
  [SCAN_CODE_NUMERIC_6] = IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_6,
  [SCAN_CODE_NUMERIC_PLUS] = IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_PLUS,
  [SCAN_CODE_NUMERIC_1] = IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_1,
  [SCAN_CODE_NUMERIC_2] = IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_2,
  [SCAN_CODE_NUMERIC_3] = IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_3,
  [SCAN_CODE_NUMERIC_0] = IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_0,
  [SCAN_CODE_NUMERIC_DELETE] = IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_DELETE,
  [SCAN_CODE_F11] = IPC_KEYBOARD_SPECIAL_KEY_F11,
  [SCAN_CODE_F12] = IPC_KEYBOARD_SPECIAL_KEY_F12,
  [SCAN_CODE_LEFT_WINDOWS] = IPC_KEYBOARD_SPECIAL_KEY_LEFT_WINDOWS,
  [SCAN_CODE_RIGHT_WINDOWS] = IPC_KEYBOARD_SPECIAL_KEY_RIGHT_WINDOWS,
  [SCAN_CODE_MENU] = IPC_KEYBOARD_SPECIAL_KEY_MENU,

#if FALSE
  [SCAN_CODE_INSERT] = IPC_KEYBOARD_SPECIAL_KEY_INSERT,
  [SCAN_CODE_HOME] = IPC_KEYBOARD_SPECIAL_KEY_HOME,
  [SCAN_CODE_END] = IPC_KEYBOARD_SPECIAL_KEY_END,
#endif
};

/* Acknowledge the keyboard controller. */

static int do_acknowledge (unsigned char scancode)
{
  if (reply_expected != 0) 
  {
    /* Unfortunately, we must recognise these codes only if we know
       they are known to be valid (i.e., after sending a command),
       because there are some brain-damaged keyboards (yes, FOCUS 9000
       again) which have keys with such codes :( */

    if (scancode == KEYBOARD_REPLY_ACK) 
    {
      acknowledge = 1;
      reply_expected = 0;
      return 0;
    }
    else if (scancode == KEYBOARD_REPLY_RESEND) 
    {
      resend = 1;
      reply_expected = 0;
      return 0;
    }
  }
  return 1;
}

/* send_data sends a character to the keyboard and waits for an
   acknowledge, possibly retrying if asked to. Returns the success
   status. */

static bool send_data (unsigned char data)
{
  int retries = 3;
  
  do 
  {
    unsigned long timeout = KEYBOARD_TIMEOUT;
    
    /* Set by interrupt routine on receipt of ACK. */

    acknowledge = 0;
    resend = 0;
    reply_expected = 1;
    controller_write_output_word (data);

    while (TRUE)
    {
      while (TRUE)
      {
        u8 status = controller_read_status ();
        u8 scancode;

        /* Loop until there is data available. */
        
        if ((status & CONTROLLER_STATUS_OUTPUT_BUFFER_FULL) == 0)
        {
          continue;
        }  

        /* Get it. */

        scancode = controller_read_input ();
        
        /* Wait until there is a message available. */

        if ((status & CONTROLLER_STATUS_MOUSE_OUTPUT_BUFFER_FULL) != 0)
        {
          continue;
        }
        else
        {
          do_acknowledge (scancode);
          break;
        }
      }

      if (acknowledge != 0)
      {
        return TRUE;
      }

      if (resend != 0)
      {
        break;
      }

      system_sleep (1);
      timeout--;
      
      if (timeout == 0)
      {
        //log_print (&log_structure, LOG_URGENCY_ERROR,
        //                   "Timeout - AT keyboard not present?");
        return FALSE;
      }
    }
  } while (retries-- > 0);
  
  //log_print (&log_structure, LOG_URGENCY_ERROR,
  //             "Too many NACKs -- noisy keyboard cable?");
  return FALSE;
}

/* Set the typematic repeat rate of the keyboard. */

const char *keyboard_set_repeat_rate (void)
{
  /* Finally, set the typematic rate to maximum. */

  controller_write_output_word (KEYBOARD_COMMAND_SET_RATE);
  if (controller_wait_for_input () != KEYBOARD_REPLY_ACK)
  {
    return "Set rate: no ACK.";
  }

  controller_write_output_word (0x00);
  if (controller_wait_for_input () != KEYBOARD_REPLY_ACK)
  {
    return "Set rate: no ACK.";
  }
  return NULL;
}

/* Update the keyboard LEDs. */
/* FIXME: Defines!!! */

void keyboard_update_leds (void)
{
  u8 output = 0;


  if (keyboard_state_scroll == 0xF0)
  {
    output = 1;
  }

  if (keyboard_state_num == 0xF0)
  {
    output |= 2;
  }

  if (keyboard_state_caps == 0xF0)
  {
    output |= 4;
  }

  if (keyboard_exists && (!send_data (KEYBOARD_COMMAND_SET_LEDS) ||
                          !send_data (output))) 
  {
    /* Re-enable keyboard if any errors. */

    send_data (KEYBOARD_COMMAND_ENABLE);
    keyboard_exists = FALSE;
  }
}

/* Check if the given code is currently pressed. */

static inline bool key_pressed (u8 scancode)
{
  if ((keyboard_pressed_keys[scancode / 8] & (1 << (scancode % 8))) != 0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/* Translate the given key according to the current keyboard map. */

static const char *translate_key (u8 scancode)
{
  if ((shift_state & KEYBOARD_LEFT_SHIFT) == KEYBOARD_LEFT_SHIFT ||
      (shift_state & KEYBOARD_RIGHT_SHIFT) == KEYBOARD_RIGHT_SHIFT ||
      keyboard_state_caps == 0xF0)
  {
    return keyboard_map_shift[scancode];
  }
  else if (key_pressed (SCAN_CODE_ALT))
  {
    return keyboard_map_altgr[scancode];
  }
  else
  {
    return keyboard_map[scancode];
  }
}

/* Handle a keyboard event. This function is called from the interrupt
   handler. */

void keyboard_handle_event (u8 scancode)
{
  keyboard_exists = TRUE;

  if (do_acknowledge (scancode))
  {
    const char *translated_key;
    message_parameter_type message_parameter;
    keyboard_packet_type keyboard_packet;
    
    //    char tmpstr[100];
 
    //    string_print (tmpstr, "ull: %u", scancode);
    //    system_call_debug_print_simple (tmpstr);

    memory_set_u8 ((u8 *) &keyboard_packet, 0, sizeof (keyboard_packet_type));

    /* Special case -- this one is sent to us when the keyboard is
       reset from an external device (like a keyboard/monitor
       switch). It is also sent when right shift is released. What a
       sick world this is... */

    if (scancode == 170)
    {
      keyboard_update_leds ();
      keyboard_set_repeat_rate ();
    }

    if ((scancode & 0x80) == 0) 
    {
      /* A key was pressed. */
      
      keyboard_pressed_keys[scancode / 8] |= (1 << (scancode % 8));
      
      /* Check if the pressed key is a lock key. */
      
      switch (scancode)
      {
        case SCAN_CODE_CAPS_LOCK:
        {
          keyboard_state_caps = ~keyboard_state_caps;
          keyboard_update_leds ();
          break;
        }
      
        case SCAN_CODE_NUM_LOCK:
        {
          keyboard_state_num = ~keyboard_state_num;
          keyboard_update_leds ();
          break;
        }
      
        case SCAN_CODE_SCROLL_LOCK:
        {
          keyboard_state_scroll = ~keyboard_state_scroll;
          keyboard_update_leds ();
          break;
        }

        /* Other key. */
        
        default:
        {
          /* Do we have a receiver registered? */

          if (keyboard_target_mailbox_id != MAILBOX_ID_NONE)
          {
            /* Does this change the shift state? If so, set the flag
               and notify our recipient. */

            switch (scancode)
            {
              case SCAN_CODE_LEFT_SHIFT:
              {
                shift_state |= KEYBOARD_LEFT_SHIFT;
                break;
              }

              case SCAN_CODE_RIGHT_SHIFT:
              {
                shift_state |= KEYBOARD_RIGHT_SHIFT;
                break;
              }

              case SCAN_CODE_ALT:
              {
                shift_state |= KEYBOARD_RIGHT_ALT;
                break;
              }

              case SCAN_CODE_CONTROL:
              {
                shift_state |= KEYBOARD_RIGHT_CONTROL;
                break;
              }
              
              default:
              {
                /* Seems to be a normal keypress. */
                
                keyboard_packet.key_pressed = TRUE;

                /* Convert it to the chaos format. */

                translated_key = translate_key (scancode);
                
                if (translated_key == NULL)
                {
                  keyboard_packet.has_special_key = 1;
                  keyboard_packet.special_key =
                    special_key_conversion[scancode];
                }
                else
                {
                  keyboard_packet.has_character_code = 1;
                  string_copy (keyboard_packet.character_code, translated_key);
                }
                break;
              }
            }
          }
          
          break;
        }
      }
    }
    else
    {
      /* A key was released. Start by masking away the highest bit and
         update the keyboard_pressed_keys structure. */

      scancode &= 0x7F;
      keyboard_pressed_keys[scancode / 8] &= (~(1 << (scancode % 8)));

      /* If we have someone to send this to, do it. */

      if (keyboard_target_mailbox_id != MAILBOX_ID_NONE)
      {
        switch (scancode)
        {
          case SCAN_CODE_LEFT_SHIFT:
          {
            shift_state &= ~KEYBOARD_LEFT_SHIFT;
            break;
          }
          
          case SCAN_CODE_RIGHT_SHIFT:
          {
            shift_state &= ~KEYBOARD_RIGHT_SHIFT;
            break;
          }
          
          case SCAN_CODE_ALT:
          {
            shift_state &= ~KEYBOARD_RIGHT_ALT;
            break;
          }

          case SCAN_CODE_CONTROL:
          {
            shift_state &= ~KEYBOARD_RIGHT_CONTROL;
            break;
          }
      
          /* Anything else will be E-lectric. */
    
          default:
          {
            keyboard_packet.key_pressed = FALSE;
            translated_key = translate_key (scancode);
            
            /* If the key couldn't be translated, translate it in our own
               way. */
            
            if (translated_key == NULL)
            {
              keyboard_packet.has_special_key = 1;
              keyboard_packet.special_key = special_key_conversion[scancode];
            }
            else
            {
              keyboard_packet.has_character_code = 1;
              string_copy (keyboard_packet.character_code, translated_key);
            }
          }
        }
      }
    }
    
    if (keyboard_target_mailbox_id != MAILBOX_ID_NONE)
    {
      keyboard_packet.left_shift_down = 
        ((shift_state & KEYBOARD_LEFT_SHIFT) ==
         KEYBOARD_LEFT_SHIFT ? 1 : 0);
      keyboard_packet.right_shift_down = 
        ((shift_state & KEYBOARD_RIGHT_SHIFT) ==
         KEYBOARD_RIGHT_SHIFT ? 1 : 0);
      keyboard_packet.right_alt_down = 
        ((shift_state & KEYBOARD_RIGHT_ALT) ==
         KEYBOARD_RIGHT_ALT ? 1 : 0);
      keyboard_packet.right_control_down = 
        ((shift_state & KEYBOARD_RIGHT_CONTROL) ==
         KEYBOARD_RIGHT_CONTROL ? 1 : 0);
      
      /* Send the key to the receiver. */
      
      message_parameter.protocol = IPC_PROTOCOL_CONSOLE;
      message_parameter.message_class = IPC_CONSOLE_KEYBOARD_EVENT;
      message_parameter.block = FALSE;
      message_parameter.length = sizeof (keyboard_packet_type);
      message_parameter.data = &keyboard_packet;
      
      ipc_send (keyboard_target_mailbox_id, &message_parameter);
    }
  }
}	

/* Handler for the keyboard IRQ. */

void keyboard_irq_handler (void)
{
  system_thread_name_set ("Keyboard IRQ handler");

  /* Register our IRQ. Must be done by the IRQ handler thread, so we
     do it here. */

  if (system_call_irq_register (KEYBOARD_IRQ, "Keyboard controller")
      != STORM_RETURN_SUCCESS)
  {
    //log_print (&log_structure, LOG_URGENCY_EMERGENCY,
    //               "Could not allocate keyboard IRQ.");
    return;
  }

  while (TRUE)
  {
    system_call_irq_wait (KEYBOARD_IRQ);
    handle_event ();
    system_call_irq_acknowledge (KEYBOARD_IRQ);
  }
}

/* Initialise the keyboard. */

const char *keyboard_init (void)
{
  int status;

  /* Test the keyboard interface. This seems to be the only way to get
     it going.  If the test is successful a x55 is placed in the
     input buffer. */

  controller_write_command_word (CONTROLLER_COMMAND_SELF_TEST);
  if (controller_wait_for_input () != 0x55)
  {
    return "Keyboard failed self test.";
  }

  /* Perform a keyboard interface test.  This causes the controller
     to test the keyboard clock and data lines.  The results of the
     test are placed in the input buffer. */

  controller_write_command_word (CONTROLLER_COMMAND_KEYBOARD_TEST);
  if (controller_wait_for_input () != 0x00)
  {
    return "Keyboard interface failed self test.";
  }
  
  /* Enable the keyboard by allowing the keyboard clock to run. */

  controller_write_command_word (CONTROLLER_COMMAND_KEYBOARD_ENABLE);

  /* Reset keyboard. If the read times out then the assumption is that
     no keyboard is plugged into the machine. This defaults the
     keyboard to scan-code set 2.
    
     Set up to try again if the keyboard asks for RESEND. */

  do 
  {
    controller_write_output_word (KEYBOARD_COMMAND_RESET);
    status = controller_wait_for_input ();
    if (status == KEYBOARD_REPLY_ACK)
    {
      break;
    }

    if (status != KEYBOARD_REPLY_RESEND)
    {
      return "Keyboard reset failed, no ACK.";
    }
  } while (TRUE);

  if (controller_wait_for_input () != KEYBOARD_REPLY_POWER_ON_RESET)
  {
    return "Keyboard reset failed, bad reply.";
  }

  /* Set keyboard controller mode. During this, the keyboard should be
     in the disabled state.
     
     Set up to try again if the keyboard asks for RESEND. */

  do 
  {
    controller_write_output_word (KEYBOARD_COMMAND_DISABLE);
    status = controller_wait_for_input ();
    if (status == KEYBOARD_REPLY_ACK)
    {  
      break;
    }

    if (status != KEYBOARD_REPLY_RESEND)
    {
      return "Disable keyboard: no ACK.";
    }
  } while (TRUE);

  controller_write_command_word (CONTROLLER_COMMAND_WRITE_MODE);
  controller_write_output_word (CONTROLLER_MODE_KEYBOARD_INTERRUPT |
                                CONTROLLER_MODE_SYS |
                                CONTROLLER_MODE_DISABLE_MOUSE |
                                CONTROLLER_MODE_KCC);
  
  /* IBM Power-PC portables need this to use scan-code set 1 -- Cort */
  
  controller_write_command_word (CONTROLLER_COMMAND_READ_MODE);
  if ((controller_wait_for_input () & CONTROLLER_MODE_KCC) == 0) 
  {
    /* If the controller does not support conversion, Set the keyboard
       to scan-code set 1.  */

    controller_write_output_word (0xF0);
    controller_wait_for_input ();
    controller_write_output_word (0x01);
    controller_wait_for_input ();
  }

  /* Enable the keyboard. */

  controller_write_output_word (KEYBOARD_COMMAND_ENABLE);
  if (controller_wait_for_input () != KEYBOARD_REPLY_ACK)
  {
    return "Enable keyboard: no ACK.";
  }

  /* Set the repeat rate. */

  return keyboard_set_repeat_rate ();
}


/* Handle the connection to the console service. */

static void handle_connection (ipc_structure_type *ipc_structure)
{
  bool done = FALSE;
  message_parameter_type message_parameter;
  u8 *data;
  unsigned int data_size = 100;

  memory_allocate ((void **) &data, data_size);

  keyboard_target_mailbox_id = ipc_structure->output_mailbox_id;
  
  /* Main loop. We just handle the packets we get in the way we
     should. */

  while (!done)
  {
    message_parameter.protocol = IPC_PROTOCOL_KEYBOARD;
    message_parameter.message_class = IPC_CLASS_NONE;
    message_parameter.length = data_size;
    message_parameter.data = data;
    message_parameter.block = TRUE;

    if (ipc_receive (ipc_structure->input_mailbox_id,
                     &message_parameter, &data_size) != IPC_RETURN_SUCCESS)
    {
      continue;
    }

    switch (message_parameter.message_class)
    {
      /* Unregister a receiver. */
      
      case IPC_KEYBOARD_UNREGISTER_TARGET:
      {
        keyboard_target_mailbox_id = MAILBOX_ID_NONE;

        /* FIXME: Close the connection. */

        return;
      }
    }
  }
}

/* Main function for the keyboard handling. */

bool keyboard_main (void)
{
  console_structure_type console_structure;

  /* Update the keyboard LEDs. */

  keyboard_update_leds ();

  /* No keys pressed at startup. */

  memory_set_u8 ((u8 * volatile) &keyboard_pressed_keys, 0,
                 sizeof (keyboard_pressed_keys));

  /* Establish a connection to the console service. */

  if (console_init (&console_structure, &empty_tag,
                    IPC_CONSOLE_CONNECTION_CLASS_PROVIDER_KEYBOARD) !=
      CONSOLE_RETURN_SUCCESS)
  {
    return FALSE;
  }

  /* Main loop. */

  system_thread_name_set ("Handling connection");
  handle_connection (&console_structure.ipc_structure);
  return TRUE;
}
