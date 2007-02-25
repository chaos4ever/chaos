/* $Id$ */
/* Abstract: Function definitions for the console library. */
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

#ifndef __LIBRARY_CONSOLE_FUNCTIONS_H__
#define __LIBRARY_CONSOLE_FUNCTIONS_H__

#include <system/system.h>
#include <console/console.h>

extern return_type console_init 
  (console_structure_type *console_structure, tag_type *tag,
   unsigned int connection_class);

extern return_type console_open
  (console_structure_type *console_structure,
   unsigned int width, unsigned int height,
   unsigned int depth, int mode_type);

extern return_type console_mode_set 
  (console_structure_type *console_structure,
   unsigned int width, unsigned int height,
   unsigned int depth, int mode_type);

extern return_type console_resize
  (console_structure_type *console_structure,
   unsigned int width, unsigned int height);

extern return_type console_print
  (console_structure_type *console_structure,
   const char *string);

extern return_type console_print_formatted
  (console_structure_type *console_structure, const char *string, ...);
// __attribute__ ((format (printf, 1, 2)));
extern return_type console_clear (console_structure_type *console_structure);
extern return_type console_cursor_move
  (console_structure_type *console_structure, unsigned int x, unsigned int y);
extern return_type console_attribute_set
  (console_structure_type *console_structure, int foreground, int background,
   int attributes);
extern return_type console_cursor_appearance_set
  (console_structure_type *console_structure, bool visibility, bool block);
extern return_type console_use_keyboard
  (console_structure_type *console_structure, bool which, int type);
extern return_type console_use_mouse
  (console_structure_type *console_structure, bool which);
extern return_type console_event_wait
  (console_structure_type *console_structure, void *event_data, int *type,
   bool block);

#endif /* !__LIBRARY_CONSOLE_FUNCTIONS_H__ */
