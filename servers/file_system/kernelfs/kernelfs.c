/* $Id$ */
/* Abstract: Kernel file system server. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

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

#include "kernelfs.h"

static log_structure_type log_structure;

/* An empty tag list. */

static tag_type empty_tag =
{
  0, 0, ""
};

static kernelfs_entry_type kernelfs[] __attribute__ ((unused)) =
{
  { FILE_ENTRY_TYPE_DIRECTORY,  "thread",  KERNELFS_CLASS_THREAD_INFO },
  { FILE_ENTRY_TYPE_DIRECTORY,  "process", KERNELFS_CLASS_PROCESS_INFO },
  { FILE_ENTRY_TYPE_FILE,       "uptime",  KERNELFS_CLASS_UPTIME_INFO },
  { FILE_ENTRY_TYPE_FILE,       "memory",  KERNELFS_CLASS_MEMORY_INFO },
  { FILE_ENTRY_TYPE_DIRECTORY,  "cpu",     KERNELFS_CLASS_CPU_INFO },
  { FILE_ENTRY_TYPE_FILE,       "storm",   KERNELFS_CLASS_STORM_INFO },
  { FILE_ENTRY_TYPE_FILE,       "video",   KERNELFS_CLASS_VIDEO_INFO },
  { FILE_ENTRY_TYPE_FILE,       "self",    KERNELFS_CLASS_SELF_INFO },
  { FILE_ENTRY_TYPE_FILE,       "port",    KERNELFS_CLASS_PORT_INFO },
  { FILE_ENTRY_TYPE_FILE,       "irq",     KERNELFS_CLASS_IRQ_INFO },
};

#define KERNELFS_ROOT_ENTRIES   (sizeof (kernelfs) / \
                                 sizeof (kernelfs_entry_type))

/* Handle the connection to the VFS service. */

static void handle_connection (ipc_structure_type *ipc_structure)
{
  message_parameter_type message_parameter;
  bool done = FALSE;
  u8 *data;
  unsigned int data_size = 16384;
  file_mount_type mount = { "kernelfs" };

  memory_allocate ((void **) &data, data_size);

  /* Mount ourselves. */

  message_parameter.block = TRUE;
  message_parameter.data = &mount;
  message_parameter.protocol = IPC_PROTOCOL_FILE;
  message_parameter.message_class = IPC_FILE_MOUNT_VOLUME;
  message_parameter.length = sizeof (file_mount_type);
  ipc_send (ipc_structure->output_mailbox_id, &message_parameter);

  while (!done)
  {
    message_parameter.data = data;
    message_parameter.protocol = IPC_PROTOCOL_FILE;
    message_parameter.message_class = IPC_CLASS_NONE;
    message_parameter.length = data_size;
    
    if (ipc_receive (ipc_structure->input_mailbox_id, &message_parameter, 
                     &data_size) != IPC_RETURN_SUCCESS)
    {
      continue;
    }
    
    switch (message_parameter.message_class)
    {
      /* Read one or more directory entries. */

      case IPC_FILE_DIRECTORY_ENTRY_READ:
      {
        file_directory_entry_read_type *directory_entry_read = 
          (file_directory_entry_read_type *) data;
        unsigned int input_index;
        unsigned int output_index = 0;
        unsigned int max_entries = directory_entry_read->entries;
        
        directory_entry_read->entries = 0;
        
        for (input_index = directory_entry_read->start_entry;
             input_index < (directory_entry_read->start_entry + max_entries) &&
               input_index < KERNELFS_ROOT_ENTRIES; input_index++)
        {
          string_copy 
            (directory_entry_read->entry[output_index].name,
             kernelfs[input_index].name);
          directory_entry_read->entry[output_index].type = 
            kernelfs[input_index].type;
          directory_entry_read->entries++;
          output_index++;
        }
        
        if (input_index == KERNELFS_ROOT_ENTRIES)
        {
          directory_entry_read->end_reached = TRUE;
        }
        else
        {
          directory_entry_read->end_reached = FALSE;
        }

        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG, "ull: %u",
                             directory_entry_read->entries);

        message_parameter.length = (sizeof (file_directory_entry_read_type) +
                                    sizeof (file_directory_entry_type) *
                                    directory_entry_read->entries);
        ipc_send (ipc_structure->output_mailbox_id, &message_parameter);
        break;
      }

      /* Get information about a file directory entry. */

      case IPC_FILE_GET_INFO:
      {
        //        file_verbose_directory_entry_type *verbose_directory_entry =
        //          (file_verbose_directory_entry_type *) data;

        message_parameter.length = sizeof (file_verbose_directory_entry_type);
        ipc_send (ipc_structure->output_mailbox_id, &message_parameter);
        
        break;
      }

      /* Unsupported functions. */

      case IPC_FILE_OPEN:
        
        /* FIXME: For READ, we first create a buffer, fills it with
           the content of the 'file' and then pass the requested part
           of the file to the caller. */

      case IPC_FILE_READ:
      case IPC_FILE_CLOSE:
      case IPC_FILE_SEEK:
      case IPC_FILE_WRITE:
      case IPC_FILE_ACL_READ:
      case IPC_FILE_ACL_WRITE:
      case IPC_FILE_MOUNT_VOLUME:
      case IPC_FILE_UNMOUNT_VOLUME:
      default:
      {
        return_type return_value = IPC_RETURN_FILE_FUNCTION_UNSUPPORTED;

        message_parameter.data = &return_value;
        message_parameter.length = sizeof (return_type);

        ipc_send (ipc_structure->output_mailbox_id, &message_parameter);
        break;
      }
    }
  }
}

/* Main function. */

int main (void)
{
  ipc_structure_type ipc_structure;
  mailbox_id_type mailbox_id[10];
  unsigned int services = 10;

  system_process_name_set (PACKAGE_NAME);
  system_thread_name_set ("Initialising");
  
  if (log_init (&log_structure, PACKAGE_NAME, &empty_tag) !=
      LOG_RETURN_SUCCESS)
  {
    return -1;
  }
    
  system_call_process_parent_unblock ();

  if (ipc_service_resolve ("virtual_file_system", mailbox_id, &services, 0, 
                           &empty_tag) != IPC_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY, 
               "Couldn't resolve VFS service.");
    return -1;
  }

  ipc_structure.output_mailbox_id = mailbox_id[0];
  ipc_service_connection_request (&ipc_structure);
  handle_connection (&ipc_structure);
  
  return 0;
}
