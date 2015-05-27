// Abstract: Boot server. */
// Author: Per Lundberg <per@halleluja.nu>

// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2015 chaos development

#include "config.h"

// The maximum number of servers to load.
#define MAX_SERVERS 16

#define STARTUP_FILE "//ramdisk/config/servers/boot/startup"

static log_structure_type log_structure;

// An empty tag list.
tag_type empty_tag =
{
    0, 0, ""
};

int main(void)
{
    file_mount_type mount;
    mailbox_id_type mailbox_id[10];
    ipc_structure_type vfs_structure;
    message_parameter_type message_parameter;
    file_handle_type handle;
    file_verbose_directory_entry_type directory_entry;
    u8 *buffer;
    u8 **buffer_pointer = &buffer;
    char *server_name_buffer;
    char *server[MAX_SERVERS];
    unsigned int where, number_of_servers = 0, server_number;
    process_id_type process_id;
    unsigned int bytes_read;
    unsigned int services = 10;

    system_process_name_set(PACKAGE_NAME);
    system_thread_name_set("Initialising");

    if (log_init(&log_structure, PACKAGE_NAME, &empty_tag) !=
        LOG_RETURN_SUCCESS)
    {
        return -1;
    }

    log_print(&log_structure, LOG_URGENCY_DEBUG, "beginning of boot");

    // Mount the initial ramdisk as //ramdisk. To do this, we must first hook up a connection to the VFS service and
    // resolve the first block service.
    if (ipc_service_resolve("virtual_file_system", mailbox_id, &services, 5, &empty_tag) != IPC_RETURN_SUCCESS)
    {
        log_print(&log_structure, LOG_URGENCY_EMERGENCY, "Couldn't resolve the VFS service.");

        return -1;
    }

    vfs_structure.output_mailbox_id = mailbox_id[0];

    if (ipc_service_connection_request(&vfs_structure) != IPC_RETURN_SUCCESS)
    {
        log_print(&log_structure, LOG_URGENCY_EMERGENCY, "Couldn't connect to the VFS service.");

        return -1;
    }

    services = 1;

    if (ipc_service_resolve("block", mailbox_id, &services, 5, &empty_tag) != IPC_RETURN_SUCCESS)
    {
        log_print(&log_structure, LOG_URGENCY_EMERGENCY, "No block services found.");

        return -1;
    }

    // Obviously this needs to be specified but the neccessary code at the other end (virtual_file_system server) is
    // not there so it proved meaningless to add this yet... it just broke other code from compiling. (because I added
    // the field to the file_mount_type structure)
    // mount.mailbox_id = mailbox_id[0];
    string_copy(mount.location, "ramdisk");

    // That's it. Send the message. The receive is here just to ensure that we don't keep on going before the volume is
    // actually mounted.
    message_parameter.protocol = IPC_PROTOCOL_FILE;
    message_parameter.message_class = IPC_FILE_MOUNT_VOLUME;
    message_parameter.data = &mount;
    message_parameter.length = sizeof(file_mount_type);
    message_parameter.block = TRUE;
    ipc_send(vfs_structure.output_mailbox_id, &message_parameter);
    ipc_receive(vfs_structure.input_mailbox_id, &message_parameter, NULL);

    log_print(&log_structure, LOG_URGENCY_DEBUG, "Mounted the first available block service as //ramdisk.");

    // Now, read the list of servers to start from here.
    log_print(&log_structure, LOG_URGENCY_DEBUG, "Reading startup script...");
    string_copy(directory_entry.path_name, STARTUP_FILE);
    if (file_get_info(&vfs_structure, &directory_entry) != FILE_RETURN_SUCCESS)
    {
        log_print(&log_structure, LOG_URGENCY_ERROR, STARTUP_FILE " not found.");

        return -1;
    }

    char **server_name_pointer = &server_name_buffer;
    memory_allocate((void **) server_name_pointer, directory_entry.size);

    file_open(&vfs_structure, STARTUP_FILE, FILE_MODE_READ, &handle);
    file_read(&vfs_structure, handle, directory_entry.size, &server_name_buffer);

    // Parse the file.
    server[0] = &server_name_buffer[0];
    number_of_servers++;

    for (where = 1; where < directory_entry.size; where++)
    {

        if (server_name_buffer[where] == '\n')
        {
            server_name_buffer[where] = '\0';
            if (where + 1 < directory_entry.size)
            {
                server[number_of_servers] = &server_name_buffer[where + 1];
                number_of_servers++;
            }
        }
    }

    log_print_formatted(&log_structure, LOG_URGENCY_DEBUG, "Starting %u servers.", number_of_servers);

    for (server_number = 0; server_number < number_of_servers; server_number++)
    {
        log_print_formatted(&log_structure, LOG_URGENCY_INFORMATIVE, "Starting %s.", server[server_number]);

        string_copy(directory_entry.path_name, server[server_number]);
        if (file_get_info(&vfs_structure, &directory_entry) !=
            FILE_RETURN_SUCCESS)
        {
            log_print_formatted(&log_structure, LOG_URGENCY_ERROR,
                                "'%s' could not be accessed!",
                                server[server_number]);
            continue;
        }

        // Open the file.
        file_open(&vfs_structure, server[server_number], FILE_MODE_READ, &handle);

        log_print_formatted(&log_structure, LOG_URGENCY_DEBUG,
                            "Allocating %lu bytes for %s.",
                            directory_entry.size, server[server_number]);

        memory_allocate((void **) buffer_pointer, directory_entry.size);

        log_print_formatted(&log_structure, LOG_URGENCY_DEBUG, "Buffer is at %p.", buffer);

        bytes_read = 0;
        while (bytes_read < directory_entry.size)
        {
            unsigned int bytes;

            // Read the file.
            bytes = directory_entry.size - bytes_read;
            if (bytes > 32 * KB)
            {
                bytes = 32 * KB;
            }
            file_read(&vfs_structure, handle, bytes, &buffer[bytes_read]);
            bytes_read += bytes;
        }

        switch (execute_elf((elf_header_type *) buffer, "", &process_id))
        {
            case EXECUTE_ELF_RETURN_SUCCESS:
            {
                log_print_formatted(&log_structure, LOG_URGENCY_INFORMATIVE,
                                    "New process ID %lu.",
                                    process_id);
                break;
            }

            case EXECUTE_ELF_RETURN_IMAGE_INVALID:
            {
                log_print(&log_structure, LOG_URGENCY_ERROR, "Invalid ELF image.");
                break;
            }

            case EXECUTE_ELF_RETURN_ELF_UNSUPPORTED:
            {
                log_print(&log_structure, LOG_URGENCY_ERROR, "Unsupported ELF.");
                break;
            }

            case EXECUTE_ELF_RETURN_FAILED:
            {
                log_print(&log_structure, LOG_URGENCY_ERROR, "system_process_create failed.");
                break;
            }
        }

        memory_deallocate((void **) buffer_pointer);
    }

    system_call_process_parent_unblock();

    log_print(&log_structure, LOG_URGENCY_DEBUG, "end of boot");

    return 0;
}
