// Abstract: Virtual file system protocol.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013-2017 chaos development

#pragma once

enum
{
    // Open a file and assign a file handle.
    IPC_VIRTUAL_FILE_SYSTEM_FILE_OPEN = (IPC_PROTOCOL_VIRTUAL_FILE_SYSTEM << 16),

    // Close the given file handle.
    IPC_VIRTUAL_FILE_SYSTEM_FILE_CLOSE,

    // Get information about the given file. (somewhat like 'stat' in UNIX)
    IPC_VIRTUAL_FILE_SYSTEM_FILE_GET_INFO,

    // Seek to the given position in the given file handle.
    IPC_VIRTUAL_FILE_SYSTEM_FILE_SEEK,

    // Read from the given file.
    IPC_VIRTUAL_FILE_SYSTEM_FILE_READ,

    // Write to the given file.
    IPC_VIRTUAL_FILE_SYSTEM_FILE_WRITE,

    // Read a number of directory entries.
    IPC_VIRTUAL_FILE_SYSTEM_DIRECTORY_ENTRY_READ,

    // Read the ACL:s for the given file name.
    IPC_VIRTUAL_FILE_SYSTEM_ACL_READ,

    // Write the ACL:s for the given file name.
    IPC_VIRTUAL_FILE_SYSTEM_ACL_WRITE,

    // Mount this volume in the logical tree. Only non-removable devices need to be mounted; removable devices are treated in some
    // kind of magic way. :-)
    IPC_VIRTUAL_FILE_SYSTEM_MOUNT,

    // Unmount this volume from the logical tree.
    IPC_VIRTUAL_FILE_SYSTEM_UNMOUNT,

    // Assign this volume in the logical tree.
    IPC_VIRTUAL_FILE_SYSTEM_ASSIGN,

    // De-assign the volume from the given "assign" point.
    IPC_VIRTUAL_FILE_SYSTEM_DEASSIGN,

    // Detect if the file system contained in the supplied mailbox ID (to a block service) is supported by this service.
    IPC_VIRTUAL_FILE_SYSTEM_DETECT,
};

enum
{
    // The call returned successfully.
    IPC_RETURN_VIRTUAL_FILE_SYSTEM_SUCCESS,

    // The requested function is not supported on this file system.
    IPC_RETURN_VIRTUAL_FILE_SYSTEM_FUNCTION_UNSUPPORTED,
};

typedef struct
{
    char meta_location[MAX_PATH_NAME_LENGTH];
    char location[MAX_PATH_NAME_LENGTH];
} virtual_file_system_assign_type;
