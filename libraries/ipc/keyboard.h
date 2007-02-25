/* $Id$ */
/* Abstract: Protocol used when communicating with keyboard
   servers. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

/* Copyright chaos development 1999. */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#ifndef __LIBRARY_IPC_KEYBOARD_H__
#define __LIBRARY_IPC_KEYBOARD_H__

#include <system/system.h>

/* Keyboard IPC messages. */

enum 
{
  IPC_KEYBOARD_REGISTER_TARGET = (IPC_PROTOCOL_KEYBOARD << 16),
  IPC_KEYBOARD_UNREGISTER_TARGET,
  IPC_KEYBOARD_EVENT,
};

/* "Special keys" are keys that don't generate printable UTF-8
   sequences (like F1, Escape etc). Note that in chaos, some keys that
   generate printable characters in other systems (for example Tab)
   are treated differently. */

enum
{
  IPC_KEYBOARD_SPECIAL_KEY_ESCAPE,
  IPC_KEYBOARD_SPECIAL_KEY_BACK_SPACE,
  IPC_KEYBOARD_SPECIAL_KEY_TAB,
  IPC_KEYBOARD_SPECIAL_KEY_ENTER,
  IPC_KEYBOARD_SPECIAL_KEY_CONTROL,
  IPC_KEYBOARD_SPECIAL_KEY_LEFT_SHIFT,
  IPC_KEYBOARD_SPECIAL_KEY_RIGHT_SHIFT,
  IPC_KEYBOARD_SPECIAL_KEY_PRINT_SCREEN,
  IPC_KEYBOARD_SPECIAL_KEY_ALT,
  IPC_KEYBOARD_SPECIAL_KEY_SPACE_BAR,
  IPC_KEYBOARD_SPECIAL_KEY_CAPS_LOCK,
  IPC_KEYBOARD_SPECIAL_KEY_F1,
  IPC_KEYBOARD_SPECIAL_KEY_F2,
  IPC_KEYBOARD_SPECIAL_KEY_F3,
  IPC_KEYBOARD_SPECIAL_KEY_F4,
  IPC_KEYBOARD_SPECIAL_KEY_F5, 
  IPC_KEYBOARD_SPECIAL_KEY_F6,
  IPC_KEYBOARD_SPECIAL_KEY_F7,
  IPC_KEYBOARD_SPECIAL_KEY_F8,
  IPC_KEYBOARD_SPECIAL_KEY_F9,
  IPC_KEYBOARD_SPECIAL_KEY_F10,
  IPC_KEYBOARD_SPECIAL_KEY_F11,
  IPC_KEYBOARD_SPECIAL_KEY_F12,
  IPC_KEYBOARD_SPECIAL_KEY_NUM_LOCK,
  IPC_KEYBOARD_SPECIAL_KEY_SCROLL_LOCK,
  IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_7,
  IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_8,
  IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_9,
  IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_MINUS,
  IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_4,
  IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_5,
  IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_6,
  IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_PLUS,
  IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_1,
  IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_2,
  IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_3,
  IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_0,
  IPC_KEYBOARD_SPECIAL_KEY_NUMERIC_DELETE,
  IPC_KEYBOARD_SPECIAL_KEY_LEFT_WINDOWS,
  IPC_KEYBOARD_SPECIAL_KEY_RIGHT_WINDOWS,
  IPC_KEYBOARD_SPECIAL_KEY_MENU

#if FALSE
  IPC_KEYBOARD_SPECIAL_KEY_HOME,
  IPC_KEYBOARD_SPECIAL_KEY_END,
  IPC_KEYBOARD_SPECIAL_KEY_INSERT
#endif
};

/* A keyboard packet is what gets sent whenever a key is pressed or
   released. */

typedef struct
{
  /* Does this packet contain a character code? If not, it is just one
     of the shift states that has been modified, or perhaps a 'special
     key'. */

  bool has_character_code;

  /* Is this a special key? */

  bool has_special_key;
  
  /* Was the key pressed or released? */

  bool key_pressed;
  
  /* The shift states. */

  bool left_shift_down;
  bool right_shift_down;
  bool left_alt_down;
  bool right_alt_down;
  bool left_control_down;
  bool right_control_down;
  bool left_windows_down;
  bool right_windows_down;
  bool task_list_down;

  /* If has_special_key is TRUE, this field contains the key code for
     this key. */

  u32 special_key;

  /* Eight bytes for the character code, encoded with
     UTF-8. */
  
  u8 character_code[6];
} keyboard_packet_type;

#endif /* !__LIBRARY_IPC_KEYBOARD_H__ */
