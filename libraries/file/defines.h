/* $Id$ */
/* Abstract: Defines used by the file library. */
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

#ifndef __LIBRARY_FILE_DEFINES_H__
#define __LIBRARY_FILE_DEFINES_H__

enum
{
  /* Open the file for reading. */

  FILE_MODE_READ,

  /* Open the file for writing (overwriting any previous contents). */

  FILE_MODE_WRITE,
};

/* Directory entry types. */

enum
{
  FILE_ENTRY_TYPE_DIRECTORY,
  FILE_ENTRY_TYPE_FILE,
  FILE_ENTRY_TYPE_SOFT_LINK,
  FILE_ENTRY_TYPE_HARD_LINK,
};

/* The maxium number of ACL:s for one entity. */

#define MAX_ACL                 16

/* The maximum number of elements in a path. */

#define MAX_PATH_ELEMENTS       16

#define PATH_NAME_SEPARATOR     '/'

#endif /* !__LIBRARY_FILE_DEFINES_H__ */
