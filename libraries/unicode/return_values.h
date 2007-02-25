/* $Id$ */
/* Abstract: Return values for unicode functions. */
/* Author: Anders Ohrt <doa@chaosdev.org> */

/* Copyright 2000 chaos development */

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

#ifndef __LIBRARY_UNICODE_RETURN_VALUES_H__
#define __LIBRARY_UNICODE_RETURN_VALUES_H__

enum
{
  /* The function completed successfully. */

  UNICODE_RETURN_SUCCESS,
  
  /* The buffer was too small to hold the data. */

  UNICODE_RETURN_BUFFER_TOO_SHORT,

  /* Some UCS-2/4/UTF-8 character was invalid. */

  UNICODE_RETURN_UCS2_INVALID,
  UNICODE_RETURN_UCS4_INVALID,
  UNICODE_RETURN_UTF8_INVALID
};

#endif /* !__LIBRARY_UNICODE_RETURN_VALUES_H__ */
