/* $Id$ */
/* Abstract: Data structures used by the FAT server. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

/* This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#ifndef __FAT_H__
#define __FAT_H__

#include <system/system.h>

/* Much of this information can be found in the "FAT: General Overview
   of On-Disk Format" Hardware White Paper published by Microsoft. */

typedef struct
{
  /* jmp_boot has two valid ways to look like in a FAT BPB. Either
     0xEB, 0x??, 0x90 or 0xE9, 0x??, 0x??. */

  u8 jmp_boot[3];

  /* Creator system of the fat-drive. Usually looks like
     "MSWIN4.x". We write "chaos" here. */

  u8 oem_name[8];

  /* Can be 512, 1024, 2048 or 4096. Should be 512 if you don't want
     any trouble. At least, that's what Microsoft says. */

  u16 bytes_per_sector;
 
  /* Must be a power of two. The result of bytes_per_sector *
     sectors_per_cluster may not greater than 32K. If it is, many
     aplications will stop working. ;-) */

  u8 sectors_per_cluster;

  /* Number of reserved sectors in the reserved region of the volume
     starting at the first sector of the volume. Must be 1 for
     FAT12/16. Usually 32 for FAT32. */

  u16 reserved_sectors;

  /* Number of FAT structures. This value should always be 2. */

  u8 number_of_fats;

  /* For FAT12 and FAT16, this field contains the number of 32-byte
     directory entries in the root directory. It must also result in
     an even multiple of bytes_per_sector when multiplied by 32. For
     FAT32, this field must be set to zero. */

  u16 number_of_root_entries;

  /* This is the total number of sectors on the volume. If zero, the
     real value can be found in total_sectors_32. */
  
  u16 total_sectors_16;

  /* 0xF8 is the standard value for fixed media. For removable media,
     0xF0 is normally used. Legal values are 0xF0 and
     0xF8-0xFF. Whatever is put here must also be put into the low
     byte of the FAT[0] entry. Other than that, it's only here for
     compataibility. (It was used in DOS 1.x) */

  u8 media;

  /* The size of one FAT in 16-bit mode. In FAT32 this must be
     zero. */

  u16 fat_size_16;

  /* Sectors per track used on this media. This field isn't used by
     chaos. */

  u16 sectors_per_track;

  /* Number of heads used on this media. This field isn't used by
     chaos. */

  u16 number_of_heads;

  /* Number of hidden sectors. This field isn't used by chaos. */

  u32 hidden_sectors;

  /* Number of total sectors. If zero, the value in total_sectors_16
     is used. For FAT32 volumes, this must be specified. */

  u32 total_sectors_32;

  /* Here follows FAT12/16 or FAT32 specific data. */

  u8 data[0];
} __attribute__ ((packed)) bios_parameter_block_type;

/* FAT12/16 specific data in the BPB. */

typedef struct
{
  /* Drive number for BIOS. Isn't used by chaos. */

  u8 drive_number;

  /* Reserved for Windows NT. Should always be set to 0. */

  u8 reserved;

  /* Extended boot signature. If this is 0x29, the following three
     fields are present. */

  u8 boot_signature;

  /* Volume serial number. Should be fairly randomized. */

  u32 volume_id;

  /* Volume label. This field must be updated when the volume label in
     the root direcotry is updated. */

  u8 volume_label[11];

  /* One of the strings "FAT12 ", "FAT16 " or "FAT ". This can not be
     used to detemine the type of the FAT, but it should be updated
     when creating file systems. */

  u8 filesystem_type[8];
} __attribute__ ((packed)) bios_parameter_block_16_type;

/* FAT32 specific data. */

typedef struct
{
  /* This is the number of sectors for one FAT. */

  u32 fat_size;

  /* Extended FAT32 flags follow. */
  
  u16 reserved : 8;
  
  /* If this bit is clear, the FAT is mirrored at runtime into all
     FATs. If it set, only the one specified in the following field is
     active. */

  u16 mirroring : 1; 

  u16 reserved2 : 3;

  /* This specifies the "active" FAT mentioned previously. */

  u16 active_fat : 4;
  
  /* This specifies the file system version. High byte is major
     number, low byte is minor. The current version is 0.0. */

  u16 filesystem_version;
  
  /* This is set to the cluster number of the first cluster of the
     root directory. Usually 2, but not required. */

  u32 root_cluster;

  /* This specifies the sector number of the 'FSINFO' structure in the
     reserved area. */

  u16 filesystem_info;

  /* If zero, this specifies where the backup of the boot record can be
     found. Usually 6. No value other than 6 is recommended by
     Microsoft. */

  u16 backup_boot_record;

  /* The following area should always be set to zero when the volume
     is initialised. */

  u8 reserved3[12];

  /* Drive number for BIOS. Isn't used by chaos. */

  u8 drive_number;

  /* Reserved for Windows NT. Should always be set to 0. */

  u8 reserved4;

  /* Extended boot signature. If this is 0x29, the following three
     fields are present. */

  u8 boot_signature;

  /* Volume serial number. Should be fairly randomized. */

  u32 volume_id;

  /* Volume label. This field must be updated when the volume label in
     the root direcotry is updated. */

  u8 volume_label[11];

  /* One of the strings "FAT12 ", "FAT16 " or "FAT ". This can not be
     used to determine the type of the FAT, but it should be updated
     when creating file systems. */

  u8 filesystem_type[8];
} __attribute__ ((packed)) bios_parameter_block_32_type;

/* The time format used by FAT. */

typedef struct
{
  u16 hours : 5;
  u16 minutes : 6;
  
  /* The seconds are specified as the real value divided by two, so
     the range is really 0-29. */
  
  u16 seconds : 5;
} __attribute__ ((packed)) fat_time_format;

/* The date format used by FAT. */

typedef struct
{
  /* 1980 is added to the year, so the range is 1980-2107. */
  
  u16 year : 7;

  /* 1 = January, valid range 1-12. */

  u16 month : 4;

  /* Valid range 1-31. */
  
  u16 day : 5;
} __attribute__ ((packed)) fat_date_format;

/* A FAT directory entry. */

typedef struct
{
  /* The file name. If name[0] == 0xE5, this entry is free. If name[0]
     == 0x00, the rest of this directory is free. If name[0] == 0x05,
     the real value for this byte is 0xE5. This is because 0xE5 is
     used in the KANJI character set... */
  
  u8 name[8];
  u8 extension[3];
  
  /* File attributes. */

  u8 read_only : 1;
  u8 hidden : 1;
  u8 system : 1;
  u8 volume_id : 1;
  u8 directory : 1;
  u8 archive : 1;
  u8 reserved : 2;
  
  /* Reserved for Windows NT. Set to zero when creating, and never
     rely upon. */

  u8 nt_reserved;
  
  /* Time stamp the file was created. */

  u8 creation_hundreds;
  fat_time_format creation_time;
  fat_date_format creation_date;

  /* Last access date. Updated when the file is read from or written
     to. */

  fat_date_format access_date;

  /* High word of this entry's first cluster number. This should be
     zero for FAT12 or FAT16. */
  
  u16 first_cluster_number_high;

  /* Time for last update. */

  fat_time_format update_time;
  fat_date_format update_date;

  /* Low word of this entry's first cluster number. */

  u16 first_cluster_number_low;

  /* File size in bytes. */

  u32 file_size;

  /* Phony pointer to next FAT entry. */

  u8 next[0];
} __attribute__ ((packed)) fat_entry_type;

/* A LFN entry. */

typedef struct
{
  u8 name1[10];
  u8 signature;
  u8 attributes;
  u8 flags;
  u8 checksum;
  u8 name2[12];
  u16 first;
  u8 name3[4];
} __attribute ((packed)) long_file_name_entry_type;

/* The fat_info_type is used to hold information about one FAT
   volume. */

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

  /* The number of bits in this file allocation table. */

  unsigned int bits;
} fat_info_type;

typedef struct
{
  /* A 'file handle', in the VFS namespace. */

  file_handle_type file_handle;

  /* The starting cluster number for this file. */

  u32 start_cluster_number;

  /* The current cluster number that we are accessing. */

  u32 current_cluster_number;

  /* The current file position in bytes. */

  u32 file_position;

  /* The total size of this file in bytes. */

  u32 file_size;
} fat_open_file_type;

/* FAT types. */

typedef u8 fat12_type;
typedef u16 fat16_type;
typedef u32 fat32_type;

#define FAT12_READ_ENTRY(fat,index) (index % 2 == 0 ? \
                          *((u16 *) &fat[(index * 3) / 2]) & 0xFFF : \
                          *((u16 *) &fat[(index * 3) / 2]) >> 4)

#define FAT12_END_OF_CLUSTER_CHAIN      0xFF8
#define FAT12_BAD_CLUSTER               0xFF7

#define FAT16_END_OF_CLUSTER_CHAIN      0xFFF8

#define FAT32_END_OF_CLUSTER_CHAIN      0xFFFFFFF8

#define FAT_MAX_OPEN_FILES              16

#endif /* !__FAT_H__ */
