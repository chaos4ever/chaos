// Abstract: Support for reading directory entries.
// Authors: Per Lundberg <per@halleluja.nu>
//          Anders Öhrt <doa@chaosdev.org>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2015 chaos development

#include <string/string.h>

#include "fat.h"

// Read the specified directory entries.
bool fat_directory_entry_read(file_directory_entry_read_type *directory_entry_read, fat_info_type *fat_info)
{
    unsigned int elements = MAX_PATH_ELEMENTS;
    char *path[MAX_PATH_ELEMENTS];
    fat_entry_type *fat_entry;
    unsigned int entry = 0;
    unsigned int real_entry = 0;
    unsigned int output_entry = 0;
    unsigned int lfn_entries_in_row = 0;
    char long_file_name[MAX_FILE_NAME_LENGTH];
    bool has_lfn;

    // If the requested directory is a subdirectory, we need to read every parent directory in and everything. First, we split the
    // directory name into its logical components.
    path_split(directory_entry_read->path_name, path, &elements);

    if (!fat_directory_read(fat_info, path, elements, &fat_entry))
    {
        //    log_print (&log_structure, LOG_URGENCY_WARNING,
        //               "fat_directory_read failed");
        return FALSE;
    }

    // Now, get our entries. fat_entry will at this stage point to the whole directory.
    for (entry = 0; fat_entry[entry].name[0] != '\0'; entry++)
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
            string_copy_max(&filename[string_length(filename)],
                            fat_entry[entry].extension, 3);
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

        if (real_entry >= directory_entry_read->start_entry)
        {
            // Make sure we don't read more than we're supposed to.
            if (output_entry >= directory_entry_read->entries)
            {
                break;
            }

            if (has_lfn)
            {
                string_copy(directory_entry_read->entry[output_entry].name, long_file_name);
            }
            else
            {
                string_copy(directory_entry_read->entry[output_entry].name, filename);
            }

            if (fat_entry[entry].directory == 1)
            {
                directory_entry_read->entry[output_entry].type = FILE_ENTRY_TYPE_DIRECTORY;
            }
            else
            {
                directory_entry_read->entry[output_entry].type = FILE_ENTRY_TYPE_FILE;
            }
            output_entry++;
        }

        real_entry++;
    }

    if (fat_entry[entry].name[0] == 0x00)
    {
        directory_entry_read->end_reached = TRUE;
    }
    directory_entry_read->entries = output_entry;

    return TRUE;
}
