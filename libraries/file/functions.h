// Abstract: File library functions.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#pragma once

#include <ipc/ipc.h>
#include <system/system.h>

extern return_type file_init(ipc_structure_type *vfs_structure, tag_type *tag);
extern return_type file_directory_entry_read(ipc_structure_type *vfs_structure, file_directory_entry_read_type *directory_entry);
extern return_type file_get_info(ipc_structure_type *vfs_structure, file_verbose_directory_entry_type *directory_entry);
extern return_type file_open(ipc_structure_type *vfs_structure, char *file_name, file_mode_type mode, file_handle_type *handle);
extern return_type file_read(ipc_structure_type *vfs_structure, file_handle_type file_handle, unsigned int length, void *buffer);
