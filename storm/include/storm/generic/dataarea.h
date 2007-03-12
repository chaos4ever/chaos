/* $Id$ */
/* Abstract: Definition of the data area which is used for
   communication between the startup code and the kernel. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1998-1999 chaos development. */

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

#ifndef __STORM_GENERIC_DATAAREA_H__
#define __STORM_GENERIC_DATAAREA_H__

#include <storm/generic/types.h>

/* Type definitions. */

typedef struct
{
  u8 display_type;
  u16 x_size;
  u16 y_size;
  u8 x_position;
  u8 y_position;
} __attribute__ ((packed)) dataarea_type;

extern dataarea_type dataarea;

#endif /* !__STORM_GENERIC_DATAAREA_H__ */
