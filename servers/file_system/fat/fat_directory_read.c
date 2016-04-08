// Abstract: Reads FAT directory entries
// Authors: Per Lundberg <per@chaosdev.io>
//          Anders Öhrt <doa@chaosdev.org>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2015-2016 chaos development

#include <memory/memory.h>
#include <string/string.h>

#include "fat.h"

// Read the specified directory.
bool fat_directory_read(fat_info_type *fat_info, char *path[], int elements, fat_entry_type **fat_entry)
{
    // If anything else than the root directory, we need to do some handy-work. Otherwise, it's pretty much a piece of cake.
    if (fat_info->bits == 12 || fat_info->bits == 16)
    {
        *fat_entry = (fat_entry_type *) &global_root;
    }
    else if (fat_info->bits == 32)
    {
        // We don't support FAT32 yet...
        return FALSE;
    }
    else
    {
        // Anything else is probably wrong too.
        return FALSE;
    }

    // There is more to it.

    //  log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
    //                       "elements: %u", elements);
    if (elements > 0)
    {
        int element;
        fat_entry_type *directory = (fat_entry_type *) &directory_buffer;

        memory_copy(directory, global_root, 16384);

        for (element = 0; element < elements; element++)
        {
            fat_entry_type *entry;

            entry = get_entry_by_name(directory, path[element]);
            if (entry == NULL)
            {
                break;
            }

            //      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "apa: %s",
            //                           entry->name);

            read_clusters(fat_info, &directory_buffer,
                          (entry->first_cluster_number_high << 16) +
                          entry->first_cluster_number_low,
                          0, MAX_U32);
        }

        *fat_entry = (fat_entry_type *) &directory_buffer;
        //    log_print (&log_structure, LOG_URGENCY_DEBUG, (*fat_entry)->name);

        return TRUE;
    }
    else
    {
        return TRUE;
    }
}
