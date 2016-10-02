/* $Id$ */
/* Abstract: Alignment functions. */
/* Author: Anders Ohrt <doa@chaosdev.org> */

/* Copyright 2000 chaos development. */

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

#ifndef __LIBRARY_SYSTEM_ALIGNMENT_H__
#define __LIBRARY_SYSTEM_ALIGNMENT_H__

#if defined __i386__

#define system_get_unaligned_u32(pointer) (*(uint32_t *) (pointer))
#define system_get_unaligned_uint16_t(pointer) (*(uint16_t *) (pointer))
#define system_get_unaligned_uint8_t(pointer) (*(uint8_t *) (pointer))

#define system_get_unaligned_s32(pointer) (*(int32_t *) (pointer))
#define system_get_unaligned_s16(pointer) (*(s16 *) (pointer))
#define system_get_unaligned_s8(pointer) (*(s8 *) (pointer))

#endif

#endif /* !__LIBRARY_SYSTEM_ALIGNMENT_H__ */
