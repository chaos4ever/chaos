// Abstract: Handles FAT filesystem detection
// Author: Per Lundberg <per@halleluja.nu>
//         Anders Öhrt <doa@chaosdev.org>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2015 chaos development

#include <memory/memory.h>

#include "fat.h"

static u8 global_fat[16384];

// Detects wether there is a FAT file system on the given mailbox ID.
bool detect_fat(fat_info_type *fat_info)
{
    message_parameter_type message_parameter;
    u8 *sector = (u8 *) buffer;
    ipc_block_read_type ipc_block_read;

    //  log_print (&log_structure, LOG_URGENCY_DEBUG, "Detecting a FAT volume.");

    // Read the BPB (BIOS Parameter Block) for this volume.
    message_parameter.protocol = IPC_PROTOCOL_BLOCK;
    message_parameter.message_class = IPC_BLOCK_READ;
    message_parameter.block = TRUE;
    message_parameter.data = &ipc_block_read;
    message_parameter.length = sizeof(ipc_block_read_type);

    ipc_block_read.start_block_number = 0;
    ipc_block_read.number_of_blocks = 1;
    ipc_send(fat_info->block_structure.output_mailbox_id, &message_parameter);

    message_parameter.length = 1024;
    message_parameter.data = sector;
    ipc_receive(fat_info->block_structure.input_mailbox_id, &message_parameter, NULL);

    // Make sure this is a valid FAT filesystem.
    if (sector[510] == 0x55 &&
        sector[511] == 0xAA &&
        (bios_parameter_block->media == 0xF0 ||
        (bios_parameter_block->media >= 0xF8)))
    {
        // Now, get the size of this FAT (12, 16 or 32 bits).
        int fat_size, total_sectors, data_sectors, number_of_clusters, block_number;

        fat_info->root_directory_sectors = (((bios_parameter_block->number_of_root_entries * 32) +
                                             (bios_parameter_block->bytes_per_sector - 1)) /
                                            bios_parameter_block->bytes_per_sector);

        if (bios_parameter_block->fat_size_16 != 0)
        {
            fat_size = bios_parameter_block->fat_size_16;
        }
        else
        {
            bios_parameter_block_32 = (bios_parameter_block_32_type *) &bios_parameter_block->data;
            fat_size = bios_parameter_block_32->fat_size;
        }

        if (bios_parameter_block->total_sectors_16)
        {
            total_sectors = bios_parameter_block->total_sectors_16;
        }
        else
        {
            total_sectors = bios_parameter_block->total_sectors_32;
        }

        fat_info->first_data_sector = (bios_parameter_block->reserved_sectors +
                                       (bios_parameter_block->number_of_fats *
                                        fat_size) +
                                       fat_info->root_directory_sectors);

        data_sectors = total_sectors - fat_info->first_data_sector;
        fat_info->sectors_per_cluster = bios_parameter_block->sectors_per_cluster;
        fat_info->bytes_per_sector = bios_parameter_block->bytes_per_sector;
        fat_info->bytes_per_cluster = (fat_info->sectors_per_cluster *
                                       fat_info->bytes_per_sector);

        number_of_clusters = (data_sectors /
                              bios_parameter_block->sectors_per_cluster);

        if (number_of_clusters < 4085)
        {
            fat_info->bits = 12;
        }
        else if (number_of_clusters < 65525)
        {
            fat_info->bits = 16;
        }
        else
        {
            fat_info->bits = 32;
        }

        // Read the FAT.
        // FIXME: Should be done dynamically, and should support FAT32.
        message_parameter.data = &ipc_block_read;
        message_parameter.length = sizeof(ipc_block_read_type);

        ipc_block_read.start_block_number = 1;
        ipc_block_read.number_of_blocks = bios_parameter_block->fat_size_16;

        ipc_send(fat_info->block_structure.output_mailbox_id, &message_parameter);

        memory_set_u8((u8 *) &global_fat, 0, 16384);
        message_parameter.length = 16384;
        message_parameter.data = &global_fat;
        ipc_receive(fat_info->block_structure.input_mailbox_id, &message_parameter, NULL);
        fat_info->fat = &global_fat;

        // Also, read the root directory.
        block_number = (bios_parameter_block->reserved_sectors +
                        bios_parameter_block->number_of_fats *
                        bios_parameter_block->fat_size_16);

        // Read the block.
        message_parameter.protocol = IPC_PROTOCOL_BLOCK;
        message_parameter.message_class = IPC_BLOCK_READ;
        message_parameter.length = sizeof(ipc_block_read_type);
        message_parameter.data = &ipc_block_read;
        message_parameter.block = TRUE;

        //    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
        //                         "Reading %u sectors",
        //                         fat_info->root_directory_sectors);

        ipc_block_read.start_block_number = block_number;
        ipc_block_read.number_of_blocks = fat_info->root_directory_sectors;
        ipc_send(fat_info->block_structure.output_mailbox_id, &message_parameter);
        message_parameter.length = (fat_info->root_directory_sectors * fat_info->bytes_per_sector);
        message_parameter.data = &global_root;
        ipc_receive(fat_info->block_structure.input_mailbox_id, &message_parameter, NULL);
        fat_info->root = &global_root;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
