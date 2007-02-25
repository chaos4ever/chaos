/* $Id$ */
/* Abstract: Return values used by the console library. */
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

#ifndef __LIBRARY_CONSOLE_RETURN_VALUES_H__
#define __LIBRARY_CONSOLE_RETURN_VALUES_H__

/* Console error codes. */

enum
{
  /* The function completed successfully. */

  CONSOLE_RETURN_SUCCESS,

  /* One or more of the services required by this library was not
     available. */

  CONSOLE_RETURN_SERVICE_UNAVAILABLE,

  /* One or more of the input arguments was invalid. */

  CONSOLE_RETURN_INVALID_ARGUMENT,

  /* The console server returned unrecognized data. */

  CONSOLE_RETURN_BAD_DATA_RETURNED,

  /* The given console structure was already initialised. */

  CONSOLE_RETURN_ALREADY_INITIALISED,
};

#endif /* !__LIBRARY_CONSOLE_RETURN_VALUES_H__ */
