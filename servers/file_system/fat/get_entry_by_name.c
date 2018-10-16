// Abstract: Reads FAT entries matching a provided name.
// Authors: Per Lundberg <per@chaosdev.io>
//          Anders Öhrt <doa@chaosdev.org>
//
// © Copyright 1999 chaos development

#include <string/string.h>

#include "fat.h"

// Get a FAT entry matching the given name. Returns NULL if no matching entry is found.
fat_entry_type *get_entry_by_name(fat_entry_type *fat_entry, const char *name)
{
    unsigned int entry;
    bool done = FALSE;
    unsigned int lfn_entries_in_row = 0;
    char long_file_name[MAX_FILE_NAME_LENGTH];
    bool has_lfn;

    //  log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "name: %s", name);

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

        if (fat_entry[entry].case_flag & 0x08)
        {
            // Case flag bits include 0x08 => the file name should be lowercased.
            for (int i = 0; filename[i] != '\0'; i++)
            {
                // Extremely simplistic, inline conversion of ASCII only for now. I _think_
                // we get quite far with it though.
                if (filename[i] >= 'A' && filename[i] <= 'Z')
                {
                    filename[i] = filename[i] - 'A' + 'a';
                }
            }
        }

        // If we have a file extension, add it too.
        if (fat_entry[entry].extension[0] != 0)
        {
            unsigned int length = string_length(filename);
            filename[string_length(filename)] = '.';
            string_copy_max(&filename[string_length(filename)], fat_entry[entry].extension, 3);
            filename[length + string_length(fat_entry[entry].extension)] = '\0';

            if (fat_entry[entry].case_flag & 0x10)
            {
                // Case flag bits include 0x10 => the extension should be lowercased.
                for (int i = length + 1; filename[i] != '\0'; i++)
                {
                    // Extremely simplistic, inline conversion of ASCII only for now. I _think_
                    // we get quite far with it though.
                    if (filename[i] >= 'A' && filename[i] <= 'Z')
                    {
                        filename[i] = filename[i] - 'A' + 'a';
                    }
                }
            }
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
            if (string_compare(long_file_name, name) == 0)
            {
                //        log_print (&log_structure, LOG_URGENCY_DEBUG,
                //                   "Matched a long file name");
                done = TRUE;
            }
        }
        else
        {
            if (string_compare(filename, name) == 0)
            {
                //        log_print (&log_structure, LOG_URGENCY_DEBUG,
                //                   "Matched a short file name");
                done = TRUE;
            }
        }
    }

    if (!done)
    {
        return NULL;
    }

    if (entry > 0)
    {
        entry--;
    }

    return &fat_entry[entry];
}
