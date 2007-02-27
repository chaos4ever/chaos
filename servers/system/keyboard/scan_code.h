/* $Id$ */
/* Abstract: Keyboard scan codes. */
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

#ifndef __SCAN_CODE_H__
#define __SCAN_CODE_H__

enum
{
  SCAN_CODE_ESCAPE              = 1,
  SCAN_CODE_BACK_SPACE          = 14,
  SCAN_CODE_TAB                 = 15,
  SCAN_CODE_ENTER               = 28,
  SCAN_CODE_CONTROL             = 29,
  SCAN_CODE_LEFT_SHIFT          = 42,
  SCAN_CODE_RIGHT_SHIFT         = 54,
  SCAN_CODE_PRINT_SCREEN        = 55,
  SCAN_CODE_ALT                 = 56,
  SCAN_CODE_SPACE_BAR           = 57,      
  SCAN_CODE_CAPS_LOCK           = 58,
  SCAN_CODE_F1                  = 59,
  SCAN_CODE_F2                  = 60,
  SCAN_CODE_F3                  = 61,
  SCAN_CODE_F4                  = 62,
  SCAN_CODE_F5                  = 63,
  SCAN_CODE_F6                  = 64,
  SCAN_CODE_F7                  = 65,
  SCAN_CODE_F8                  = 66,
  SCAN_CODE_F9                  = 67,
  SCAN_CODE_F10                 = 68,
  SCAN_CODE_NUM_LOCK            = 69,
  SCAN_CODE_SCROLL_LOCK         = 70,
  SCAN_CODE_NUMERIC_7           = 71,
  SCAN_CODE_NUMERIC_8           = 72,
  SCAN_CODE_NUMERIC_9           = 73,
  SCAN_CODE_NUMERIC_MINUS       = 74,
  SCAN_CODE_NUMERIC_4           = 75,
  SCAN_CODE_NUMERIC_5           = 76,
  SCAN_CODE_NUMERIC_6           = 77,
  SCAN_CODE_NUMERIC_PLUS        = 78,
  SCAN_CODE_NUMERIC_1           = 79,
  SCAN_CODE_NUMERIC_2           = 80,
  SCAN_CODE_NUMERIC_3           = 81,
  SCAN_CODE_NUMERIC_0           = 82,
  SCAN_CODE_NUMERIC_DELETE      = 83,
  SCAN_CODE_F11                 = 87,
  SCAN_CODE_F12                 = 88,
  SCAN_CODE_LEFT_WINDOWS        = 125,
  SCAN_CODE_RIGHT_WINDOWS       = 126,
  SCAN_CODE_MENU                = 127,

  /* FIXME. */

  SCAN_CODE_HOME                = 100,
  SCAN_CODE_END                 = 101,
  SCAN_CODE_INSERT              = 102
};

#endif /* !__SCANCODE_H__ */
