/* $Id$ */
/* Abstract: Platform generic defines. */

/* Authors: Per Lundberg <plundis@chaosdev.org> 
            Henrik Hallin <hal@chaosdev.org> 
            Anders Öhrt <doa@chaosdev.org> */

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

#ifndef __STORM_GENERIC_DEFINES_H__
#define __STORM_GENERIC_DEFINES_H__

/* Get the external defines to. */

#include <storm/defines.h>

/* Now, get the architecture specifics. */

#include <storm/current-arch/defines.h>

/* Get the largest value of two. */

#define MAX_OF_TWO(a,b)                 ((a) > (b) ? (a) : (b))

/* Get the largest value of three. */

#define MAX_OF_THREE(a,b,c)             (MAX_OF_TWO ((a), \
                                         MAX_OF_TWO ((b), (c))))

/* Get the minimum of two values. */

#define MIN_OF_TWO(a,b)                 ((a) < (b) ? (a) : (b))

/* Get the minimum of three values. */

#define MIN_OF_THREE(a,b,c)             (MIN_OF_TWO ((a), \
                                         MIN_OF_TWO ((b), (c))))

/* This is a symbol inserted by the linker which tells us where the
   end of the ELF file is located when it's loaded into memory. */

extern void *_end;

/* The start and end of the initialisation code segment. */

extern void *_init_start;
extern void *_init_end;

/* Convert a pointer to a page index. */

#define GET_PAGE_NUMBER(x)              ((u32) (x) / SIZE_PAGE)

/* Get the low or high u16 of an u32. */

#define LOW_U16(x)                      ((x) & 0xFFFF)
#define HIGH_U16(x)                     ((x) >> 16)

/* Get the low or high u8 of an u16. */

#define LOW_U8(x)                       ((x) & 0xFF)
#define HIGH_U8(x)                      ((x) >> 8)

/* Use those defines to put the given function and variables in
   special sections which are later freed. */

#define INIT_CODE                       __attribute__ ((section (".text.init")))
#define INIT_DATA                       __attribute__ ((section (".data.init")))

/* Use this define to attribute that the function will not ever return. */

#define NORETURN                        __attribute__ ((noreturn))

/* Use this define to tell the compiler that an argument or variable
   is not used. */

#define UNUSED                          __attribute__ ((unused))

#endif /* !__STORM_GENERIC_DEFINES_H__ */
