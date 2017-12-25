// Abstract: File library.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#include <file/file.h>
#include <string/string.h>

// Initialise the file library.
return_type file_init(ipc_structure_type *vfs_structure, tag_type *tag)
{
    mailbox_id_type mailbox_id[10];
    unsigned int services = 10;

    if (ipc_service_resolve("virtual_file_system", mailbox_id, &services, 5, tag) != IPC_RETURN_SUCCESS)
    {
        return FILE_RETURN_SERVICE_UNAVAILABLE;
    }

    vfs_structure->output_mailbox_id = mailbox_id[0];
    if (ipc_service_connection_request(vfs_structure) != IPC_RETURN_SUCCESS)
    {
        return FILE_RETURN_SERVICE_UNAVAILABLE;
    }

    return FILE_RETURN_SUCCESS;
}

// Read one or more entries from a directory in the file system.
return_type file_directory_entry_read(ipc_structure_type *vfs_structure, file_directory_entry_read_type *directory_entry)
{
    message_parameter_type message_parameter;

    message_parameter.protocol = IPC_PROTOCOL_FILE;
    message_parameter.message_class = IPC_FILE_DIRECTORY_ENTRY_READ;
    message_parameter.data = directory_entry;
    message_parameter.block = TRUE;

    message_parameter.length = sizeof(file_directory_entry_read_type);
    if (system_call_mailbox_send(vfs_structure->output_mailbox_id, &message_parameter) != STORM_RETURN_SUCCESS)
    {
        return FILE_RETURN_SERVICE_UNAVAILABLE;
    }
    message_parameter.length = (sizeof(file_directory_entry_read_type) +
                                sizeof(file_directory_entry_type) * directory_entry->entries);
    if (system_call_mailbox_receive(vfs_structure->input_mailbox_id, &message_parameter) != STORM_RETURN_SUCCESS)
    {
        return FILE_RETURN_SERVICE_UNAVAILABLE;
    }
    return FILE_RETURN_SUCCESS;
}

// Get information about the given entity.
return_type file_get_info(ipc_structure_type *vfs_structure, file_verbose_directory_entry_type *directory_entry)
{
    message_parameter_type message_parameter;

    message_parameter.protocol = IPC_PROTOCOL_FILE;
    message_parameter.message_class = IPC_FILE_GET_INFO;
    message_parameter.data = directory_entry;
    message_parameter.block = TRUE;
    message_parameter.length = sizeof(file_verbose_directory_entry_type);

    if (system_call_mailbox_send(vfs_structure->output_mailbox_id, &message_parameter) != STORM_RETURN_SUCCESS)
    {
        return FILE_RETURN_IPC_FAILED;
    }

    if (system_call_mailbox_receive(vfs_structure->input_mailbox_id, &message_parameter) != STORM_RETURN_SUCCESS)
    {
        return FILE_RETURN_IPC_FAILED;
    }

    if (directory_entry->success)
    {
        return FILE_RETURN_SUCCESS;
    }
    else
    {
        return FILE_RETURN_FILE_ABSENT;
    }
}

// Open a file.
return_type file_open(ipc_structure_type *vfs_structure, char *file_name, file_mode_type mode, file_handle_type *handle)
{
    message_parameter_type message_parameter;
    file_open_type open;

    string_copy_max(open.file_name, file_name, MAX_PATH_NAME_LENGTH);
    open.mode = mode;

    message_parameter.protocol = IPC_PROTOCOL_FILE;
    message_parameter.message_class = IPC_FILE_OPEN;
    message_parameter.data = &open;
    message_parameter.block = TRUE;
    message_parameter.length = sizeof(file_open_type);

    if (system_call_mailbox_send(vfs_structure->output_mailbox_id, &message_parameter) != STORM_RETURN_SUCCESS)
    {
        return FILE_RETURN_IPC_FAILED;
    }

    message_parameter.data = handle;
    message_parameter.length = sizeof(file_handle_type);

    if (system_call_mailbox_receive(vfs_structure->input_mailbox_id, &message_parameter) != STORM_RETURN_SUCCESS)
    {
        return FILE_RETURN_IPC_FAILED;
    }

    return FILE_RETURN_SUCCESS;
}

// Read from a file.
return_type file_read(ipc_structure_type *vfs_structure, file_handle_type file_handle, unsigned int length, uint8_t *buffer)
{
    message_parameter_type message_parameter;
    file_read_type read;

    read.file_handle = file_handle;
    read.bytes = length;

    message_parameter.protocol = IPC_PROTOCOL_FILE;
    message_parameter.message_class = IPC_FILE_READ;
    message_parameter.data = &read;
    message_parameter.block = TRUE;
    message_parameter.length = sizeof(file_read_type);

    if (system_call_mailbox_send(vfs_structure->output_mailbox_id, &message_parameter) != STORM_RETURN_SUCCESS)
    {
        return FILE_RETURN_IPC_FAILED;
    }

    message_parameter.data = buffer;
    message_parameter.length = read.bytes;

    if (system_call_mailbox_receive(vfs_structure->input_mailbox_id, &message_parameter) != STORM_RETURN_SUCCESS)
    {
        return FILE_RETURN_IPC_FAILED;
    }

    return FILE_RETURN_SUCCESS;
}
