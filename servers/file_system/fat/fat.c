/* $Id$ */
/* Abstract: FAT file system server. */
/* Authors: Per Lundberg <plundis@chaosdev.org> 
            Anders Öhrt <doa@chaosdev.org> */

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

#include <file/file.h>
#include <ipc/ipc.h>
#include <log/log.h>
#include <memory/memory.h>
#include <string/string.h>
#include <system/system.h>

#include "config.h"
#include "fat.h"

/* FIXME: Put those in a structure and allocate for each thread so
   that we can support multiple FAT volumes. */

static u32 buffer[256];
static bios_parameter_block_type *bios_parameter_block =
  (bios_parameter_block_type *) &buffer;
static bios_parameter_block_32_type *bios_parameter_block_32;
//static bios_parameter_block_16_type *bios_parameter_block_16 =
// (bios_parameter_block_16_type *) &(bios_parameter_block.data);
static u8 global_fat[16384];
static u8 global_root[16384];
static u8 directory_buffer[16384];
static fat_open_file_type fat_open_file[FAT_MAX_OPEN_FILES];
static unsigned int number_of_open_files = 0;
static log_structure_type log_structure;
static tag_type empty_tag =
{
  0, 0, ""
};

/* Read the long file name FAT entries for the given FAT entry. */

static void read_long_file_name
  (fat_entry_type *fat_entry, int short_file_name_entry, int lfn_entries,
   char *long_file_name);

/* Get the maximum of the two input variables. */

static unsigned int min_of_two (unsigned int a, unsigned int b)
{
  return (a > b) ? b : a;
}

/* Split a path name to its components. */

static void path_split (char *path_name, char **output, unsigned int *elements)
{
  unsigned int index;
  unsigned int output_index = 0;

  for (index = 0; path_name[index] != 0 && output_index < *elements; index++)
  {
    if (path_name[index] == PATH_NAME_SEPARATOR &&
        path_name[index + 1] != 0)
    {
      path_name[index] = '\0';
      output[output_index] = &path_name[index + 1];
      output_index++;
    }
  }
  *elements = output_index;
}

/* Get the next cluster number in the chain for the given cluster. */

static u32 get_next_cluster (u32 cluster_number, void *fat, int bits)
{
  switch (bits)
  {
    case 12:
    {
      fat12_type *fat12 = fat;
      u32 new_cluster_number = FAT12_READ_ENTRY (fat12, cluster_number);

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

/* Get a FAT entry matching the given name. Returns NULL if no
   matching entry is found. */

static fat_entry_type *get_entry_by_name (fat_entry_type *fat_entry,
                                          char *name)
{
  unsigned int entry;
  bool done = FALSE;
  unsigned int lfn_entries_in_row = 0;
  char long_file_name[MAX_FILE_NAME_LENGTH];
  bool has_lfn;

  //  log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "name: %s", name);

  /* Now, parse the directory until we found our boy. */

  for (entry = 0; fat_entry[entry].name[0] != 0x00 && !done; entry++)
  {
    int counter;
    char filename[13];

    /* First of all, make sure this isn't a long file entry. */
    
    if (fat_entry[entry].read_only == 1 && 
        fat_entry[entry].hidden == 1 && 
        fat_entry[entry].system == 1 && 
        fat_entry[entry].volume_id == 1)
    {
      lfn_entries_in_row++;
      continue;
    }
    
    /* Also, make sure the file hasn't been deleted. */

    if (fat_entry[entry].name[0] == 0xE5)
    {
      continue;
    }

    /* We don't want the volume ID either. */

    if (fat_entry[entry].volume_id == 1)
    {
      continue;
    }

    /* Replace all spaces in the file name with zeroes, so that we
       get a NULL terminated string. */
    
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
    
    string_copy_max (filename, fat_entry[entry].name, 8);
    
    /* Make sure the string is zero terminated. */

    filename[8] = 0;

    /* If we have a file extension, add it too. */

    if (fat_entry[entry].extension[0] != 0)
    {
      unsigned int length = string_length (filename);
      filename[string_length (filename)] = '.';
      string_copy_max (&filename[string_length (filename)], 
                       fat_entry[entry].extension, 3);
      filename[length + string_length (fat_entry[entry].extension)] = '\0';
    }

    /* Get the long filename of the file (if any). */

    read_long_file_name (fat_entry, entry, lfn_entries_in_row, long_file_name);
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
      if (string_compare (long_file_name, name) == 0)
      {
        //        log_print (&log_structure, LOG_URGENCY_DEBUG, 
        //                   "Matched a long file name");
        done = TRUE;
      }
    }
    else
    {
      if (string_compare (filename, name) == 0)
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

/* Read the given cluster to the given buffer. */

static bool read_single_cluster (fat_info_type *fat_info, u32 cluster_number,
                                 void *data_buffer)
{
  message_parameter_type message_parameter;
  ipc_block_read_type ipc_block_read;

  message_parameter.block = TRUE;
  message_parameter.protocol = IPC_PROTOCOL_BLOCK;
  message_parameter.message_class = IPC_BLOCK_READ;
  message_parameter.data = &ipc_block_read;
  message_parameter.length = sizeof (ipc_block_read_type);

  ipc_block_read.start_block_number = 
    ((cluster_number - 2) * fat_info->sectors_per_cluster) +
    fat_info->first_data_sector;
  ipc_block_read.number_of_blocks = fat_info->sectors_per_cluster;
  ipc_send (fat_info->block_structure.output_mailbox_id, &message_parameter);
  
  message_parameter.data = data_buffer;
  message_parameter.length = fat_info->bytes_per_cluster;
  ipc_receive (fat_info->block_structure.input_mailbox_id, &message_parameter,
               NULL);
  return TRUE;
}

/* Read the contents of the given cluster chain, starting from
   start_cluster + skip, and going number_of_clusters or until the end
   of the chain, depending on which is encountered first. Returns the
   cluster number where we ended, or U32_MAX if we got to the end of
   the file/directory/whatever.  */

static u32 read_clusters (fat_info_type *fat_info, void *output,
                          u32 start_cluster, u32 skip,
                          u32 number_of_clusters)
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
      read_single_cluster (fat_info, cluster_number, (void *)
                           ((u32) output + (clusters_read *
                                            fat_info->bytes_per_sector *
                                            fat_info->sectors_per_cluster)));
      clusters_read++;
    }
    cluster_number = get_next_cluster (cluster_number, fat_info->fat,
                                       fat_info->bits);
  } while (cluster_number != MAX_U32 &&
           clusters_read < number_of_clusters);

  return cluster_number;
}

/* Read the specified directory. */

static bool fat_directory_read (fat_info_type *fat_info,
                                char *path[], int elements,
                                fat_entry_type **fat_entry)
{
  /* If anything else than the root directory, we need to do some
     handy-work. Otherwise, it's pretty much a piece of cake. */

  if (fat_info->bits == 12 || fat_info->bits == 16)
  {
    *fat_entry = (fat_entry_type *) &global_root;
  }
  else if (fat_info->bits == 32)
  {
    /* We don't support FAT32 yet... */
    
    return FALSE;
  }
  
  /* Anything else is probably wrong too. */
  
  else
  {
    return FALSE;
  }

  /* There is more to it. */

  //  log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, 
  //                       "elements: %u", elements);
  if (elements > 0)
  {
    int element;
    fat_entry_type *directory = (fat_entry_type *) &directory_buffer;

    memory_copy (directory, global_root, 16384);

    for (element = 0; element < elements; element++)
    {
      fat_entry_type *entry;

      entry = get_entry_by_name (directory, path[element]);
      if (entry == NULL)
      {
        break;
      }

      //      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "apa: %s", 
      //                           entry->name);

      read_clusters (fat_info, &directory_buffer,
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

/* Read the specified directory entries. */

static bool fat_directory_entry_read
  (file_directory_entry_read_type *directory_entry_read,
   fat_info_type *fat_info)
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

  /* If the requested directory is a subdirectory, we need to read
     every parent directory in and everything. First, we split the
     directory name into its logical components. */

  path_split (directory_entry_read->path_name, path, &elements);

  if (!fat_directory_read (fat_info, path, elements, &fat_entry))
  {
    //    log_print (&log_structure, LOG_URGENCY_WARNING, 
    //               "fat_directory_read failed");
    return FALSE;
  }
  
  /* Now, get our entries. fat_entry will at this stage point to the
     whole directory. */

  for (entry = 0; fat_entry[entry].name[0] != '\0'; entry++)
  {
    int counter;
    char filename[13];

    /* First of all, make sure this isn't a long file entry. */
    
    if (fat_entry[entry].read_only == 1 && 
        fat_entry[entry].hidden == 1 && 
        fat_entry[entry].system == 1 && 
        fat_entry[entry].volume_id == 1)
    {
      lfn_entries_in_row++;
      continue;
    }
    
    /* Also, make sure the file hasn't been deleted. */

    if (fat_entry[entry].name[0] == 0xE5)
    {
      continue;
    }

    /* We don't want the volume ID either. */

    if (fat_entry[entry].volume_id == 1)
    {
      continue;
    }

    /* Replace all spaces in the file name with zeroes, so that we
       get a NULL terminated string. */
    
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
    
    string_copy_max (filename, fat_entry[entry].name, 8);
    
    /* Make sure the string is zero terminated. */

    filename[8] = 0;

    /* If we have a file extension, add it too. */

    if (fat_entry[entry].extension[0] != 0)
    {
      unsigned int length = string_length (filename);
      filename[string_length (filename)] = '.';
      string_copy_max (&filename[string_length (filename)], 
                       fat_entry[entry].extension, 3);
      filename[length + string_length (fat_entry[entry].extension)] = '\0';
    }

    /* Get the long filename of the file (if any). */

    read_long_file_name (fat_entry, entry, lfn_entries_in_row, long_file_name);
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
      /* Make sure we don't read more than we're supposed to. */
      
      if (output_entry >= directory_entry_read->entries)
      {
        break;
      }
      
      if (has_lfn)
      {
        string_copy (directory_entry_read->entry[output_entry].name,
                     long_file_name);
      }
      else
      {
        string_copy (directory_entry_read->entry[output_entry].name,
                     filename);
      }

      if (fat_entry[entry].directory == 1)
      {
        directory_entry_read->entry[output_entry].type =
          FILE_ENTRY_TYPE_DIRECTORY;
      }
      else
      {
        directory_entry_read->entry[output_entry].type =
          FILE_ENTRY_TYPE_FILE;
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

/* Get information about the given file. */

static bool fat_file_get_info (fat_info_type *fat_info,
                               file_verbose_directory_entry_type *file_info)
{
  unsigned int elements = MAX_PATH_ELEMENTS;
  char *path[MAX_PATH_ELEMENTS];
  fat_entry_type *fat_entry;
  fat_entry_type *our_file;

  /* Read the directory where this file is located. If it is a
     subdirectory, we need to read every parent directory.  First, we
     split the directory name into its logical components. */

  path_split (file_info->path_name, path, &elements);

  /* All file names have at least one component. Otherwise, they are
     wrong. */

  if (elements < 1)
  {
    return FALSE;
  }

  /* Read the directory. */

  if (!fat_directory_read (fat_info, path, elements - 1, &fat_entry))
  {
    //    log_print (&log_structure, LOG_URGENCY_DEBUG, "Babar krossar Ratataxes!");
    return FALSE;
  }

  our_file = get_entry_by_name (fat_entry, path[elements - 1]);

  /* Did we not get a match? */

  if (our_file == NULL)
  {
    //    log_print (&log_structure, LOG_URGENCY_ERROR, "Jadå, glassen e slut!");
    return FALSE;
  }
  
  /* Get the information about the directory. */

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

/* Open a file. */

static bool fat_file_open (fat_info_type *fat_info, 
                           ipc_file_open_type *open)
{
  unsigned int elements = MAX_PATH_ELEMENTS;
  char *path[MAX_PATH_ELEMENTS];
  fat_entry_type *fat_entry;
  bool done = FALSE;
  u32 cluster_number;
  unsigned int entry = 0;
  unsigned int lfn_entries_in_row = 0;
  char long_file_name[MAX_FILE_NAME_LENGTH];
  bool has_lfn;

  /* Read the directory where this file is located. If it is a
     subdirectory, we need to read every parent directory.  First, we
     split the directory name into its logical components. */

  path_split (open->file_name, path, &elements);

  /* All file names have at least one component. Otherwise, they are
     wrong. */

  if (elements < 1)
  {
    return FALSE;
  }

  /* Read the directory. */

  if (!fat_directory_read (fat_info, path, elements - 1, &fat_entry))
  {
    //    log_print (&log_structure, LOG_URGENCY_DEBUG, 
    //               "Failed to read from directory.");
    return FALSE;
  }

  /* Now, parse the directory until we found our boy. */

  for (entry = 0; fat_entry[entry].name[0] != 0x00 && !done; entry++)
  {
    int counter;
    char filename[13];

    /* First of all, make sure this isn't a long file entry. */
    
    if (fat_entry[entry].read_only == 1 && 
        fat_entry[entry].hidden == 1 && 
        fat_entry[entry].system == 1 && 
        fat_entry[entry].volume_id == 1)
    {
      lfn_entries_in_row++;
      continue;
    }
    
    /* Also, make sure the file hasn't been deleted. */

    if (fat_entry[entry].name[0] == 0xE5)
    {
      continue;
    }

    /* We don't want the volume ID either. */

    if (fat_entry[entry].volume_id == 1)
    {
      continue;
    }

    /* Replace all spaces in the file name with zeroes, so that we
       get a NULL terminated string. */
    
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
    
    string_copy_max (filename, fat_entry[entry].name, 8);
    
    /* Make sure the string is zero terminated. */

    filename[8] = 0;

    /* If we have a file extension, add it too. */

    if (fat_entry[entry].extension[0] != 0)
    {
      unsigned int length = string_length (filename);
      filename[string_length (filename)] = '.';
      string_copy_max (&filename[string_length (filename)], 
                       fat_entry[entry].extension, 3);
      filename[length + string_length (fat_entry[entry].extension)] = '\0';
    }

    /* Get the long filename of the file (if any). */

    read_long_file_name (fat_entry, entry, lfn_entries_in_row, long_file_name);
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
      if (string_compare (long_file_name, path[elements - 1]) == 0)
      {
        done = TRUE;
      }
    }
    else
    {
      if (string_compare (filename, path[elements - 1]) == 0)
      {
        done = TRUE;
      }
    }
  }

  /* The entry we are after is actually one lower than what we've
     got. */

  if (entry > 0)
  {
    entry--;
  }

  /* Did we fall through to the end of the list? */

  if (fat_entry[entry].name[0] == '\0')
  {
    //    log_print (&log_structure, LOG_URGENCY_ERROR, "Jadå, glassen e slut!");
    return FALSE;
  }
  
  /* Get the starting cluster of the file. */

  cluster_number = ((fat_entry[entry].first_cluster_number_high << 16) +
                    fat_entry[entry].first_cluster_number_low);

  /* Add this file to our list of open files. */

  fat_open_file[number_of_open_files].file_handle = open->file_handle;
  fat_open_file[number_of_open_files].start_cluster_number =
  fat_open_file[number_of_open_files].current_cluster_number =
    cluster_number;
  fat_open_file[number_of_open_files].file_position = 0;
  fat_open_file[number_of_open_files].file_size = fat_entry[entry].file_size;
  number_of_open_files++;

  return TRUE;
}

/* Read from a previously opened file. */

static bool fat_file_read (fat_info_type *fat_info, 
                           file_handle_type file_handle,
                           void *read_buffer, u32 bytes)
{
  unsigned int file;
  u32 cluster_number;
  u32 read_bytes = 0;

  /* Find this file in our list of opened files. */

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

  /* If we start in the middle of a cluster, read the part we want
     into our buffer. */

  if (fat_open_file[file].file_position % fat_info->bytes_per_cluster != 0)
  {
    u8 *extra_buffer;
    unsigned int length = min_of_two
      (bytes, (fat_info->bytes_per_cluster -
               (fat_open_file[file].file_position %
                fat_info->bytes_per_cluster)));

    memory_allocate ((void **) &extra_buffer, length);

    read_single_cluster (fat_info, cluster_number, (void *) extra_buffer);
    memory_copy (read_buffer, (u8 *) ((u32) extra_buffer +
                                      fat_open_file[file].file_position %
                                      fat_info->bytes_per_cluster), length);

    memory_deallocate ((void **) &extra_buffer);

    fat_open_file[file].file_position += length;
    read_bytes += length;

    /* Are we at the end of the cluster? */

    if (fat_open_file[file].file_position % fat_info->bytes_per_cluster == 0)
    {
      cluster_number = get_next_cluster (cluster_number, fat_info->fat,
                                         fat_info->bits);
    }
  }

  /* Now, read the rest of the clusters we're after. */
  /* FIXME: Detect when a file ends in the middle of a cluster. */

  while (read_bytes < bytes)
  {
    read_single_cluster (fat_info, cluster_number,
                         (void *) ((u32) read_buffer + read_bytes));
    read_bytes += fat_info->bytes_per_cluster;
    fat_open_file[file].file_position +=
      min_of_two (bytes, fat_info->bytes_per_cluster);
    if (fat_open_file[file].file_position % fat_info->bytes_per_cluster == 0)
    {
      cluster_number = get_next_cluster (cluster_number, fat_info->fat,
                                         fat_info->bits);
    }
  }

  fat_open_file[file].current_cluster_number = cluster_number;

  return TRUE;
}

/* Detects wether there is a FAT file system on the given mailbox ID. */

static bool detect_fat (fat_info_type *fat_info)
{
  message_parameter_type message_parameter;
  u8 *sector = (u8 *) buffer;
  ipc_block_read_type ipc_block_read;

  //  log_print (&log_structure, LOG_URGENCY_DEBUG, "Detecting a FAT volume.");

  /* Read the BPB (BIOS Parameter Block) for this volume. */

  message_parameter.protocol = IPC_PROTOCOL_BLOCK;
  message_parameter.message_class = IPC_BLOCK_READ;
  message_parameter.block = TRUE;
  message_parameter.data = &ipc_block_read;
  message_parameter.length = sizeof (ipc_block_read_type);

  ipc_block_read.start_block_number = 0;
  ipc_block_read.number_of_blocks = 1;
  ipc_send (fat_info->block_structure.output_mailbox_id, &message_parameter);

  message_parameter.length = 1024;
  message_parameter.data = sector;
  ipc_receive (fat_info->block_structure.input_mailbox_id, &message_parameter,
               NULL);

  /* Make sure this is a valid FAT filesystem. */

  if (sector[510] == 0x55 &&
      sector[511] == 0xAA && 
      (bios_parameter_block->media == 0xF0 ||
       (bios_parameter_block->media >= 0xF8 &&
        bios_parameter_block->media <= 0xFF)))
  {
    /* Now, get the size of this FAT (12, 16 or 32 bits). */

    int fat_size, total_sectors, data_sectors, number_of_clusters,
      block_number;

    fat_info->root_directory_sectors = (((bios_parameter_block->number_of_root_entries * 32) +
                                         (bios_parameter_block->bytes_per_sector - 1)) / 
                                        bios_parameter_block->bytes_per_sector);

    if (bios_parameter_block->fat_size_16 != 0)
    {
      fat_size = bios_parameter_block->fat_size_16;
    }
    else
    {
      bios_parameter_block_32 =
        (bios_parameter_block_32_type *) &bios_parameter_block->data;
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

    /* Read the FAT. FIXME: Should be done dynamically, and should
       support FAT32. */

    message_parameter.data = &ipc_block_read;
    message_parameter.length = sizeof (ipc_block_read_type);

    ipc_block_read.start_block_number = 1;
    ipc_block_read.number_of_blocks = bios_parameter_block->fat_size_16;

    ipc_send (fat_info->block_structure.output_mailbox_id, &message_parameter);

    memory_set_u8 ((u8 *) &global_fat, 0, 16384);
    message_parameter.length = 16384;
    message_parameter.data = &global_fat;
    ipc_receive (fat_info->block_structure.input_mailbox_id,
                 &message_parameter, NULL);
    fat_info->fat = &global_fat;

    /* Also, read the root directory. */

    block_number = (bios_parameter_block->reserved_sectors +
                    bios_parameter_block->number_of_fats *
                    bios_parameter_block->fat_size_16);

    /* Read the block. */
    
    message_parameter.protocol = IPC_PROTOCOL_BLOCK;
    message_parameter.message_class = IPC_BLOCK_READ;
    message_parameter.length = sizeof (ipc_block_read_type);
    message_parameter.data = &ipc_block_read;
    message_parameter.block = TRUE;

    //    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
    //                         "Reading %u sectors",
    //                         fat_info->root_directory_sectors);

    ipc_block_read.start_block_number = block_number;
    ipc_block_read.number_of_blocks = fat_info->root_directory_sectors;
    ipc_send (fat_info->block_structure.output_mailbox_id, &message_parameter);
    message_parameter.length = (fat_info->root_directory_sectors *
                                fat_info->bytes_per_sector);
    message_parameter.data = &global_root;
    ipc_receive (fat_info->block_structure.input_mailbox_id,
                 &message_parameter, NULL);
    fat_info->root = &global_root;

    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/* Handle an IPC connection request. */

static void handle_connection (mailbox_id_type reply_mailbox_id)
{
  message_parameter_type message_parameter;
  bool done = FALSE;
  bool mounted = FALSE;
  fat_info_type fat_info;
  ipc_structure_type ipc_structure;
  u8 *data;
  unsigned int data_size = 16384;

  memory_allocate ((void **) &data, data_size);

  /* Accept the connection. */ 

  ipc_structure.output_mailbox_id = reply_mailbox_id;
  if (ipc_connection_establish (&ipc_structure) != IPC_RETURN_SUCCESS)
  {
    return;
  }

  message_parameter.block = TRUE;

  while (!done)
  {
    message_parameter.data = data;
    message_parameter.protocol = IPC_PROTOCOL_FILE;
    message_parameter.message_class = IPC_CLASS_NONE;
    message_parameter.length = data_size;
    
    if (ipc_receive (ipc_structure.input_mailbox_id, &message_parameter, 
                     &data_size) != IPC_RETURN_SUCCESS)
    {
      continue;
    }
    
    switch (message_parameter.message_class)
    {
      /* Read one or more directory entries. */

      case IPC_FILE_DIRECTORY_ENTRY_READ:
      {
        if (mounted)
        {
          file_directory_entry_read_type *directory_entry_read = 
            (file_directory_entry_read_type *) data;
          
          if (!fat_directory_entry_read (directory_entry_read,
                                         &fat_info))
          {
            directory_entry_read->entries = 0;
          }
          message_parameter.length =
            sizeof (file_directory_entry_read_type) +
            sizeof (file_directory_entry_type) *
            directory_entry_read->entries ;
          //          log_print_formatted (0, PACKAGE, "Successfully read %u entries",
          //                               directory_entry_read->entries);
          //          log_print_formatted (0, PACKAGE, "max: %u\n", directory_entry_read->entries);
          ipc_send (ipc_structure.output_mailbox_id, &message_parameter);
        }
        break;
      }

      case IPC_FILE_GET_INFO:
      {
        if (mounted)
        {
          file_verbose_directory_entry_type *directory_entry = 
            (file_verbose_directory_entry_type *) data;
          
          if (!fat_file_get_info (&fat_info, directory_entry))
          {
            return_type return_value = FILE_RETURN_FILE_ABSENT;

            log_print_formatted (&log_structure, LOG_URGENCY_ERROR,
                                 "IPC_FILE_GET_INFO failed");
            message_parameter.message_class = IPC_FILE_RETURN_VALUE;
            message_parameter.data = &return_value;
            message_parameter.length = sizeof (return_type);
          }

          ipc_send (ipc_structure.output_mailbox_id, &message_parameter);
        }
        break;
      }

      case IPC_FILE_MOUNT_VOLUME:
      {
        mailbox_id_type *mailbox = (mailbox_id_type *) data;

        fat_info.block_structure.output_mailbox_id = *mailbox;
        if (ipc_service_connection_request (&fat_info.block_structure) !=
            IPC_RETURN_SUCCESS)
        {
          break;
        }

        /* Check if we have a FAT file system at this location. */
        
        if (!detect_fat (&fat_info))
        {
          log_print (&log_structure, LOG_URGENCY_ERROR,
                     "No FAT filesystem detected.");
          break;
        }

        mounted = TRUE;

        break;
      }

      case IPC_FILE_OPEN:
      {
        ipc_file_open_type *open = (ipc_file_open_type *) data;

        if (!fat_file_open (&fat_info, open))
        {
          log_print (&log_structure, LOG_URGENCY_ERROR,
                     "Failed to open file.");
        }

        ipc_send (ipc_structure.output_mailbox_id, &message_parameter);
                                  
        break;
      }

      case IPC_FILE_READ:
      {
        file_read_type *read = (file_read_type *) data;
        u8 *read_buffer;

        memory_allocate ((void **) &read_buffer, read->bytes);

        if (!fat_file_read (&fat_info, read->file_handle, read_buffer, 
                            read->bytes))
        {
          log_print (&log_structure, LOG_URGENCY_ERROR,
                     "Failed to read from file.");
        }
        
        message_parameter.data = read_buffer;
        message_parameter.length = read->bytes;
#ifdef DEBUG
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                             "Sending %lu", read->bytes);
#endif
        ipc_send (ipc_structure.output_mailbox_id, &message_parameter);
        memory_deallocate ((void **) &read_buffer);
        break;
      }

      /* Unsupported functions. */

      case IPC_FILE_CLOSE:
      case IPC_FILE_SEEK:
      case IPC_FILE_WRITE:
      case IPC_FILE_ACL_READ:
      case IPC_FILE_ACL_WRITE:
      case IPC_FILE_UNMOUNT_VOLUME:
      default:
      {
        return_type return_value = IPC_RETURN_FILE_FUNCTION_UNSUPPORTED;

        message_parameter.data = &return_value;
        message_parameter.length = sizeof (return_type);

        ipc_send (ipc_structure.output_mailbox_id, &message_parameter);
        break;
      }
    }
  }
}

/* Main function. */

int main (void)
{
  ipc_structure_type ipc_structure;

  /* Set our name. */

  system_process_name_set (PACKAGE_NAME);
  system_thread_name_set ("Initializing");

  if (log_init (&log_structure, PACKAGE_NAME, 
                &empty_tag) != LOG_RETURN_SUCCESS)
  {
    return -1;
  }

  /* Create our service. */

  if (ipc_service_create ("file_system", &ipc_structure, 
                          &empty_tag) != STORM_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Couldn't create a file system service.");
    return -1;
  }

  system_call_process_parent_unblock ();

  /* Main loop. */

  system_thread_name_set ("Service handler");
  while (TRUE)
  {
    mailbox_id_type reply_mailbox_id;

    ipc_service_connection_wait (&ipc_structure);
    reply_mailbox_id = ipc_structure.output_mailbox_id;

    if (system_thread_create () == SYSTEM_RETURN_THREAD_NEW)
    {
      system_thread_name_set ("Handling connection");
      handle_connection (reply_mailbox_id);
      return 0;
    }
  }    
}

/* Read the Long File Name for a FAT directory entry. */

static void read_long_file_name (fat_entry_type *fat_entry,
                                 int short_file_name_entry,
                                 int lfn_entries,
                                 char *long_file_name)
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
      /* There wasn't any long file name for this entry. This is
         perfectly valid; it happens when you have files like COPYING
         and README, in all-uppercase. */

      break;
    }
    else if (fat_entry[entry].first_cluster_number_low != 0)
    {
      log_print (&log_structure, LOG_URGENCY_ERROR, "Corrupt LFN entry.");
      break;
    }
    else
    {
      lfn = (long_file_name_entry_type *) &(fat_entry[entry]);

      /* FIXME: Convert those Unicode encoded characters correctly. */
      /* COMMENT: It isn't even real unicode, only 16 bit. Isn't it?. Crap! */

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
