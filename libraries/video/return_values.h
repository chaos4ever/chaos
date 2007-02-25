/* $Id$ */
/* Abstract: Video library return values. */
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

#ifndef __LIBRARY_VIDEO_RETURN_TYPES_H__
#define __LIBRARY_VIDEO_RETURN_TYPES_H__

enum
{
  /* The function completed successfully. */

  VIDEO_RETURN_SUCCESS,

  /* No video service was found. */

  VIDEO_RETURN_SERVICE_UNAVAILABLE,

  /* The requested mode could not be set up. */

  VIDEO_RETURN_MODE_UNAVAILABLE,
};

#endif /* !__LIBRARY_VIDEO_RETURN_TYPES */
