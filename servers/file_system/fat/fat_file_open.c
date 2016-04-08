// Abstract: Handles long file name (LFN) support
// Author: Per Lundberg <per@chaosdev.io>
//         Anders Öhrt <doa@chaosdev.org>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2015-2016 chaos development

#include <string/string.h>

#include "fat.h"

// Open a file.
bool fat_file_open(fat_info_type *fat_info, ipc_file_open_type *open)
{
    unsigned int elements = MAX_PATH_ELEMENTS;
    char *path[MAX_PATH_ELEMENTS];
    fat_entry_type *fat_entry;
    bool done = FALSE;
    u32 cluster_number;
    unsigned int entry = 0;
    unsigned int lfn_entries_in_row = 0;
    char long_file_name[MAX_FILE_NAME_LENGTH];
    bool has_lfn;

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

    // Now, parse the directory until we found our boy.
    for (entry = 0; fat_entry[entry].name[0] != 0x00 && !done; entry++)
    {
        int counter;
        char filename[13];

        // First of all, make sure this isn't a long file entry.
        if (fat_entry[entry].read_only == 1 &&
            fat_entry[entry].hidden == 1 &&
            fat_entry[entry].system == 1 &&
            fat_entry[entry].volume_id == 1)
        {
            lfn_entries_in_row++;
            continue;
        }

        // Also, make sure the file hasn't been deleted.
        if (fat_entry[entry].name[0] == 0xE5)
        {
            continue;
        }

        // We don't want the volume ID either.
        if (fat_entry[entry].volume_id == 1)
        {
            continue;
        }

        // Replace all spaces in the file name with zeroes, so that we get a NULL terminated string.
        for (counter = 0; counter < 8; counter++)
        {
            if (fat_entry[entry].name[counter] == 0x20)
            {
                fat_entry[entry].name[counter] = 0;
                break;
            }
        }

        for (counter = 0; counter < 3; counter++)
        {
            if (fat_entry[entry].extension[counter] == 0x20)
            {
                fat_entry[entry].extension[counter] = 0;
                break;
            }
        }

        string_copy_max(filename, fat_entry[entry].name, 8);

        // Make sure the string is zero terminated.
        filename[8] = 0;

        // If we have a file extension, add it too.
        if (fat_entry[entry].extension[0] != 0)
        {
            unsigned int length = string_length(filename);
            filename[string_length(filename)] = '.';
            string_copy_max(&filename[string_length(filename)], fat_entry[entry].extension, 3);
            filename[length + string_length(fat_entry[entry].extension)] = '\0';
        }

        // Get the long filename of the file (if any).
        read_long_file_name(fat_entry, entry, lfn_entries_in_row, long_file_name);
        lfn_entries_in_row = 0;

        if (long_file_name[0] != '\0')
        {
            has_lfn = TRUE;
        }
        else
        {
            has_lfn = FALSE;
        }

        if (has_lfn)
        {
            if (string_compare(long_file_name, path[elements - 1]) == 0)
            {
                done = TRUE;
            }
        }
        else
        {
            if (string_compare(filename, path[elements - 1]) == 0)
            {
                done = TRUE;
            }
        }
    }

    // The entry we are after is actually one lower than what we've got.
    if (entry > 0)
    {
        entry--;
    }

    // Did we fall through to the end of the list?
    if (fat_entry[entry].name[0] == '\0')
    {
        //    log_print (&log_structure, LOG_URGENCY_ERROR, "Jadå, glassen e slut!");
        return FALSE;
    }

    // Get the starting cluster of the file.
    cluster_number = ((fat_entry[entry].first_cluster_number_high << 16) +
                      fat_entry[entry].first_cluster_number_low);

    // Add this file to our list of open files.
    fat_open_file[number_of_open_files].file_handle = open->file_handle;
    fat_open_file[number_of_open_files].start_cluster_number =
        fat_open_file[number_of_open_files].current_cluster_number = cluster_number;
    fat_open_file[number_of_open_files].file_position = 0;
    fat_open_file[number_of_open_files].file_size = fat_entry[entry].file_size;
    number_of_open_files++;

    return TRUE;
}
