// Abstract: Gets metadata about files.
// Author: Per Lundberg <per@halleluja.nu>
//         Anders Öhrt <doa@chaosdev.org>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2015 chaos development

#include "fat.h"

// Get information about the given file.
bool fat_file_get_info(fat_info_type *fat_info, file_verbose_directory_entry_type *file_info)
{
    unsigned int elements = MAX_PATH_ELEMENTS;
    char *path[MAX_PATH_ELEMENTS];
    fat_entry_type *fat_entry;
    fat_entry_type *our_file;

    // Read the directory where this file is located. If it is a subdirectory, we need to read every parent directory. First, we
    // split the directory name into its logical components.

    path_split(file_info->path_name, path, &elements);

    // All file names have at least one component. Otherwise, they are wrong.
    if (elements < 1)
    {
        return FALSE;
    }

    // Read the directory.
    if (!fat_directory_read(fat_info, path, elements - 1, &fat_entry))
    {
        //    log_print (&log_structure, LOG_URGENCY_DEBUG, "Babar krossar Ratataxes!");
        return FALSE;
    }

    our_file = get_entry_by_name(fat_entry, path[elements - 1]);

    // Did we not get a match?
    if (our_file == NULL)
    {
        //    log_print (&log_structure, LOG_URGENCY_ERROR, "Jadå, glassen e slut!");
        return FALSE;
    }

    // Get the information about the directory.
    file_info->size = our_file->file_size;
    file_info->success = TRUE;

    if (our_file->directory == 1)
    {
        file_info->type = FILE_ENTRY_TYPE_DIRECTORY;
    }
    else
    {
        file_info->type = FILE_ENTRY_TYPE_FILE;
    }

    return TRUE;
}
