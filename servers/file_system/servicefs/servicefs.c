/* $Id$ */
/* Abstract: Service file system server. */
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

#include "servicefs.h"

static log_structure_type log_structure;

/* An empty tag list. */

static tag_type empty_tag =
{
  0, 0, ""
};

/* Split a path name to its components. */

static void path_split (char *path_name, char **output, unsigned int *elements)
{
  unsigned int index = 0;
  unsigned int output_index = 0;

  if (path_name[0] == '/')
  {
    /* Are we at the meta root? */

    if (path_name[1] == '/')
    {
      output[0] = &path_name[2];
      output_index = 1;
      index = 2;
    }
    else
    {
      output[0] = "/";
      output[1] = &path_name[1];
      index = 2;
      output_index += 2;
    }
  }

  while (path_name[index] != '\0' && output_index < *elements)
  {
    if (path_name[index] == PATH_NAME_SEPARATOR &&
        path_name[index + 1] != '\0')
    {
      path_name[index] = '\0';
      output[output_index] = &path_name[index + 1];
      output_index++;
    }
    index++;
  }

  *elements = output_index;
}

/* Handle an IPC connection request. */

static void handle_connection (ipc_structure_type *ipc_structure)
{
  message_parameter_type message_parameter;
  bool done = FALSE;
  u8 *data;
  unsigned int data_size = 16384;
  file_mount_type mount = { "servicefs" };

  memory_allocate ((void **) &data, data_size);

  /* Mount ourselves. */

  message_parameter.block = TRUE;
  message_parameter.data = &mount;
  message_parameter.protocol = IPC_PROTOCOL_FILE;
  message_parameter.message_class = IPC_FILE_MOUNT_VOLUME;
  message_parameter.length = sizeof (file_mount_type);
  ipc_send (ipc_structure->output_mailbox_id, &message_parameter);

  message_parameter.block = TRUE;

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
        service_protocol_type *protocol_info;
        unsigned int protocols = 50;
        unsigned int output_index = 0;
        unsigned int max_entries = directory_entry_read->entries;
        unsigned int input_index;
        char *path[10];
        unsigned int elements = 10;

        path_split (directory_entry_read->path_name, path, 
                    &elements);
        
        log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                             "%u", elements);
        
        /* TODO: Do something more with the path array here. */

        directory_entry_read->entries = 0;

        memory_allocate ((void **) &protocol_info, 
                         protocols * sizeof (service_protocol_type));
        system_call_service_protocol_get (&protocols, protocol_info);

        for (input_index = directory_entry_read->start_entry;
             input_index < (directory_entry_read->start_entry + max_entries) &&
               input_index < protocols; input_index++)
        {
          string_copy 
            (directory_entry_read->entry[output_index].name,
             protocol_info[input_index].name);
          directory_entry_read->entry[output_index].type = 
            FILE_ENTRY_TYPE_DIRECTORY;
          directory_entry_read->entries++;
          output_index++;
        }
        
        if (input_index == protocols)
        {
          directory_entry_read->end_reached = TRUE;
        }
        else
        {
          directory_entry_read->end_reached = FALSE;
        }
        
        message_parameter.length = (sizeof (file_directory_entry_read_type) +
                                    sizeof (file_directory_entry_type) *
                                    directory_entry_read->entries);
        ipc_send (ipc_structure->output_mailbox_id, &message_parameter);
        break;
      }

      /* Get verbose information about the given file entry. */

      case IPC_FILE_GET_INFO:
      {
        file_verbose_directory_entry_type *verbose_directory_entry =
          (file_verbose_directory_entry_type *) data;
        unsigned int protocols = 50;
        unsigned int index;
        service_protocol_type *protocol_info;

        memory_allocate ((void **) &protocol_info, 
                         protocols * sizeof (service_protocol_type));
        system_call_service_protocol_get (&protocols, protocol_info);

        verbose_directory_entry->success = FALSE;
            
        for (index = 0; index < protocols; index++)
        {
          if (string_length (verbose_directory_entry->path_name) > 1 &&
              string_compare (protocol_info[index].name, 
                              verbose_directory_entry->path_name + 1) == 0)
          {
            verbose_directory_entry->success = TRUE;
            verbose_directory_entry->type = FILE_ENTRY_TYPE_DIRECTORY;
            verbose_directory_entry->time = time_get ();
            verbose_directory_entry->size = 
              protocol_info[index].number_of_services;
            break;
          }
        }

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
    
  /* Create our service. */

  if (ipc_service_create ("file_system", &ipc_structure, &empty_tag) != 
      STORM_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Couldn't create a file system service.");
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
