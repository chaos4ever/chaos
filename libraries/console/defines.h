/* $Id$ */
/* Abstract: Defines used by the console library. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

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

#ifndef __LIBRARY_CONSOLE_DEFINES_H__
#define __LIBRARY_CONSOLE_DEFINES_H__

/* Defines. */

/* Welcome, to the Real World. In this, we use EGA colors in the year
   of 2KAD... */

enum
{
  CONSOLE_COLOUR_BLACK,
  CONSOLE_COLOUR_RED,
  CONSOLE_COLOUR_GREEN,
  CONSOLE_COLOUR_CYAN,
  CONSOLE_COLOUR_BLUE,
  CONSOLE_COLOUR_PURPLE,
  CONSOLE_COLOUR_BROWN,
  CONSOLE_COLOUR_GRAY,
};

enum
{
  CONSOLE_ATTRIBUTE_RESET = 0,
  CONSOLE_ATTRIBUTE_BRIGHT = 1,
  CONSOLE_ATTRIBUTE_BLINK = 5,
};

enum
{
  CONSOLE_KEYBOARD_NORMAL,
  CONSOLE_KEYBOARD_EXTENDED
};

enum
{
  CONSOLE_EVENT_KEYBOARD,
  CONSOLE_EVENT_MOUSE
};

#endif /* !__LIBRARY_CONSOLE_DEFINES_H__ */
