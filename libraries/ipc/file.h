/* $Id$ */
/* Abstract: File protocol. */
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

#ifndef __LIBRARY_IPC_FILE_H__
#define __LIBRARY_IPC_FILE_H__

/* Limits. */
/* FIXME: Remove these. */

#define MAX_PATH_NAME_LENGTH    1024
#define MAX_FILE_NAME_LENGTH    256

enum
{
  /* Return a status code. */

  IPC_FILE_RETURN_VALUE = (IPC_PROTOCOL_FILE << 16),

  /* Open a file and assign a file handle. */

  IPC_FILE_OPEN,

  /* Close the given file handle. */

  IPC_FILE_CLOSE,

  /* Get information about the given file. (somewhat like 'stat' in
     UNIX) */

  IPC_FILE_GET_INFO,

  /* Seek to the given position in the given file handle. */

  IPC_FILE_SEEK,
  
  /* Read from the given file. */

  IPC_FILE_READ,
  
  /* Write to the given file. */

  IPC_FILE_WRITE,

  /* Read a number of directory entries. */

  IPC_FILE_DIRECTORY_ENTRY_READ,

  /* Read the ACL:s for the given file name. */

  IPC_FILE_ACL_READ,
  
  /* Write the ACL:s for the given file name. */

  IPC_FILE_ACL_WRITE,

  /* Mount this volume in the logical tree. Only non-removable devices
     need to be mounted; removable devices are treated in some kind of
     magic way. :-) */
  
  IPC_FILE_MOUNT_VOLUME,
  
  /* Unmount this volume from the logical tree. */

  IPC_FILE_UNMOUNT_VOLUME,

  /* Assign this volume in the logical tree. */

  IPC_FILE_ASSIGN_VOLUME,

  /* De-assign the volume from the given "assign" point. */

  IPC_FILE_DEASSIGN_VOLUME,

  /* Detect if the file system contained in the supplied mailbox ID
     (to a block service) is supported by this service. */

  IPC_FILE_DETECT_VOLUME,
};

enum
{
  /* The call returned successfully. */

  IPC_RETURN_FILE_SUCCESS,

  /* The requested function is not supported on this file system. */

  IPC_RETURN_FILE_FUNCTION_UNSUPPORTED,
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
} ipc_file_open_type;

/* Type definitions. */

typedef struct
{
  /* The location where we want this volume to be mounted. */

  char location[MAX_PATH_NAME_LENGTH];

  /* The mailbox of the block device server for this volume. */

  mailbox_id_type mailbox_id;
} file_mount_type;

typedef struct
{
  char meta_location[MAX_PATH_NAME_LENGTH];
  char location[MAX_PATH_NAME_LENGTH];
} file_assign_type;

#endif /* !__LIBRARY_IPC_FILE_H__ */
