// Abstract: Boot server. */
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#include "config.h"

// The maximum number of programs to load from the startup file.
#define MAX_PROGRAMS 16

#define STARTUP_FILE "//ramdisk/config/servers/boot/startup"

static log_structure_type log_structure;
static ipc_structure_type vfs_structure;
static char *program_list_buffer;
static char *programs[MAX_PROGRAMS];

// An empty tag list.
tag_type empty_tag =
{
    0, 0, ""
};

static void set_process_and_thread_name(void);
static bool resolve_log_service(void);
static bool resolve_vfs_service(void);
static bool resolve_initial_ramdisk_service(mailbox_id_type *initial_ramdisk_mailbox_id);
static void mount_initial_ramdisk(mailbox_id_type initial_ramdisk_mailbox_id);
static bool read_program_list(unsigned int *file_size);
static unsigned int parse_program_list(unsigned int file_size);
static void start_programs(unsigned int number_of_programs);

int main(void)
{
    set_process_and_thread_name();
    system_call_process_parent_unblock();

    if (!resolve_log_service())
    {
        return -1;
    }

    if (!resolve_vfs_service())
    {
        return -1;
    }

    mailbox_id_type initial_ramdisk_id;
    if (!resolve_initial_ramdisk_service(&initial_ramdisk_id))
    {
        return -1;
    }

    mount_initial_ramdisk(initial_ramdisk_id);

    unsigned int file_size = 0;
    if (!read_program_list(&file_size))
    {
        return -1;
    }

    unsigned int number_of_programs = parse_program_list(file_size);
    log_print_formatted(&log_structure, LOG_URGENCY_DEBUG, "Starting %u programs.", number_of_programs);

    start_programs(number_of_programs);
    log_print(&log_structure, LOG_URGENCY_DEBUG, "Boot server completed.");

    return 0;
}

static void set_process_and_thread_name()
{
    system_process_name_set(PACKAGE_NAME);
    system_thread_name_set("Initialising");
}

static bool resolve_log_service()
{
    return log_init(&log_structure, PACKAGE_NAME, &empty_tag) == LOG_RETURN_SUCCESS;
}

static bool resolve_vfs_service()
{
    if (file_init(&vfs_structure, &empty_tag) == FILE_RETURN_SUCCESS)
    {
        return TRUE;
    }
    else
    {
        log_print(&log_structure, LOG_URGENCY_EMERGENCY,
                  "Failed to connect to the VFS service. Is the virtual_file_system server running");

        return FALSE;
    }
}

static bool resolve_initial_ramdisk_service(mailbox_id_type *initial_ramdisk_mailbox_id)
{
    mailbox_id_type mailbox_id[1];
    unsigned int services = 1;

    if (ipc_service_resolve("block", mailbox_id, &services, 5, &empty_tag) == IPC_RETURN_SUCCESS)
    {
        *initial_ramdisk_mailbox_id = mailbox_id[0];
        return TRUE;
    }
    else
    {
        log_print(&log_structure, LOG_URGENCY_EMERGENCY, "No block services found.");

        return FALSE;
    }
}

static void mount_initial_ramdisk(mailbox_id_type initial_ramdisk_mailbox_id)
{
    file_mount_type mount;

    mount.mailbox_id = initial_ramdisk_mailbox_id;
    string_copy(mount.location, "ramdisk");

    message_parameter_type message_parameter;
    message_parameter.protocol = IPC_PROTOCOL_FILE;
    message_parameter.message_class = IPC_FILE_MOUNT_VOLUME;
    message_parameter.data = &mount;
    message_parameter.length = sizeof(file_mount_type);
    message_parameter.block = TRUE;
    ipc_send(vfs_structure.output_mailbox_id, &message_parameter);

    log_print_formatted(&log_structure, LOG_URGENCY_DEBUG, "Mounted initial ramdisk service (mailbox ID %u) as //ramdisk.", initial_ramdisk_mailbox_id);
}

static bool read_program_list(unsigned int *file_size)
{
    file_verbose_directory_entry_type directory_entry;

    log_print(&log_structure, LOG_URGENCY_DEBUG, "Reading list of programs to start");
    string_copy(directory_entry.path_name, STARTUP_FILE);
    if (file_get_info(&vfs_structure, &directory_entry) != FILE_RETURN_SUCCESS)
    {
        log_print(&log_structure, LOG_URGENCY_ERROR, STARTUP_FILE " not found.");
        return FALSE;
    }

    if (memory_allocate((void **) &program_list_buffer, directory_entry.size) != MEMORY_RETURN_SUCCESS)
    {
        log_print(&log_structure, LOG_URGENCY_ERROR, "Failed allocating memory for program list.");
        return FALSE;
    }

    file_handle_type handle;
    if (file_open(&vfs_structure, STARTUP_FILE, FILE_MODE_READ, &handle) != FILE_RETURN_SUCCESS)
    {
        log_print(&log_structure, LOG_URGENCY_ERROR, "Failed opening " STARTUP_FILE);
        return FALSE;
    }

    if (file_read(&vfs_structure, handle, directory_entry.size, program_list_buffer) != FILE_RETURN_SUCCESS)
    {
        log_print(&log_structure, LOG_URGENCY_ERROR, "Failed reading from " STARTUP_FILE);
        return FALSE;
    }

    *file_size = directory_entry.size;

    return TRUE;
}

static unsigned int parse_program_list(unsigned int file_size)
{
    unsigned int number_of_programs = 0;
    programs[number_of_programs++] = &program_list_buffer[0];

    // Convert the LF-terminated file into a buffer o NUL-terminated strings.
    for (unsigned int i = 1; i < file_size; i++)
    {
        if (program_list_buffer[i] == '\n')
        {
            program_list_buffer[i] = '\0';

            if (i + 1 < file_size)
            {
                // There are remaining characters in the buffer, so we need to create a new entry in the programs array.
                programs[number_of_programs] = &program_list_buffer[i + 1];
                number_of_programs++;
            }
        }
    }

    return number_of_programs;
}

static void start_programs(unsigned int number_of_programs)
{
    uint8_t *buffer;
    uint8_t **buffer_pointer = &buffer;
    process_id_type process_id;
    unsigned int bytes_read;

    for (unsigned int i = 0; i < number_of_programs; i++)
    {
        log_print_formatted(&log_structure, LOG_URGENCY_INFORMATIVE, "Starting %s.", programs[i]);

        file_verbose_directory_entry_type directory_entry;

        string_copy(directory_entry.path_name, programs[i]);
        if (file_get_info(&vfs_structure, &directory_entry) != FILE_RETURN_SUCCESS)
        {
            log_print_formatted(&log_structure, LOG_URGENCY_ERROR, "'%s' could not be accessed!", programs[i]);
            continue;
        }

        // Open the file.
        file_handle_type handle;
        if (file_open(&vfs_structure, programs[i], FILE_MODE_READ, &handle) != FILE_RETURN_SUCCESS)
        {
            log_print_formatted(&log_structure, LOG_URGENCY_ERROR, "Failed to open '%s'", programs[i]);
            continue;
        }

        log_print_formatted(&log_structure, LOG_URGENCY_DEBUG,
                            "Allocating %u bytes for %s.",
                            directory_entry.size, programs[i]);

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
                                    "New process ID %u.", process_id);
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
                log_print(&log_structure, LOG_URGENCY_ERROR, "system_call_process_create failed.");
                break;
            }
        }

        memory_deallocate((void **) buffer_pointer);
    }
}
