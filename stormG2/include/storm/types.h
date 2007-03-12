/* $Id$ */
/* Abstract: Standard types used in the chaos operating system. */

/* Authors: Per Lundberg <plundis@chaosdev.org> 
            Henrik Hallin <hal@chaosdev.org> */

/* Copyright 1998-2000 chaos development. */

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

#ifndef __STORM_TYPES_H__
#define __STORM_TYPES_H__

/* General types. Those should only be used when a fixed size variable
   (for example in IP headers); for speed performances, use int or
   unsigned whenever a generic type is desired. But if you do,
   remember to not take for granted that the maximum value is 2^32 - 1
   etcetera. */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

#ifndef __cplusplus
typedef int bool;
#endif

typedef unsigned long long u64;
typedef signed long long s64;

typedef int return_type;
typedef u32 id_type;
typedef id_type process_id_type;
typedef id_type cluster_id_type;
typedef id_type thread_id_type;
typedef unsigned int state_type;
typedef u64 time_type;
typedef id_type mailbox_id_type;
typedef volatile int spinlock_type;

#endif /* !__STORM_TYPES_H__ */
