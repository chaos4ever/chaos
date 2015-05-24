 // Abstract: Support for reading one or more clusters of data.
// Authors: Per Lundberg <per@halleluja.nu>
//          Anders Öhrt <doa@chaosdev.org>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2015 chaos development

#include "fat.h"

// Get the next cluster number in the chain for the given cluster.
u32 get_next_cluster(u32 cluster_number, void *fat, int bits)
{
    switch (bits)
    {
        case 12:
        {
            fat12_type *fat12 = fat;
            u32 new_cluster_number = FAT12_READ_ENTRY(fat12, cluster_number);

            if (new_cluster_number == FAT12_BAD_CLUSTER ||
                    new_cluster_number >= FAT12_END_OF_CLUSTER_CHAIN)
            {
                return MAX_U32;
            }
            else
            {
                return new_cluster_number;
            }

            break;
        }

        case 16:
        {
            fat16_type *fat16 = fat;
            u32 new_cluster_number = fat16[cluster_number];

            if (new_cluster_number >= FAT16_END_OF_CLUSTER_CHAIN)
            {
                return MAX_U32;
            }
            else
            {
                return new_cluster_number;
            }
            break;
        }

        case 32:
        {
            fat32_type *fat32 = fat;
            u32 new_cluster_number = fat32[cluster_number];

            if (new_cluster_number >= FAT32_END_OF_CLUSTER_CHAIN)
            {
                return MAX_U32;
            }
            else
            {
                return new_cluster_number;
            }
            break;
        }
        default:
        {
            return MAX_U32;
        }
    }
}

// Read the contents of the given cluster chain, starting from start_cluster + skip, and going number_of_clusters or until the end
// of the chain, depending on which is encountered first. Returns the cluster number where we ended, or U32_MAX if we got to the
// end of the file/directory/whatever.
u32 read_clusters(fat_info_type *fat_info, void *output, u32 start_cluster, u32 skip, u32 number_of_clusters)
{
    u32 cluster_number = start_cluster;
    u32 clusters_read = 0;

    do
    {
        if (skip > 0)
        {
            skip--;
        }
        else
        {
            //      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
            //                           "Reading cluster number %lu", cluster_number);
            read_single_cluster(fat_info, cluster_number, (void *)
                                ((u32) output + (clusters_read *
                                                 fat_info->bytes_per_sector *
                                                 fat_info->sectors_per_cluster)));
            clusters_read++;
        }
        cluster_number = get_next_cluster(cluster_number, fat_info->fat, fat_info->bits);
    } while (cluster_number != MAX_U32 && clusters_read < number_of_clusters);

    return cluster_number;
}

// Read the given cluster to the given buffer.
bool read_single_cluster(fat_info_type *fat_info, u32 cluster_number, void *data_buffer)
{
    message_parameter_type message_parameter;
    ipc_block_read_type ipc_block_read;

    message_parameter.block = TRUE;
    message_parameter.protocol = IPC_PROTOCOL_BLOCK;
    message_parameter.message_class = IPC_BLOCK_READ;
    message_parameter.data = &ipc_block_read;
    message_parameter.length = sizeof(ipc_block_read_type);

    ipc_block_read.start_block_number = ((cluster_number - 2) * fat_info->sectors_per_cluster) +
                                        fat_info->first_data_sector;
    ipc_block_read.number_of_blocks = fat_info->sectors_per_cluster;
    ipc_send(fat_info->block_structure.output_mailbox_id, &message_parameter);

    message_parameter.data = data_buffer;
    message_parameter.length = fat_info->bytes_per_cluster;
    ipc_receive(fat_info->block_structure.input_mailbox_id, &message_parameter, NULL);
    return TRUE;
}
