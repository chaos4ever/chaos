// Abstract: Handles long file name (LFN) support
// Author: Per Lundberg <per@chaosdev.io>
//         Anders Öhrt <doa@chaosdev.org>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2015 chaos development

#include "fat.h"

// Read the Long File Name for a FAT directory entry.
void read_long_file_name(fat_entry_type *fat_entry, int short_file_name_entry, int lfn_entries, char *long_file_name)
{
    int entry, characters_copied = 0;
    int character;
    long_file_name_entry_type *lfn;

    //  log_print_formatted (0, PACKAGE, "Reading %d LFN entries.", lfn_entries);

    for (entry = short_file_name_entry - 1;
         entry > (short_file_name_entry - 1) - lfn_entries;
         entry--)
    {
        if (!(fat_entry[entry].read_only == 1 &&
            fat_entry[entry].hidden == 1 &&
            fat_entry[entry].system == 1 &&
            fat_entry[entry].volume_id == 1))
        {
            // There wasn't any long file name for this entry. This is perfectly valid; it happens when you have files like COPYING
            // or README, in all-uppercase.
            break;
        }
        else if (fat_entry[entry].first_cluster_number_low != 0)
        {
            log_print(&log_structure, LOG_URGENCY_ERROR, "Corrupt LFN entry.");
            break;
        }
        else
        {
            lfn = (long_file_name_entry_type *) &(fat_entry[entry]);

            // FIXME: Convert those Unicode encoded characters correctly.
            // COMMENT: It isn't even real unicode, only 16 bit. Isn't it?. Rats!
            for (character = 0; character < 5; character++, characters_copied++)
            {
                long_file_name[characters_copied] = lfn->name1[character * 2 + 1];
            }

            for (character = 0; character < 6; character++, characters_copied++)
            {
                long_file_name[characters_copied] = lfn->name2[character * 2];
            }

            for (character = 0; character < 2; character++, characters_copied++)
            {
                long_file_name[characters_copied] = lfn->name3[character * 2];
            }
        }
    }
    long_file_name[characters_copied] = '\0';
}
