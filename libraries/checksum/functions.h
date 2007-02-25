/* $Id$ */
/* Abstract: CRC32 library function prototypes. */
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

#ifndef __LIBRARY_CHECKSUM_FUNCTIONS_H__
#define __LIBRARY_CHECKSUM_FUNCTIONS_H__

#include <system/system.h>

extern u32 checksum_crc32 (u8 *block, unsigned int length);

void checksum_md5
  (void *input, unsigned int length, checksum_md5_digest_type *digest);

#endif /* !__LIBRARY_CHECKSUM_FUNCTIONS_H__ */
