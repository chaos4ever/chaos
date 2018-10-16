// Abstract: Data structures used by the FAT server.
// Authors: Per Lundberg <per@chaosdev.io>
//          Anders Öhrt <doa@chaosdev.org>
//
// © Copyright 1999 chaos development

#pragma once

#include <file/file.h>
#include <ipc/ipc.h>
#include <log/log.h>
#include <system/system.h>

// Much of this information can be found in the "FAT: General Overview of On-Disk Format" Hardware White Paper published by Microsoft.
typedef struct
{
    // jmp_boot has two valid ways to look like in a FAT BPB. Either 0xEB, 0x??, 0x90 or 0xE9, 0x??, 0x??.
    uint8_t jmp_boot[3];

    // Creator system of the fat-drive. Usually looks like "MSWIN4.x". We write "chaos" here.
    uint8_t oem_name[8];

    // Can be 512, 1024, 2048 or 4096. Should be 512 if you don't want any trouble. At least, that's
    // what Microsoft says.
    uint16_t bytes_per_sector;

    // Must be a power of two. The result of bytes_per_sector * sectors_per_cluster may not greater
    // than 32K. If it is, many applications will stop working. ;-)
    uint8_t sectors_per_cluster;

    // Number of reserved sectors in the reserved region of the volume starting at the first sector of
    // the volume. Typically, but not always, 1 for FAT12/16. Usually 32 for FAT32.
    uint16_t reserved_sectors;

    // Number of FAT structures. This value should always be 2.
    uint8_t number_of_fats;

    // For FAT12 and FAT16, this field contains the number of 32-byte directory entries in the root
    // directory. It must also result in an even multiple of bytes_per_sector when multiplied by 32.
    // For FAT32, this field must be set to zero.
    uint16_t number_of_root_entries;

    // This is the total number of sectors on the volume. If zero, the real value can be found in
    // total_sectors_32.
    uint16_t total_sectors_16;

    // 0xF8 is the standard value for fixed media. For removable media, 0xF0 is normally used. Legal
    // values are 0xF0 and 0xF8-0xFF. Whatever is put here must also be put into the low byte of the
    // FAT[0] entry. Other than that, it's only here for compataibility. (It was used in DOS 1.x)
    uint8_t media;

    // The size of one FAT in 16-bit mode. In FAT32 this must be zero.
    uint16_t fat_size_16;

    // Sectors per track used on this media. This field isn't used by chaos.
    uint16_t sectors_per_track;

    // Number of heads used on this media. This field isn't used by chaos.
    uint16_t number_of_heads;

    // Number of hidden sectors. This field isn't used by chaos.
    uint32_t hidden_sectors;

    // Number of total sectors. If zero, the value in total_sectors_16 is used. For FAT32 volumes,
    // this must be specified.
    uint32_t total_sectors_32;

    // Here follows FAT12/16 or FAT32 specific data.
    uint8_t data[0];
} __attribute__((packed)) bios_parameter_block_type;

// FAT12/16 specific data in the BPB.
typedef struct
{
    // Drive number for BIOS. Isn't used by chaos.
    uint8_t drive_number;

    // Reserved for Windows NT. Should always be set to 0.
    uint8_t reserved;

    // Extended boot signature. If this is 0x29, the following three fields are present.
    uint8_t boot_signature;

    // Volume serial number. Should be fairly randomized.
    uint32_t volume_id;

    // Volume label. This field must be updated when the volume label in the root directory is updated.
    uint8_t volume_label[11];

    // One of the strings "FAT12 ", "FAT16 " or "FAT ". This can not be used to determine the type
    // of the FAT, but it should be updated when creating file systems.
    uint8_t filesystem_type[8];
} __attribute__((packed)) bios_parameter_block_16_type;

// FAT32 specific data.
typedef struct
{
    // This is the number of sectors for one FAT.
    uint32_t fat_size;

    // Extended FAT32 flags follow.
    uint16_t reserved : 8;

    // If this bit is clear, the FAT is mirrored at runtime into all FATs. If it set, only the one
    // specified in the following field is active.
    uint16_t mirroring : 1;
    uint16_t reserved2 : 3;

    // This specifies the "active" FAT mentioned previously.
    uint16_t active_fat : 4;

    // This specifies the file system version. High byte is major number, low byte is minor. The current version is 0.0.
    uint16_t filesystem_version;

    // This is set to the cluster number of the first cluster of the root directory. Usually 2, but
    // not required.
    uint32_t root_cluster;

    // This specifies the sector number of the 'FSINFO' structure in the reserved area.
    uint16_t filesystem_info;

    // If zero, this specifies where the backup of the boot record can be found. Usually 6. No
    // value other than 6 is recommended by Microsoft.
    uint16_t backup_boot_record;

    // The following area should always be set to zero when the volume is initialised.
    uint8_t reserved3[12];

    // Drive number for BIOS. Isn't used by chaos.
    uint8_t drive_number;

    // Reserved for Windows NT. Should always be set to 0.
    uint8_t reserved4;

    // Extended boot signature. If this is 0x29, the following three fields are present.
    uint8_t boot_signature;

    // Volume serial number. Should be fairly randomized.
    uint32_t volume_id;

    // Volume label. This field must be updated when the volume label in the root direcotry
    // is updated.
    uint8_t volume_label[11];

    // One of the strings "FAT12 ", "FAT16 " or "FAT ". This can not be used to determine the type
    // of the FAT, but it should be updated when creating file systems.
    uint8_t filesystem_type[8];
} __attribute__((packed)) bios_parameter_block_32_type;

// The time format used by FAT.
typedef struct
{
    uint16_t hours : 5;
    uint16_t minutes : 6;

    // The seconds are specified as the real value divided by two, so the range is really 0-29.
    uint16_t seconds : 5;
} __attribute__((packed)) fat_time_format;

// The date format used by FAT.
typedef struct
{
    // 1980 is added to the year, so the range is 1980-2107.
    uint16_t year : 7;

    // 1 = January, valid range 1-12.
    uint16_t month : 4;

    // Valid range 1-31.
    uint16_t day : 5;
} __attribute__((packed)) fat_date_format;

// A FAT directory entry.
typedef struct
{
    // The file name. If name[0] == 0xE5, this entry is free. If name[0] == 0x00, the rest of this
    // directory is free. If name[0] == 0x05, the real value for this byte is 0xE5. This is because
    // 0xE5 is used in the KANJI character set...
    char name[8];
    char extension[3];

    // File attributes.
    uint8_t read_only : 1;
    uint8_t hidden : 1;
    uint8_t system : 1;
    uint8_t volume_id : 1;
    uint8_t directory : 1;
    uint8_t archive : 1;
    uint8_t reserved : 2;

    // Reserved for Windows NT. Set to zero when creating, and never rely upon.
    // FIXME: This is the case flag. We should support it, to properly handle lowercase file names on FAT volumes.
    // See https://github.com/chaos4ever/chaos/issues/107
    uint8_t nt_reserved;

    // Time stamp the file was created.
    uint8_t creation_hundreds;
    fat_time_format creation_time;
    fat_date_format creation_date;

    // Last access date. Updated when the file is read from or written to.
    fat_date_format access_date;

    // High word of this entry's first cluster number. This should be zero for FAT12 or FAT16.
    uint16_t first_cluster_number_high;

    // Time for last update.
    fat_time_format update_time;
    fat_date_format update_date;

    // Low word of this entry's first cluster number.
    uint16_t first_cluster_number_low;

    // File size in bytes.
    uint32_t file_size;

    // Phony pointer to next FAT entry.
    uint8_t next[0];
} __attribute__((packed)) fat_entry_type;

// A LFN entry.
typedef struct
{
    uint8_t name1[10];
    uint8_t signature;
    uint8_t attributes;
    uint8_t flags;
    uint8_t checksum;
    uint8_t name2[12];
    uint16_t first;
    uint8_t name3[4];
} __attribute((packed)) long_file_name_entry_type;

// The fat_info_type is used to hold information about one FAT volume.
typedef struct
{
    unsigned int sectors_per_cluster;
    unsigned int bytes_per_sector;
    unsigned int first_data_sector;
    unsigned int root_directory_sectors;
    unsigned int bytes_per_cluster;
    ipc_structure_type block_structure;
    void *fat;
    void *root;

    // The number of bits in this file allocation table.
    unsigned int bits;
} fat_info_type;

typedef struct
{
    // A 'file handle', in the VFS namespace.
    file_handle_type file_handle;

    // The starting cluster number for this file.
    uint32_t start_cluster_number;

    // The current cluster number that we are accessing.
    uint32_t current_cluster_number;

    // The current file position in bytes.
    uint32_t file_position;

    // The total size of this file in bytes.
    uint32_t file_size;
} fat_open_file_type;

// FAT types.
typedef uint8_t fat12_type;
typedef uint16_t fat16_type;
typedef uint32_t fat32_type;

// Defines
#define PACKAGE_NAME                    "fat"
#define PACKAGE_VERSION                 "0.0.1"

#define FAT12_READ_ENTRY(fat,index) (index % 2 == 0 ? \
                          *((uint16_t *) &fat[(index * 3) / 2]) & 0xFFF : \
                          *((uint16_t *) &fat[(index * 3) / 2]) >> 4)

#define FAT12_END_OF_CLUSTER_CHAIN      0xFF8
#define FAT12_BAD_CLUSTER               0xFF7

#define FAT16_END_OF_CLUSTER_CHAIN      0xFFF8

#define FAT32_END_OF_CLUSTER_CHAIN      0xFFFFFFF8

#define FAT_MAX_OPEN_FILES              16

// External variables
extern bios_parameter_block_type *bios_parameter_block;
extern bios_parameter_block_32_type *bios_parameter_block_32;
extern uint32_t buffer[256];
extern uint8_t directory_buffer[16384];
extern fat_open_file_type fat_open_file[FAT_MAX_OPEN_FILES];
extern uint8_t global_root[16384];
extern log_structure_type log_structure;
extern unsigned int number_of_open_files;

// Function prototypes
extern bool detect_fat(fat_info_type *fat_info);
extern bool fat_directory_entry_read(file_directory_entry_read_type *directory_entry_read,
    fat_info_type *fat_info);
extern bool fat_directory_read(fat_info_type *fat_info, char *path[], int elements,
    fat_entry_type **fat_entry);
extern bool fat_file_get_info(fat_info_type *fat_info, file_verbose_directory_entry_type *file_info);
extern bool fat_file_read(fat_info_type *fat_info, file_handle_type file_handle, void *read_buffer, uint32_t bytes);
extern bool fat_file_open(fat_info_type *fat_info, ipc_file_open_type *open);
extern fat_entry_type *get_entry_by_name(fat_entry_type *fat_entry, const char *name);
extern uint32_t get_next_cluster(uint32_t cluster_number, void *fat, int bits);
extern void handle_connection(mailbox_id_type *reply_mailbox_id);
extern void path_split(char *path_name, char **output, unsigned int *elements);
extern uint32_t read_clusters(fat_info_type *fat_info, void *output, uint32_t start_cluster,
    uint32_t skip, uint32_t number_of_clusters);
extern void read_long_file_name(fat_entry_type *fat_entry, int short_file_name_entry,
    int lfn_entries, char *long_file_name);
extern bool read_single_cluster(fat_info_type *fat_info, uint32_t cluster_number,
    void *data_buffer);

// Get the maximum of the two input variables.
static unsigned int min_of_two(unsigned int a, unsigned int b) UNUSED;
static unsigned int min_of_two(unsigned int a, unsigned int b)
{
    return (a > b) ? b : a;
}
