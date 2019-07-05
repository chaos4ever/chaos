// Abstract: Handles long file name (LFN) support
// Authors: Per Lundberg <per@chaosdev.io>
//          Anders Öhrt <doa@chaosdev.org>
//
// © Copyright 1999 chaos development

#include <string/string.h>

#include "fat.h"

// Open a file.
bool fat_file_open(fat_info_type *fat_info, ipc_file_open_type *open)
{
    unsigned int elements = MAX_PATH_ELEMENTS;
    char *path[MAX_PATH_ELEMENTS];
    fat_entry_type *fat_entry;

    // Read the directory where this file is located. If it is a subdirectory, we need to read every parent directory. First, we
    // split the directory name into its logical components.
    path_split(open->file_name, path, &elements);

    // All file names have at least one component. Otherwise, they are wrong.
    if (elements < 1)
    {
        return FALSE;
    }

    // Read the directory.
    if (!fat_directory_read(fat_info, path, elements - 1, &fat_entry))
    {
        //    log_print (&log_structure, LOG_URGENCY_DEBUG,
        //               "Failed to read from directory.");
        return FALSE;
    }

    // The last element of a /path/to/file => the actual file name that
    // can be used for scanning the FAT entries for the directory in
    // question.
    const char *file_name = path[elements - 1];
    fat_entry_type *matching_fat_entry = get_entry_by_name(fat_entry, file_name);

    if (matching_fat_entry == NULL)
    {
        return FALSE;
    }

    // Get the starting cluster of the file.
    uint32_t cluster_number =
        ((matching_fat_entry->first_cluster_number_high << 16) +
         matching_fat_entry->first_cluster_number_low);

    // Add this file to our list of open files.
    fat_open_file[number_of_open_files].file_handle = open->file_handle;
    fat_open_file[number_of_open_files].start_cluster_number =
        fat_open_file[number_of_open_files].current_cluster_number = cluster_number;
    fat_open_file[number_of_open_files].file_position = 0;
    fat_open_file[number_of_open_files].file_size = matching_fat_entry->file_size;
    number_of_open_files++;

    return TRUE;
}
