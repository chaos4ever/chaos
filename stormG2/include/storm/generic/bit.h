/* $Id$ */
/* Abstract: Bit functions. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

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

#ifndef __STORM_GENERIC_BIT_H__
#define __STORM_GENERIC_BIT_H__

/* Little-endian system? */

#if (defined __i386__) || (defined __i486__) || (defined __i586__) || \
    (defined __i686__)

#define BIT_SET(a,b) ((a) |= (1 << (b)))
#define BIT_CLEAR(a,b) ((a) &= !(1 << (b)))
#define BIT_GET(a,b) ((a) & (1 << (b)) ? 1 : 0) 
#define BIT_IN_BYTES(a) ((a) % 8 != 0 ? (a) / 8 + 1 : (a) / 8)

/* Get the numeric value for bit x. */

#define BIT_VALUE(x) (1 << (x))

#else
#error FIXME: Write big-endian code
#endif

#endif /* !__STORM_GENERIC_BIT_H__ */
