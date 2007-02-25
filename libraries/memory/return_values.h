/* $Id$ */
/* Abstract: Return values for the memory library. */
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

#ifndef __LIBRARY_MEMORY_RETURN_VALUES_H__
#define __LIBRARY_MEMORY_RETURN_VALUES_H__

enum
{
  /* The function call suceeded. */

  MEMORY_RETURN_SUCCESS,

  /* Out of memory. */
  
  MEMORY_RETURN_OUT_OF_MEMORY,

  /* The SLAB system we tried to access was broken. */

  MEMORY_RETURN_SLAB_BROKEN,
};

#endif /* !__LIBRARY_MEMORY_RETURN_VALUES_H__ */
