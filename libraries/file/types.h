/* $Id$ */
/* Abstract: File library types.*/
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

#ifndef __LIBRARY_FILE_TYPES_H__
#define __LIBRARY_FILE_TYPES_H__

#include <ipc/ipc.h>
#include <system/system.h>

typedef unsigned int file_entry_type;
typedef unsigned int file_mode_type;

typedef struct
{
  /* The full path name of this file. */

  char file_name[MAX_PATH_NAME_LENGTH];

  /* In which mode do we want to open this file? */

  file_mode_type mode;
} file_open_type;

typedef struct
{
  file_handle_type file_handle;

  /* The amount of data we want. */

  unsigned int bytes;
} file_read_type;

typedef struct
{
  file_entry_type type;
  char name[MAX_FILE_NAME_LENGTH];
} file_directory_entry_type;

typedef struct
{
  /* The name of the directory to get information about. */

  char path_name[MAX_PATH_NAME_LENGTH];

  /* The directory entry to start at. */

  unsigned int start_entry;

  /* This boolean is set when we've reached the end of the given
     directory. */

  bool end_reached;

  /* The maximum number of entries to write to the entry array on input, and
     the actual number of written entries on output. */

  unsigned int entries;

  /* This magic lets us allocate this structure as sizeof
     (file_system_directory_entry_read_type) + sizeof
     (file_system_directory_entry_type) * entries. */

  file_directory_entry_type entry[0];
} file_directory_entry_read_type;

/* A verbose directory entry. Returned by
   IPC_FILE_SYSTEM_FILE_GET_INFO. */

typedef struct
{
  /* Input parameters. */
  /* The full path name of the entity to get information about. */

  char path_name[MAX_PATH_NAME_LENGTH];

  /* Output parameters. */
  /* Did the command succed? */

  bool success;

  /* The type of entity. */

  file_entry_type type;

  /* Specified as chaos time. */

  time_type time;

  /* Entity size. If a file or link, in bytes. If a directory, the
     number of files in the directory. */

  u32 size;
} file_verbose_directory_entry_type;

#endif /* !__LIBRARY_FILE_TYPES_H__ */
