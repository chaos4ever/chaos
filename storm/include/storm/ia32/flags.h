/* $Id$ */
/* Abstract: Definitions of the bits in the flag field. */
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

#ifndef __STORM_IA32_FLAGS_H__
#define __STORM_IA32_FLAGS_H__

#include <storm/generic/bit.h>

/* Flags in the EFLAGS register. See the Intel documentation for more
   information about what those does. */

enum
{
  FLAG_CARRY = (BIT_VALUE (0)),
  FLAG_SET = (BIT_VALUE (1)),
  FLAG_PARITY = (BIT_VALUE (2)),
  FLAG_ADJUST = (BIT_VALUE (4)),
  FLAG_ZERO = (BIT_VALUE (6)),
  FLAG_SIGN = (BIT_VALUE (7)),
  FLAG_TRAP = (BIT_VALUE (8)),
  FLAG_INTERRUPT_ENABLE = (BIT_VALUE (9)),
  FLAG_DIRECTION = (BIT_VALUE (10)),
  FLAG_OVERFLOW = (BIT_VALUE (11)),
  FLAG_IOPL_LOW = (BIT_VALUE (12)),
  FLAG_IOPL_HIGH = (BIT_VALUE (13)),
  FLAG_NESTED_TASK = (BIT_VALUE (14)),
  FLAG_RESUME_TASK = (BIT_VALUE (16)),
  FLAG_V8086_MODE = (BIT_VALUE (17)),
  FLAG_ALIGNMENT_CHECK = (BIT_VALUE (18)),
  FLAG_VIRTUAL_INTERRUPT = (BIT_VALUE (19)),
  FLAG_VIRTUAL_INTERRUPT_PENDING = (BIT_VALUE (20)),
  FLAG_ID = (BIT_VALUE (21))
};

#endif /* !__STORM_I38&_FLAGS_H__ */
