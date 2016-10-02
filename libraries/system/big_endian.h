/* $Id$ */
/* Abstract: Big endian functions. */
/* Author: Per Lundberg <per@chaosdev.io> */

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

#ifndef __LIBRARY_SYSTEM_BIG_ENDIAN_H__
#define __LIBRARY_SYSTEM_BIG_ENDIAN_H__

#define SYSTEM_BIG_ENDIAN

#define system_big_endian_to_native_uint64_t(x) ((uint64_t) x)
#define system_big_endian_to_native_u32(x) ((uint32_t) x)
#define system_big_endian_to_native_uint16_t(x) ((uint16_t) x)

#define system_little_endian_to_native_uint64_t(x) (system_byte_swap_uint64_t (x))
#define system_little_endian_to_native_u32(x) (system_byte_swap_uint32_t (x))
#define system_little_endian_to_native_uint16_t(x) (system_byte_swap_uint16_t (x))

#define system_native_to_big_endian_uint64_t(x) (x)
#define system_native_to_big_endian_u32(x) (x)
#define system_native_to_big_endian_uint16_t(x) (x)(system_byte_swap_uint16_t (x))

#define system_native_to_little_endian_uint64_t(x) (system_byte_swap_uint64_t (x))
#define system_native_to_little_endian_u32(x) (system_byte_swap_uint32_t (x))
#define system_native_to_little_endian_uint16_t(x) (system_byte_swap_uint16_t (x))

#endif /* !__LIBRARY_SYSTEM_LITTLE_ENDIAN_H__ */
