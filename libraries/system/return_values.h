/* $Id$ */
/* Abstract: Return values used by the system library. */
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

#ifndef __LIBRARY_SYSTEM_RETURN_VALUES_H__
#define __LIBRARY_SYSTEM_RETURN_VALUES_H__

enum
{
  /* The call completed successfully. */

  SYSTEM_RETURN_SUCCESS,

  /* Those are returned by system_thread_create (), so that the caller
     knows how to distinguish between the old and the new thread. */

  SYSTEM_RETURN_THREAD_NEW,
  SYSTEM_RETURN_THREAD_OLD,

  /* Something went wrong in system_call_thread_create (), and we do
     not yet handle it properly. Please fix. */

  SYSTEM_RETURN_THREAD_CREATE_FAILED,
};

#endif /* !__LIBRARY_SYSTEM_RETURN_VALUES_H__ */
