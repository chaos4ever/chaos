// Abstract: File system protocol.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013-2017 chaos development

#pragma once

enum
{
    // Return a status code.
    IPC_FILE_SYSTEM_RETURN_VALUE = (IPC_PROTOCOL_FILE_SYSTEM << 16),

    // Open a file and assign a file handle.
    IPC_FILE_SYSTEM_FILE_OPEN,

    // Close the given file handle.
    IPC_FILE_SYSTEM_FILE_CLOSE,

    // Get information about the given file. (somewhat like 'stat' in UNIX)
    IPC_FILE_SYSTEM_FILE_GET_INFO,

    // Seek to the given position in the given file handle.
    IPC_FILE_SYSTEM_FILE_SEEK,

    // Read from the given file.
    IPC_FILE_SYSTEM_FILE_READ,

    // Write to the given file.
    IPC_FILE_SYSTEM_FILE_WRITE,

    // Read a number of directory entries.
    IPC_FILE_SYSTEM_DIRECTORY_ENTRY_READ,

    // Read the ACL:s for the given file name.
    IPC_FILE_SYSTEM_ACL_READ,

    // Write the ACL:s for the given file name.
    IPC_FILE_SYSTEM_ACL_WRITE,

    // Mount this volume in the logical tree. Only non-removable devices need to be mounted; removable devices are treated in some
    // kind of magic way. :-)
    IPC_FILE_SYSTEM_MOUNT,

    // Unmount this volume from the logical tree.
    IPC_FILE_SYSTEM_UNMOUNT,
};

enum
{
    // The call returned successfully.
    IPC_RETURN_FILE_SYSTEM_SUCCESS,

    // The requested function is not supported on this file system.
    IPC_RETURN_FILE_SYSTEM_FUNCTION_UNSUPPORTED,
};

typedef unsigned int file_handle_type;

typedef struct
{
    // The full path name of this file.
    char file_name[MAX_PATH_NAME_LENGTH];

    // In which mode do we want to open this file?
    int mode;

    // The file handle for this file. File handles are common for all processes.
    file_handle_type file_handle;
} ipc_file_system_open_type;
