/* $Id$ */
/* Abstract: Platform-independent defines local to storm. */
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

#ifndef __STORM_GENERIC_DEFINES_H__
#define __STORM_GENERIC_DEFINES_H__

#include <storm/generic/types.h>

#define STORM

/* Get the largest value of two. */

#define MAX_OF_TWO(a,b)                 ((a) > (b) ? (a) : (b))

/* Get the largest value of three. */

#define MAX_OF_THREE(a,b,c)             (MAX_OF_TWO ((a), \
                                         MAX_OF_TWO ((b), (c))))

/* This is a symbol inserted by the linker which tells us where the
   end of the ELF file is located when it's loaded into memory. We use
   it to figure out how much space the kernel uses, when mapping
   memory for newly created processes. */

extern void *_end;

/* The start and end of the initialisation code segment. */

extern void *_init_start;
extern void *_init_end;

/* This tells whether storm is finished booting or not. */

extern bool initialised; 

/* Get the external defines to. */

#include <storm/defines.h>

/* Now, get the architecture specifics. */

#include <storm/current-arch/defines.h>

#endif /* !__STORM_GENERIC_DEFINES_H__ */
