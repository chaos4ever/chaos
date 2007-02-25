/* $Id$ */
/* Abstract: File system protocol. */
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

#ifndef __LIBRARY_IPC_FILE_SYSTEM_H__
#define __LIBRARY_IPC_FILE_SYSTEM_H__

enum
{
  /* Return a status code. */

  IPC_FILE_SYSTEM_RETURN_VALUE = (IPC_PROTOCOL_FILE_SYSTEM << 16),

  /* Open a file and assign a file handle. */

  IPC_FILE_SYSTEM_FILE_OPEN,

  /* Close the given file handle. */

  IPC_FILE_SYSTEM_FILE_CLOSE,

  /* Get information about the given file. (somewhat like 'stat' in
     UNIX) */

  IPC_FILE_SYSTEM_FILE_GET_INFO,

  /* Seek to the given position in the given file handle. */

  IPC_FILE_SYSTEM_FILE_SEEK,
  
  /* Read from the given file. */

  IPC_FILE_SYSTEM_FILE_READ,
  
  /* Write to the given file. */

  IPC_FILE_SYSTEM_FILE_WRITE,

  /* Read a number of directory entries. */

  IPC_FILE_SYSTEM_DIRECTORY_ENTRY_READ,

  /* Read the ACL:s for the given file name. */

  IPC_FILE_SYSTEM_ACL_READ,
  
  /* Write the ACL:s for the given file name. */

  IPC_FILE_SYSTEM_ACL_WRITE,

  /* Mount this volume in the logical tree. Only non-removable devices
     need to be mounted; removable devices are treated in some kind of
     magic way. :-) */
  
  IPC_FILE_SYSTEM_MOUNT,
  
  /* Unmount this volume from the logical tree. */

  IPC_FILE_SYSTEM_UNMOUNT,
};

enum
{
  /* The call returned successfully. */

  IPC_RETURN_FILE_SYSTEM_SUCCESS,

  /* The requested function is not supported on this file system. */

  IPC_RETURN_FILE_SYSTEM_FUNCTION_UNSUPPORTED,
};

typedef unsigned int file_handle_type;

typedef struct
{
  /* The full path name of this file. */

  char file_name[MAX_PATH_NAME_LENGTH];

  /* In which mode do we want to open this file? */

  int mode;

  /* The file handle for this file. File handles are common for all
     processes. */

  file_handle_type file_handle;
} ipc_file_system_open_type;

#endif /* !__LIBRARY_IPC_FILE_SYSTEM_H__ */
