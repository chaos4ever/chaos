// Abstract: Defines and type definitions for the virtual file system.
// Author: Per Lundberg <per@chaosdev.io>
//
// Copyright 1999 chaos development.

#pragma once

#include <file/file.h>
#include <system/system.h>

// The maximum number of simultaneous mounted volumes.
// FIXME: Should be dynamic.

#define MAX_VOLUMES     16

// In chaos, file systems are mounted under the meta-root, //. The UNIX-like directory tree is a completely separate name space
// from this. To make something appear in the directory tree, we use the function vfs_assign. One of the benefits of this is that a
// file system can be assigned to several places. Thus, this number needs to be much higher than MAX_VOLUMES.
#define MAX_ASSIGNS     256

// The maximum number of simultaneously opened files.
// FIXME: Should be dynamic.
#define MAX_FILE_HANDLES        16

// Local type definitions.
// A mount point.
typedef struct
{
    char location[MAX_PATH_NAME_LENGTH];
    char file_system_type[256];

    // Is this file system handled by the VFS server?
    bool handled_by_vfs;
    ipc_structure_type ipc_structure;
} mount_point_type;

// An "assign point", as mentioned before.
typedef struct
{
    char location[MAX_PATH_NAME_LENGTH];

    // The index into the vfs_mount_point structure.
    unsigned int volume;
} assign_point_type;
