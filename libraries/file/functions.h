/* $Id$ */
/* Abstract: File library functions. */
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

#ifndef __LIBRARY_FILE_FUNCTIONS_H__
#define __LIBRARY_FILE_FUNCTIONS_H__

#include <ipc/ipc.h>
#include <system/system.h>

extern return_type file_init 
  (ipc_structure_type *vfs_structure, tag_type *tag);

extern return_type file_directory_entry_read
  (ipc_structure_type *vfs_structure,
   file_directory_entry_read_type *directory_entry);

extern return_type file_get_info
  (ipc_structure_type *vfs_structure,
   file_verbose_directory_entry_type *directory_entry);

extern return_type file_open
  (ipc_structure_type *vfs_structure, char *file_name, file_mode_type mode,
   file_handle_type *handle);

extern return_type file_read
  (ipc_structure_type *vfs_structure, file_handle_type file_handle,
   unsigned int length, void *buffer);

#endif /* !__LIBRARY_FILE_FUNCTIONS_H__ */
