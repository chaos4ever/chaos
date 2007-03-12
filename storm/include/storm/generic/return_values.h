/* $Id$ */
/* Abstract: Return values. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

/* Copyright chaos development 1999-2000. */

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
   USA */

#ifndef __STORM_GENERIC_RETURN_VALUES_H__
#define __STORM_GENERIC_RETURN_VALUES_H__

#include <storm/return_values.h>

/* Internal return values. */
/* FIXME: Look over this and see that all is good. */

enum
{
  /* Function returned successfully. */
  
  RETURN_SUCCESS = 0,

  /* The AVL tree was broken. */

  RETURN_AVL_TREE_BROKEN,

  /* The node you tried to free, was not previously allocated. */

  RETURN_AVL_NODE_ALREADY_FREE,

  /* FIXME: Remove this when the kernel API has stabilised. */

  RETURN_FUNCTION_UNFINISHED,

  /* We tried to put something at the NULL descriptor. */

  RETURN_GDT_ENTRY_BAD,

  /* The ELF image is corrupted. */

  RETURN_ELF_INVALID,

  /* The ELF image is in an unsupported format. (bad word length, for
     example) */

  RETURN_ELF_UNSUPPORTED,

  /* One, or more, of the sections in this ELF image is not placed in
     the regions reserved for data and code. */

  RETURN_ELF_SECTION_MISPLACED,

  /* "Couldn't get a free page" :-) */

  RETURN_OUT_OF_MEMORY,

  /* We tried to reserve a page already in use. */
  
  RETURN_PAGE_USED,

  /* We tried to reserve a page that couldn't be found. */

  RETURN_PAGE_NOT_FOUND,

  /* The ports requested are not available. */

  RETURN_PORTS_UNAVAILABLE,

  /* No threads were unblocked. */

  RETURN_NO_THREAD_UNBLOCKED,

  /* The thread was unblocked successfully. */

  RETURN_THREAD_UNBLOCKED,
  
  /* We're out of threads. */

  RETURN_THREAD_UNAVAILABLE,

  /* The thread given to a function does not exist. */

  RETURN_THREAD_INVALID,

  /* One or more of the input arguments was invalid. */

  RETURN_INVALID_ARGUMENT,

  /* The requested memory region could not be deallocated, since it
     wasn't previously allocated. */

  RETURN_MEMORY_NOT_ALLOCATED,
};

#endif /* !__STORM_GENERIC_RETURN_VALUES_H__ */
