/* $Id$ */
/* Abstract: Debug functions. */
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

#ifndef __STORM_IA32_DEBUG_H__
# define __STORM_IA32_DEBUG_H__

# include "config.h"

# if !OPTION_RELEASE

#  include <storm/generic/types.h>
#  include <storm/generic/defines.h>

#  if OPTION_OPENBSD
#   define DEBUG_ATTRIBUTE_TEXT           0x17
#   define DEBUG_ATTRIBUTE_BACKGROUND     0x07
#  elif OPTION_NETBSD
#   define DEBUG_ATTRIBUTE_TEXT           0x02
#   define DEBUG_ATTRIBUTE_BACKGROUND     0x07
#  elif OPTION_FREEBSD
#   define DEBUG_ATTRIBUTE_TEXT           0x0F
#   define DEBUG_ATTRIBUTE_BACKGROUND     0x07
#  else
#   define DEBUG_ATTRIBUTE_TEXT           0x07
#   define DEBUG_ATTRIBUTE_BACKGROUND     0x07
#  endif

#  define DEBUG_SCREEN_WIDTH              80
#  define DEBUG_SCREEN_HEIGHT             25
#  define BASE_SCREEN                     (0xB8000)

/* Typedefs. */

typedef struct
{ 
  char character;
  u8 attribute;
} __attribute__ ((packed)) debug_screen_type;

/* External variables. */

extern debug_screen_type *screen;

# endif /* !OPTION_RELEASE */

#endif /* !__STORM_IA32_DEBUG_H__ */
