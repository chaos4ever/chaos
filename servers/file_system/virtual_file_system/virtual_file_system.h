/* $Id$ */
/* Abstract: Defines and type definitions for the virtual file system. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

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
   USA. */

#ifndef __VIRTUAL_FILE_SYSTEM_H__
#define __VIRTUAL_FILE_SYSTEM_H__

#include <file/file.h>
#include <system/system.h>

/* The maximum number of simultaneous mounted volumes. Should be
   dynamic. */

#define MAX_VOLUMES     16

/* In chaos, file systems are mounted under the meta-root, //. The
   UNIX-like directory tree is a completely separate name space from
   this. To make something appear in the directory tree, we use the
   function vfs_assign. One of the benefits of this is that a file
   system can be assigned to several places. Thus, this number needs
   to be much higher than MAX_VOLUMES. */

#define MAX_ASSIGNS     256

/* The maximum number of simultaneously opened files. Should be
   dynamic. */

#define MAX_FILE_HANDLES        16

/* Local type definitions. */
/* A mount point. */

typedef struct 
{
  char location[MAX_PATH_NAME_LENGTH];
  char file_system_type[256];

  /* Is this file system handled by the VFS server? */

  bool handled_by_vfs;
  ipc_structure_type ipc_structure;
} mount_point_type;

/* An "assign point", as mentioned before. */

typedef struct
{
  char location[MAX_PATH_NAME_LENGTH];

  /* The index into the vfs_mount_point structure. */

  unsigned int volume;
} assign_point_type;

#endif /* !__VIRTUAL_FILE_SYSTEM_H__ */
