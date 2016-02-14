// Abstract: Handles IPC connections from elsewhere.
// Authors: Per Lundberg <per@chaosdev.io>
//          Anders Öhrt <doa@chaosdev.org>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2015 chaos development

#include <ipc/ipc.h>
#include <log/log.h>
#include <memory/memory.h>

#include "fat.h"

void handle_connection(mailbox_id_type *reply_mailbox_id)
{
    system_thread_name_set("Handling connection");

    message_parameter_type message_parameter;
    bool done = FALSE;
    bool mounted = FALSE;
    fat_info_type fat_info;
    ipc_structure_type ipc_structure;
    u8 *data;
    u8 **data_pointer = &data;
    unsigned int data_size = 16384;

    memory_allocate((void **) data_pointer, data_size);

    // Accept the connection.
    ipc_structure.output_mailbox_id = *reply_mailbox_id;
    if (ipc_connection_establish(&ipc_structure) != IPC_RETURN_SUCCESS)
    {
        return;
    }

    message_parameter.block = TRUE;

    while (!done)
    {
        message_parameter.data = data;
        message_parameter.protocol = IPC_PROTOCOL_FILE;
        message_parameter.message_class = IPC_CLASS_NONE;
        message_parameter.length = data_size;

        if (ipc_receive(ipc_structure.input_mailbox_id, &message_parameter, &data_size) != IPC_RETURN_SUCCESS)
        {
            continue;
        }

        switch (message_parameter.message_class)
        {
            // Read one or more directory entries.
            case IPC_FILE_DIRECTORY_ENTRY_READ:
            {
                if (mounted)
                {
                    file_directory_entry_read_type *directory_entry_read = (file_directory_entry_read_type *) data;

                    if (!fat_directory_entry_read(directory_entry_read, &fat_info))
                    {
                        directory_entry_read->entries = 0;
                    }
                    message_parameter.length =
                        sizeof(file_directory_entry_read_type) +
                        sizeof(file_directory_entry_type) *
                        directory_entry_read->entries ;
                    //          log_print_formatted (0, PACKAGE, "Successfully read %u entries",
                    //                               directory_entry_read->entries);
                    //          log_print_formatted (0, PACKAGE, "max: %u\n", directory_entry_read->entries);
                    ipc_send(ipc_structure.output_mailbox_id, &message_parameter);
                }
                break;
            }

            case IPC_FILE_GET_INFO:
            {
                if (mounted)
                {
                    file_verbose_directory_entry_type *directory_entry = (file_verbose_directory_entry_type *) data;

                    if (!fat_file_get_info(&fat_info, directory_entry))
                    {
                        return_type return_value = FILE_RETURN_FILE_ABSENT;

                        log_print_formatted(&log_structure, LOG_URGENCY_ERROR, "IPC_FILE_GET_INFO failed");
                        message_parameter.message_class = IPC_FILE_RETURN_VALUE;
                        message_parameter.data = &return_value;
                        message_parameter.length = sizeof(return_type);
                    }

                    ipc_send(ipc_structure.output_mailbox_id, &message_parameter);
                }
                break;
            }

            case IPC_FILE_MOUNT_VOLUME:
            {
                mailbox_id_type *mailbox = (mailbox_id_type *) data;

                fat_info.block_structure.output_mailbox_id = *mailbox;
                if (ipc_service_connection_request(&fat_info.block_structure) != IPC_RETURN_SUCCESS)
                {
                    break;
                }

                // Check if we have a FAT file system at this location.
                if (!detect_fat(&fat_info))
                {
                    log_print(&log_structure, LOG_URGENCY_ERROR, "No FAT filesystem detected.");
                    break;
                }

                mounted = TRUE;

                break;
            }

            case IPC_FILE_OPEN:
            {
                ipc_file_open_type *open = (ipc_file_open_type *) data;

                if (!fat_file_open(&fat_info, open))
                {
                    log_print(&log_structure, LOG_URGENCY_ERROR, "Failed to open file.");
                }

                ipc_send(ipc_structure.output_mailbox_id, &message_parameter);

                break;
            }

            case IPC_FILE_READ:
            {
                file_read_type *read = (file_read_type *) data;
                u8 *read_buffer;
                u8 **read_buffer_pointer = &read_buffer;

                memory_allocate((void **) read_buffer_pointer, read->bytes);

                if (!fat_file_read(&fat_info, read->file_handle, read_buffer, read->bytes))
                {
                    log_print(&log_structure, LOG_URGENCY_ERROR, "Failed to read from file.");
                }

                message_parameter.data = read_buffer;
                message_parameter.length = read->bytes;
#ifdef DEBUG
                log_print_formatted(&log_structure, LOG_URGENCY_DEBUG, "Sending %lu", read->bytes);
#endif
                ipc_send(ipc_structure.output_mailbox_id, &message_parameter);
                memory_deallocate((void **) read_buffer_pointer);
                break;
            }

            // Unsupported functions.
            case IPC_FILE_CLOSE:
            case IPC_FILE_SEEK:
            case IPC_FILE_WRITE:
            case IPC_FILE_ACL_READ:
            case IPC_FILE_ACL_WRITE:
            case IPC_FILE_UNMOUNT_VOLUME:
            default:
            {
                return_type return_value = IPC_RETURN_FILE_FUNCTION_UNSUPPORTED;

                message_parameter.data = &return_value;
                message_parameter.length = sizeof(return_type);

                ipc_send(ipc_structure.output_mailbox_id, &message_parameter);
                break;
            }
        }
    }
}

