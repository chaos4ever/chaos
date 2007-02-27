/* $Id$ */
/* Abstract: Boot server. */
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

#include "config.h"

/* The maximum number of servers to load. */

#define MAX_SERVERS 16

#define STARTUP_FILE "//ramdisk/config/servers/boot/startup"

static log_structure_type log_structure;

/* An empty tag list. */

tag_type empty_tag =
{
  0, 0, ""
};

/* Main function. */

int main (void)
{
#if FALSE
  virtual_file_system_mount_type mount;
  mailbox_id_type mailbox_id[10];
  ipc_structure_type vfs_structure;
  message_parameter_type message_parameter;
  file_read_type read;
  file_handle_type handle;
  file_verbose_directory_entry_type directory_entry;
  u8 *buffer;
  u8 *server_name_buffer;
  char *server[MAX_SERVERS];
  unsigned int where, number_of_servers = 0, server_number;
  process_id_type process_id;
  unsigned int bytes_read;
  unsigned int services = 10;
#endif

  /* Set our name. */

  system_process_name_set (PACKAGE_NAME);
  system_thread_name_set ("Initialising");

  if (log_init (&log_structure, PACKAGE_NAME, &empty_tag) !=
      LOG_RETURN_SUCCESS)
  {
    return -1;
  }

#if FALSE

  /* Mount the initial ramdisk as //ramdisk. To do this, we must first
     hook up a connection to the VFS service and resolve the first
     block service. */
    
  if (ipc_service_resolve ("virtual_file_system", mailbox_id, &services, 5, 
                           &empty_tag) != IPC_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Couldn't resolve the VFS service.");
    return -1;
  }

  vfs_structure.output_mailbox_id = mailbox_id[0];

  if (ipc_service_connection_request (&vfs_structure) != IPC_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Couldn't connect to the VFS service.");
    return -1;
  }

  services = 1;

  if (ipc_service_resolve ("block", mailbox_id, &services, 5, &empty_tag) !=
      IPC_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY, 
               "No block services found.");
    return -1;
  }

  mount.mailbox_id = mailbox_id[0];
  string_copy (mount.location, "ramdisk");

  /* That's it. Send the message. */

  message_parameter.protocol = IPC_PROTOCOL_VIRTUAL_FILE_SYSTEM;
  message_parameter.message_class = IPC_VIRTUAL_FILE_SYSTEM_MOUNT;
  message_parameter.data = &mount;
  message_parameter.length = sizeof (virtual_file_system_mount_type);
  message_parameter.block = TRUE;
  ipc_send (vfs_structure.output_mailbox_id, &message_parameter);

  log_print (&log_structure, LOG_URGENCY_DEBUG,
             "Mounted the first available block service as //ramdisk.");

  /* Now, read the list of servers to start from here. */

  log_print (&log_structure, LOG_URGENCY_DEBUG, "Reading startup script...");
  string_copy (directory_entry.path_name, STARTUP_FILE);
  if (file_get_info (&vfs_structure, &directory_entry) != FILE_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_ERROR,
               STARTUP_FILE " not found.");
    return -1;
  }

  memory_allocate ((void **) &server_name_buffer, directory_entry.size);

  file_open (&vfs_structure, STARTUP_FILE, FILE_MODE_READ, &handle);
  file_read (&vfs_structure, handle, directory_entry.size,
             &server_name_buffer);

  /* Parse the file. */

  server[0] = &server_name_buffer[0];
  number_of_servers++;

  for (where = 1; where < directory_entry.size; where++)
  {

    if (server_name_buffer[where] == '\n')
    {
      server_name_buffer[where] = '\0';
      if (where + 1 < directory_entry.size)
      {
        server[number_of_servers] = &server_name_buffer[where + 1];
        number_of_servers++;
      }
    }
  }

  log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                       "Starting %u servers.", number_of_servers);

  for (server_number = 0; server_number < number_of_servers; server_number++)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE, 
                         "Starting %s.", server[server_number]);

    string_copy (directory_entry.path_name, server[server_number]);
    if (file_get_info (&vfs_structure, &directory_entry) !=
        FILE_RETURN_SUCCESS)
    {
      log_print_formatted (&log_structure, LOG_URGENCY_ERROR,
                           "'%s' could not be accessed!",
                           server[server_number]);
      continue;
    }

    /* Open the file. */

    file_open (&vfs_structure, server[server_number], FILE_MODE_READ, &handle);
    read.file_handle = handle;
    
    bytes_read = 0;

    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "Allocating %lu bytes for %s.",
                         directory_entry.size, server[server_number]);

    memory_allocate ((void **) &buffer, directory_entry.size);

    log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                         "Buffer is at %p.", buffer);
    
    while (bytes_read < directory_entry.size)
    {
      unsigned int bytes;

      /* Read the file. */
    
      bytes = directory_entry.size - bytes_read;
      if (bytes > 32 * KB)
      {
        bytes = 32 * KB;
      }
      file_read (&vfs_structure, handle, bytes, &buffer[bytes_read]);
      bytes_read += bytes;
    }
    
    switch (execute_elf ((elf_header_type *) buffer, "", &process_id))
    {
      case EXECUTE_ELF_RETURN_SUCCESS:
      {
        log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                             "New process ID %lu.",
                              process_id);
        break;
      }
      
      case EXECUTE_ELF_RETURN_IMAGE_INVALID:
      {
        log_print (&log_structure, LOG_URGENCY_ERROR,
                   "Invalid ELF image.");
        break;
      }
      
      case EXECUTE_ELF_RETURN_ELF_UNSUPPORTED:
      {
        log_print (&log_structure, LOG_URGENCY_ERROR,
                   "Unsupported ELF.");
        break;
      }
      
      case EXECUTE_ELF_RETURN_FAILED:
      {
        log_print (&log_structure, LOG_URGENCY_ERROR,
                   "system_process_create failed.");
        break;
      }
    }

    memory_deallocate ((void **) &buffer);
  }

#endif

  system_call_process_parent_unblock ();

  log_print (&log_structure, LOG_URGENCY_DEBUG, "surf");

  return 0;
}
