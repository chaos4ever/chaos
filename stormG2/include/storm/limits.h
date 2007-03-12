/* $Id$ */
/* Abstract: Limitations on the kernel level. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

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

#ifndef __STORM_LIMITS_H__
#define __STORM_LIMITS_H__

#include <storm/types.h>

/* Limits of types. */

#define MAX_U8                          ((u8) -1)
#define MAX_U16                         ((u16) -1)
#define MAX_U32                         ((u32) -1)
#define MAX_U64                         ((u64) -1)
#define MAX_TIME                        ((time_type) -1)
#define MAX_ID                          ((id_type) -1)

/* Other limitations. */

#define MAX_PROCESS_NAME_LENGTH         128
#define MAX_THREAD_NAME_LENGTH          128

/* The maximum length of the kernel parameters. */

#define MAX_KERNEL_PARAMETER_LENGTH     256

/* The maximum number of servers to start at boot time. */

#define MAX_STARTUP_SERVERS             256

/* The maximum length of a thread state name. */

#define MAX_STATE_NAME_LENGTH           64

/* The maximum length of a protocol name. */

#define MAX_PROTOCOL_NAME_LENGTH        32

#endif /* !__STORM_LIMITS_H__ */
