/* $Id$ */
/* Abstract: Checksum library types. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

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

#ifndef __LIBRARY_CHECKSUM_TYPES_H__
#define __LIBRARY_CHECKSUM_TYPES_H__

#include <system/system.h>

/* Type definitions. */

/* MD5 context. */

typedef struct
{
  /* State (ABCD). */

  u32 state[4];

  /* Number of bits, modulo 2^64 (lsb first). */

  u32 count[2];

  /* Input buffer. */

  unsigned char buffer[64];
} checksum_md5_context_type;

typedef char checksum_md5_digest_type[CHECKSUM_MD5_DIGEST_LENGTH];

#endif /* !__LIBRARY_CHECKSUM_TYPES_H__ */
