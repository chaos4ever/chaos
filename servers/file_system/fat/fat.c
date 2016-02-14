// Abstract: FAT file system server (entry point).
// Authors: Per Lundberg <per@chaosdev.io>
//          Anders Öhrt <doa@chaosdev.org>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2015 chaos development

#include "fat.h"

// FIXME: Put those in a structure and allocate for each thread so that we can support multiple FAT volumes.
bios_parameter_block_type *bios_parameter_block = (bios_parameter_block_type *) &buffer;
bios_parameter_block_32_type *bios_parameter_block_32;
u32 buffer[256];
u8 directory_buffer[16384];
fat_open_file_type fat_open_file[FAT_MAX_OPEN_FILES];
u8 global_root[16384];
log_structure_type log_structure;
unsigned int number_of_open_files = 0;

static tag_type empty_tag =
{
    0, 0, ""
};

// Split a path name to its components.
void path_split(char *path_name, char **output, unsigned int *elements)
{
    unsigned int index;
    unsigned int output_index = 0;

    for (index = 0; path_name[index] != 0 && output_index < *elements; index++)
    {
        if (path_name[index] == PATH_NAME_SEPARATOR && path_name[index + 1] != 0)
        {
            path_name[index] = '\0';
            output[output_index] = &path_name[index + 1];
            output_index++;
        }
    }

    *elements = output_index;
}

int main(void)
{
    ipc_structure_type ipc_structure;

    system_process_name_set(PACKAGE_NAME);
    system_thread_name_set("Initializing");

    if (log_init(&log_structure, PACKAGE_NAME, &empty_tag) != LOG_RETURN_SUCCESS)
    {
        return -1;
    }

    // Create our service.
    if (ipc_service_create("file_system", &ipc_structure, &empty_tag) != STORM_RETURN_SUCCESS)
    {
        log_print(&log_structure, LOG_URGENCY_EMERGENCY,
                  "Couldn't create a file system service.");
        return -1;
    }

    system_call_process_parent_unblock();

    system_thread_name_set("Service handler");
    while (TRUE)
    {
        mailbox_id_type reply_mailbox_id;

        ipc_service_connection_wait(&ipc_structure);
        reply_mailbox_id = ipc_structure.output_mailbox_id;

        system_thread_create((thread_entry_point_type *) handle_connection, &reply_mailbox_id);
    }
}
