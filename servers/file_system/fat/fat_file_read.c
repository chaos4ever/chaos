// Abstract: Reads from files on a FAT file system
// Authors: Per Lundberg <per@chaosdev.io>
//          Anders Öhrt <doa@chaosdev.org>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2015 chaos development

#include <memory/memory.h>

#include "fat.h"

// Read from a previously opened file.
bool fat_file_read(fat_info_type *fat_info, file_handle_type file_handle, void *read_buffer, u32 bytes)
{
    unsigned int file;
    u32 cluster_number;
    u32 read_bytes = 0;

    // Find this file in our list of opened files.
    for (file = 0; file < number_of_open_files; file++)
    {
        if (file_handle == fat_open_file[file].file_handle)
        {
            break;
        }
    }

    if (file == number_of_open_files)
    {
        return FALSE;
    }

    cluster_number = fat_open_file[file].current_cluster_number;

    // If we start in the middle of a cluster, read the part we want into our buffer.
    if (fat_open_file[file].file_position % fat_info->bytes_per_cluster != 0)
    {
        u8 *extra_buffer;
        u8 **extra_buffer_pointer = &extra_buffer;
        unsigned int length = min_of_two(bytes, (fat_info->bytes_per_cluster -
                                         (fat_open_file[file].file_position %
                                         fat_info->bytes_per_cluster)));

        memory_allocate((void **) extra_buffer_pointer, length);

        read_single_cluster(fat_info, cluster_number, (void *) extra_buffer);
        memory_copy(read_buffer,
                    (u8 *)((u32) extra_buffer + fat_open_file[file].file_position % fat_info->bytes_per_cluster),
                    length);

        memory_deallocate((void **) extra_buffer_pointer);

        fat_open_file[file].file_position += length;
        read_bytes += length;

        // Are we at the end of the cluster?
        if (fat_open_file[file].file_position % fat_info->bytes_per_cluster == 0)
        {
            cluster_number = get_next_cluster(cluster_number, fat_info->fat, fat_info->bits);
        }
    }

    // Now, read the rest of the clusters we're after.
    // FIXME: Detect when a file ends in the middle of a cluster.
    while (read_bytes < bytes)
    {
        read_single_cluster(fat_info, cluster_number,
                            (void *)((u32) read_buffer + read_bytes));
        read_bytes += fat_info->bytes_per_cluster;
        fat_open_file[file].file_position +=
            min_of_two(bytes, fat_info->bytes_per_cluster);
        if (fat_open_file[file].file_position % fat_info->bytes_per_cluster == 0)
        {
            cluster_number = get_next_cluster(cluster_number, fat_info->fat, fat_info->bits);
        }
    }

    fat_open_file[file].current_cluster_number = cluster_number;

    return TRUE;
}
