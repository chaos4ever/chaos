/* $Id$ */
/* Abstract: Bit functions. */
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

#ifndef __LIBRARY_SYSTEM_BIT_H__
#define __LIBRARY_SYSTEM_BIT_H__

#define SYSTEM_BIT_SET(a,b) ((a) |= (1 << (b)))
#define SYSTEM_BIT_CLEAR(a,b) ((a) &= !(1 << (b)))
#define SYSTEM_BIT_GET(a,b) ((a) & (1 << (b)) ? 1 : 0) 
#define SYSTEM_BIT_IN_BYTES(a) ((a) % 8 != 0 ? (a) / 8 + 1 : (a) / 8)

/* Get the numeric value for bit x. */

#define SYSTEM_BIT_VALUE(x) (1 << (x))

#endif /* !__LIBRARY_SYSTEM_BIT_H__ */
