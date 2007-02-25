/* $Id$ */
/* Abstract: Cludio internal commands. */
/* Authors: Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

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
#include "cluido.h"

#define VFS_BUFFER_SIZE         4096

const char *file_type[] =
{
  /* Directory. */

  "[DIR]",

  /* Ordinary file. */

  "[FILE]",

  /* Soft link. */
  
  "[SLNK]",

  /* Hard link. */

  "[HLNK]"
};

void command_arp (int number_of_arguments, char **argument);
void command_benchmark (int number_of_arguments, char **argument);
void command_clear (int number_of_arguments, char **argument);
void command_cpu (int number_of_arguments, char **argument);
void command_crash (int number_of_arguments, char **argument);
void command_directory_change_working (int number_of_arguments,
                                       char **argument);
void command_directory_list (int number_of_arguments, char **argument);
void command_execute (int number_of_arguments, char **argument);
void command_font_set (int number_of_arguments, char **argument);
void command_help (int number_of_arguments, char **argument);
void command_ip (int number_of_arguments, char **argument);
void command_irq (int number_of_arguments, char **argument);
void command_kill (int number_of_arguments, char **argument);
void command_memory (int number_of_arguments, char **argument);
void command_pci (int number_of_arguments, char **argument);
void command_ports (int number_of_arguments, char **argument);
void command_processes (int number_of_arguments, char **argument);
void command_reboot (int number_of_arguments, char **argument);
void command_run (int number_of_arguments, char **argument);
void command_set (int number_of_arguments, char **argument);
void command_show (int number_of_arguments, char **argument);
void command_show_file (int number_of_arguments, char **argument);
void command_shutdown (int number_of_arguments, char **argument) __attribute__ ((noreturn));
void command_sleep (int number_of_arguments, char **argument);
void command_test (int number_of_arguments, char **argument);
void command_threads (int number_of_arguments, char **argument);
void command_time (int number_of_arguments, char **argument);
void command_top (int number_of_arguments, char **argument);
void command_unset (int number_of_arguments, char **argument);
void command_uptime (int number_of_arguments, char **argument);
void command_version (int number_of_arguments, char **argument);

/* Structure for holding a list of all the commands, and which
   functions they correspond to. */

command_type command[] =
{
  { "?", "", "Display help about available commands.", command_help },
  { "arp", "", "Show the entries in the ARP table.", command_arp },
  { "benchmark", "", "Do some basic IPC benchmarking.", command_benchmark },
  { "clear", "", "Clear the console.", command_clear },
  { "cpu", "", "Display information about installed CPUs.", command_cpu },
  { "crash", "", "Cause this process to crash.", command_crash },
  { "cd", "DIRECTORY",
    "Change the current working directory.",
    command_directory_change_working },
  { "list", "", "List the contents of the current directory.",
    command_directory_list },
  { "execute", "FILE", "Executes the given file.", command_execute },
  { "font_set", "FILE", "Set the font to the one in FILE", command_font_set },
  { "help", "[COMMAND]", "Display help about available commands.",
    command_help },
  { "ip", "(--dhcp interface | --set interface address netmask [gateway] | --forward { on | off })", "Configure IPv4 networking.", command_ip },
  { "irq", "", "Display information about the IRQ levels in use.",
    command_irq },
  { "kill", "THREAD", "Kills the given thread.", command_kill },
  { "memory", "", "Display memory amount and usage.", command_memory },
  { "pci", "", "Display information about any PCI devices in the system.",
    command_pci },
  { "ports", "", "Display information about the I/O ports in use.",
    command_ports },
  { "processes", "", "Display information about the running processes.",
    command_processes },
  { "reboot", "", "Reboot the system.", command_reboot },
  { "run", "SCRIPT", "Run a cluido script (just a list of commands).", command_run },
  { "shutdown", "", "Shutdown system by telling the boot server to shutdown the system.",
    command_shutdown },
  { "set", "VARIABLE VALUE", "Set VARIABLE to VALUE in the environment",
    command_set },
  { "show", "", "Show the contents of the environment.", command_show },
  { "show_file", "FILE", "Show the contents of the given file.", 
    command_show_file },
  { "sleep", "TIME", "Sleep for TIME seconds.", command_sleep },
  { "test", "", "Basic test command.", command_test },
  { "threads", "", "Display information about the all threads in the system.",
    command_threads },
  { "time", "", "Show the time.", command_time },
  { "top", "",
    "Display information about the running threads sorted on CPU usage.",
    command_top },
  { "unset", "VARIABLE", "Unset VARIABLE in the environment.", command_unset },
  { "uptime", "", "Display current machine uptime.", command_uptime },
  { "version", "", "Show the program version", command_version }
};

int number_of_commands = (sizeof (command) / sizeof (command_type));

/* Show the entries in the ARP table. */

void command_arp (int number_of_arguments __attribute__ ((unused)),
                  char **argument __attribute__ ((unused)))
{
  message_parameter_type message_parameter;
  int amount;
  int counter;
  ipv4_arp_entry_type arp;

  message_parameter.protocol = IPC_PROTOCOL_IPV4;
  message_parameter.message_class = IPC_IPV4_ARP_GET_AMOUNT;
  message_parameter.data = &amount;
  message_parameter.length = 0;
  message_parameter.block = TRUE;
  ipc_send (ipv4_structure.output_mailbox_id, &message_parameter);

  message_parameter.length = sizeof (int);
  ipc_receive (ipv4_structure.input_mailbox_id, &message_parameter, NULL);

  message_parameter.message_class = IPC_IPV4_ARP_GET_NUMBER;
  for (counter = 0; counter < amount; counter++)
  {
    message_parameter.data = &counter;
    message_parameter.length = sizeof (int);
    ipc_send (ipv4_structure.output_mailbox_id, &message_parameter);

    message_parameter.data = &arp;
    message_parameter.length = sizeof (ipv4_arp_entry_type);
    ipc_receive (ipv4_structure.input_mailbox_id, &message_parameter, NULL);
    
    console_print_formatted (&console_structure, 
                             "%u.%u.%u.%u %02X:%02X:%02X:%02X:%02X:%02X\n",
                             (arp.ip_address >> 0) & 0xFF, 
                             (arp.ip_address >> 8) & 0xFF,
                             (arp.ip_address >> 16) & 0xFF,
                             (arp.ip_address >> 24) & 0xFF,
                             arp.ethernet_address[0],
                             arp.ethernet_address[1],
                             arp.ethernet_address[2],
                             arp.ethernet_address[3],
                             arp.ethernet_address[4],
                             arp.ethernet_address[5]);
  }
}

/* Benchmark the system. This includes IPC and console. */
/* FIXME: Add file benchmarking as soon as file creation/deletion is done. */

void command_benchmark (int number_of_arguments __attribute__ ((unused)),
                        char **argument __attribute__ ((unused)))
{
  int counter;
  time_type start_time, end_time, phony;
  
  system_call_timer_read (&start_time);
    
  for (counter = 0; counter < 10000; counter++)
  {
    console_print (&console_structure, 
                   "\rBenchmarking IPC and console server...");
  }

  system_call_timer_read (&end_time);
  console_print_formatted (&console_structure, 
                           "\n%llu milliseconds (10 000 prints).\n",
                           end_time - start_time);

  console_print (&console_structure, "Benchmarking system calls...\n");
  system_call_timer_read (&start_time);
  for (counter = 0; counter < 1000000; counter++)
  {
    system_call_timer_read (&phony);
  }
  system_call_timer_read (&end_time);
  console_print_formatted (&console_structure,
                           "%llu milliseconds (1 000 000 system calls).\n",
                           end_time - start_time);
}

/* Clear the screen. */

void command_clear (int number_of_arguments __attribute__ ((unused)),
                    char **argument __attribute__ ((unused)))
{
  console_clear (&console_structure);
}

/* Get information about installed CPU:s. */

void command_cpu (int number_of_arguments __attribute__ ((unused)),
                  char **argument __attribute__ ((unused)))
{
  kernelfs_cpu_info_type cpu_info;

  cpu_info.kernelfs_class = KERNELFS_CLASS_CPU_INFO;
  system_call_kernelfs_entry_read (&cpu_info);
  console_print_formatted (&console_structure, 
                           "CPU 0: Vendor: %s, Model: %s, Speed: %lu Hz.\n",
                           cpu_info.vendor, cpu_info.name, cpu_info.hz);
}

/* Cause an illegal pagefault. */

void command_crash (int number_of_arguments __attribute__ ((unused)),
                    char **argument __attribute__ ((unused)))
{
  *(u32 *) NULL = 0x42424242;
}

/* Change current working directory. */

void command_directory_change_working (int number_of_arguments,
                                       char *argument[])
{
  file_verbose_directory_entry_type directory_entry;

  /* FIXME: Ugly. */

  char new_working_directory[256];

  string_copy (new_working_directory, working_directory);

  if (number_of_arguments != 2)
  {
    return;
  }

  if (string_compare (argument[1], ".") == 0)
  {
    return;
  }

  if (string_compare (argument[1], "..") == 0)
  {
    /* Go up a level. */

    int index = string_length (new_working_directory) - 1;
    
    while (new_working_directory[index] != '\0' &&
           new_working_directory[index] != '/')
    {
      index--;
    }

    if (index > 0 && new_working_directory[index] == '/' &&
        new_working_directory[index - 1] == '/')
    {
      new_working_directory[index + 1] = '\0';
    }
    else
    {
      new_working_directory[index] = '\0';
    }
  }
  else if (argument[1][0] == '/')
  {
    /* Absolute path. */

    string_copy (new_working_directory, argument[1]);
  }
  else
  {
    /* Relative path. */

    if (string_compare (working_directory, "//") != 0)
    {
      string_append (new_working_directory, "/");
    }
    string_append (new_working_directory, argument[1]);
  }

  string_copy_max (directory_entry.path_name, new_working_directory,
                   MAX_PATH_NAME_LENGTH);
  file_get_info (&vfs_structure, &directory_entry);

  if (!directory_entry.success || 
      directory_entry.type != FILE_ENTRY_TYPE_DIRECTORY)
  {
    console_print_formatted (&console_structure, "%s is not a directory.\n",
                             new_working_directory);
  }
  else
  {
    string_copy (working_directory, new_working_directory);
  }
}

/* List files. */

void command_directory_list (int number_of_arguments __attribute__ ((unused)),
                             char *argument[] __attribute__ ((unused)))
{
  u8 *buffer;
  file_directory_entry_read_type *directory_entry;
  unsigned int index;

  memory_allocate ((void **) &buffer, VFS_BUFFER_SIZE);
  directory_entry = (file_directory_entry_read_type *) buffer;

  directory_entry->start_entry = 0;
  directory_entry->entries = ((VFS_BUFFER_SIZE - 
                               sizeof (file_directory_entry_read_type)) /
                              sizeof (file_directory_entry_type));
  directory_entry->end_reached = FALSE;

  while (directory_entry->end_reached != TRUE)
  {
    string_copy (directory_entry->path_name, working_directory);

    if (file_directory_entry_read (&vfs_structure, directory_entry) !=
        FILE_RETURN_SUCCESS)
    {
      break;
    }
    
    for (index = 0; index < directory_entry->entries; index++)
    {
      file_verbose_directory_entry_type verbose_directory_entry;

      string_copy (verbose_directory_entry.path_name, working_directory);
      if (string_compare (working_directory, "//") != 0)
      {
        string_append (verbose_directory_entry.path_name, "/");
      }
      string_append (verbose_directory_entry.path_name,
                     directory_entry->entry[index].name);

      file_get_info (&vfs_structure, &verbose_directory_entry);

      if (directory_entry->entry[index].type == FILE_ENTRY_TYPE_FILE)
      {
        console_print_formatted (&console_structure, "%-10u %s\n",
                                 verbose_directory_entry.size,
                                 directory_entry->entry[index].name);
      }
      else
      {
        console_print_formatted (&console_structure, "%-10s %s\n",
                                 file_type[directory_entry->entry[index].type],
                                 directory_entry->entry[index].name);
      }
    }
    directory_entry->start_entry += directory_entry->entries;
  }

  memory_deallocate ((void **) &buffer);
}

/* Executes the given file. */

void command_execute (int number_of_arguments, char **argument)
{
  u8 *buffer;
  file_handle_type handle;
  file_verbose_directory_entry_type directory_entry;
  process_id_type process_id;
  unsigned int bytes_read = 0;

  if (number_of_arguments != 2)
  {
    console_print_formatted (&console_structure,
                             "Wrong number of arguments! Try 'help %s'.\n",
                             argument[0]);
    return;
  }

  string_copy (directory_entry.path_name, argument[1]);
  if (file_get_info (&vfs_structure, &directory_entry) != FILE_RETURN_SUCCESS)
  {
    console_print_formatted (&console_structure, 
                             "Could not get information about file %s.\n",
                             argument[1]);
    return;
  }

  /* Allocate a buffer, so we can read the entire file. */

  memory_allocate ((void **) &buffer, directory_entry.size);

  file_open (&vfs_structure, argument[1], FILE_MODE_READ, &handle);

  /* Read the file. */

  while (bytes_read < directory_entry.size)
  {
    unsigned int bytes;
    
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
      console_print_formatted (&console_structure, 
                               "New process ID %lu.\n", process_id);
      break;
    }

    case EXECUTE_ELF_RETURN_IMAGE_INVALID:
    {
      console_print (&console_structure, "Invalid ELF image.\n");
      break;
    }

    case EXECUTE_ELF_RETURN_ELF_UNSUPPORTED:
    {
      console_print (&console_structure, "Unsupported ELF.\n");
      break;
    }

    case EXECUTE_ELF_RETURN_FAILED:
    {
      console_print (&console_structure, 
                     "system_call_process_create failed.\n");
      break;
    }
  }

  memory_deallocate ((void **) &buffer);
}

/* Change the VGA font accordingly. */

void command_font_set (int number_of_arguments, char **argument)
{
  u8 *buffer;
  message_parameter_type message_parameter;
  mailbox_id_type mailbox_id[10];
  ipc_structure_type ipc_structure;
  file_verbose_directory_entry_type directory_entry;
  file_handle_type handle;
  unsigned int services = 10;

  /* FIXME: Support fonts of different sizes. */

  unsigned int font_size = 2048;

  if (number_of_arguments != 2)
  {
    console_print_formatted (&console_structure,
                             "Wrong number of arguments! Try 'help %s'.\n",
                             argument[0]);
    return;
  }

  memory_allocate ((void **) &buffer, font_size);
  
  string_copy (directory_entry.path_name, argument[1]);
  if (file_get_info (&vfs_structure, &directory_entry) != FILE_RETURN_SUCCESS)
  {
    console_print_formatted (&console_structure, 
                             "Could not get information about file %s.\n",
                             argument[1]);
    memory_deallocate ((void **) &buffer);
    return;
  }

  file_open (&vfs_structure, argument[1], FILE_MODE_READ, &handle);

  /* Read the file.  */

  file_read (&vfs_structure, handle, font_size, buffer);

  /* Set the font. */
  /* FIXME: Have a library function for this, and make it go through
     the console server. */

  ipc_service_resolve ("video", mailbox_id, &services, 5, &empty_tag);

  ipc_structure.output_mailbox_id = mailbox_id[0];
  ipc_service_connection_request (&ipc_structure);

  message_parameter.protocol = IPC_PROTOCOL_VIDEO;
  message_parameter.message_class = IPC_VIDEO_FONT_SET;
  message_parameter.data = buffer;
  message_parameter.length = font_size;

  ipc_send (ipc_structure.output_mailbox_id, &message_parameter);

  memory_deallocate ((void **) &buffer);
}

/* Show a list of the available commands, with a short description */

void command_help (int number_of_arguments, char **argument)
{
  int counter;

  if (number_of_arguments == 2)
  {
    for (counter = 0; counter < number_of_commands; counter++)
    {
      if (string_compare (command[counter].name, argument[1]) == 0)
      {
        console_print_formatted (&console_structure, 
                                 "SYNOPSIS: %s %s\n\n  DESCRIPTION: %s\n",
                                 command[counter].name,
                                 command[counter].arguments, 
                                 command[counter].description);
        break;
      }
    }
    
    if (counter == number_of_commands)
    {
      console_print (&console_structure, "Unknown command. Try 'help'.\n");
    }
  }
  else
  {
    console_print (&console_structure,
                   "Available commands. (try 'help command' for help about a specific command)\n\n");

    for (counter = 0; counter < number_of_commands; counter++)
    {
      console_print_formatted (&console_structure,
                               "  %-10s %s\n", command[counter].name,
                               command[counter].description);
    }
  }
}

/* Configure IP networking. */

void command_ip (int number_of_arguments, char **argument)
{
  /* If no arguments are given, print information about the current
     interfaces. */

  if (number_of_arguments == 1)
  {
    ipv4_interface_type ipv4_interface;
    message_parameter_type message_parameter;
    unsigned int index;
    unsigned int amount;
    unsigned int flags;

    message_parameter.protocol = IPC_PROTOCOL_IPV4;
    message_parameter.message_class = IPC_IPV4_INTERFACE_GET_AMOUNT;
    message_parameter.data = &amount;
    message_parameter.length = 0;
    message_parameter.block = TRUE;
    ipc_send (ipv4_structure.output_mailbox_id, &message_parameter);

    message_parameter.length = sizeof (unsigned int);
    ipc_receive (ipv4_structure.input_mailbox_id, &message_parameter, NULL);

    for (index = 0; index < amount; index++)
    {
      message_parameter.data = &index;
      message_parameter.message_class = IPC_IPV4_INTERFACE_GET_NUMBER;
      message_parameter.length = sizeof (unsigned int);
      ipc_send (ipv4_structure.output_mailbox_id, &message_parameter);
      
      message_parameter.data = &ipv4_interface;
      message_parameter.length = sizeof (ipv4_interface_type);
      ipc_receive (ipv4_structure.input_mailbox_id,
                   &message_parameter, NULL);

      console_print_formatted (&console_structure, 
                               "Interface: %s\n",
                               ipv4_interface.identification);
      console_print_formatted (&console_structure,
                               "  Ethernet address: %02X:%02X:%02X:%02X:%02X:%02X\n",
                               ipv4_interface.hardware_address[0],
                               ipv4_interface.hardware_address[1],
                               ipv4_interface.hardware_address[2],
                               ipv4_interface.hardware_address[3],
                               ipv4_interface.hardware_address[4],
                               ipv4_interface.hardware_address[5]);
      console_print_formatted (&console_structure, 
                               "  IP address: %lu.%lu.%lu.%lu, netmask: %lu.%lu.%lu.%lu, gateway: %lu.%lu.%lu.%lu\n",
                               ipv4_interface.ip_address & 0xFF,
                               (ipv4_interface.ip_address >> 8) & 0xFF, 
                               (ipv4_interface.ip_address >> 16) & 0xFF, 
                               ipv4_interface.ip_address >> 24,
                               ipv4_interface.netmask & 0xFF,
                               (ipv4_interface.netmask >> 8) & 0xFF, 
                               (ipv4_interface.netmask >> 16) & 0xFF, 
                               ipv4_interface.netmask >> 24,
                               ipv4_interface.gateway & 0xFF,
                               (ipv4_interface.gateway >> 8) & 0xFF, 
                               (ipv4_interface.gateway >> 16) & 0xFF, 
                               ipv4_interface.gateway >> 24);
      console_print_formatted (&console_structure,
                               "  Flags: %s, %s\n", 
                               ipv4_interface.dhcp ? "DHCP" : "Static",
                               ipv4_interface.up ? "Up" : "Down");
    }

    ipv4_get_flags (&ipv4_structure, &flags);
    console_print_formatted (&console_structure, "General flags: %s\n",
                             (flags & IPC_IPV4_FLAG_FORWARD) == 
                             IPC_IPV4_FLAG_FORWARD ? "IP forwarding enabled" :
                             "IP forwarding disabled");
  }

  /* Otherwise, we are probably trying to set the interface up. */

  else if (number_of_arguments > 1)
  {
    ipv4_interface_type interface;
    message_parameter_type message_parameter;

    if (string_compare (argument[1], "--dhcp") == 0 &&
        number_of_arguments == 3)
    {
      interface.dhcp = TRUE;
      interface.up = TRUE;
      string_copy (interface.identification, argument[2]);

      message_parameter.protocol = IPC_PROTOCOL_IPV4;
      message_parameter.message_class = IPC_IPV4_INTERFACE_CONFIGURE;
      message_parameter.data = &interface;
      message_parameter.length = sizeof (ipv4_interface_type);
      message_parameter.block = TRUE;
      ipc_send (ipv4_structure.output_mailbox_id,
                &message_parameter);
    }
    
    /* Disable/enable IP forwarding. */

    else if (string_compare (argument[1], "--forward") == 0 &&
             number_of_arguments == 3)
    {
      bool forward;
      unsigned int flags;

      if (string_compare (argument[2], "on") == 0)
      {
        forward = TRUE;
      }
      else if (string_compare (argument[2], "off") == 0)
      {
        forward = FALSE;
      }
      else
      {
        console_print_formatted (&console_structure,
                                 "Syntax error. Try 'help %s'.\n",
                                 argument[0]);
        return;
      }

      ipv4_get_flags (&ipv4_structure, &flags);

      if (forward)
      {
        flags |= IPC_IPV4_FLAG_FORWARD;
      }

      ipv4_set_flags (&ipv4_structure, flags);
    }
    else if (string_compare (argument[1], "--set") == 0 &&
             number_of_arguments >= 5)
    {
      interface.dhcp = FALSE;
      interface.up = TRUE;
      string_copy (interface.identification, argument[2]);

      if (ipv4_string_to_binary_ip_address
          (argument[3], &interface.ip_address) != IPV4_RETURN_SUCCESS)
      {
        console_print (&console_structure, "Invalid IP address specified!\n");
        return;
      }

      if (ipv4_string_to_binary_ip_address
          (argument[4], &interface.netmask) != IPV4_RETURN_SUCCESS)
      {
        console_print (&console_structure, "Invalid netmask specified!\n");
        return;
      }

      if (number_of_arguments == 6)
      {
        if (ipv4_string_to_binary_ip_address
            (argument[5], &interface.gateway) != IPV4_RETURN_SUCCESS)
        {
          console_print (&console_structure, "Invalid gateway address specified!\n");
          return;
        }
      }
      else
      {
        interface.gateway = 0;
      }

      message_parameter.protocol = IPC_PROTOCOL_IPV4;
      message_parameter.message_class = IPC_IPV4_INTERFACE_CONFIGURE;
      message_parameter.data = &interface;
      message_parameter.length = sizeof (ipv4_interface_type);
      message_parameter.block = TRUE;
      ipc_send (ipv4_structure.output_mailbox_id,
                &message_parameter);
    }
    else
    {
      console_print_formatted (&console_structure,
                               "Unknown parameter: %s, or wrong number of arguments. Try 'help %s'.\n",
                               argument[1], argument[0]);
    }
  }
}

/* Get information about IRQ levels. */

void command_irq (int number_of_arguments, 
                  char **argument __attribute__ ((unused)))
{
  unsigned int irqs = KERNELFS_CLASS_IRQ_AMOUNT;
  kernelfs_irq_info_type kernelfs_irq_info;
  unsigned int index;

  if (number_of_arguments != 1)
  {
    console_print_formatted (&console_structure, 
                             "Wrong number of arguments! Try 'help %s'.\n",
                             argument[0]);
    return;
  }

  if (system_call_kernelfs_entry_read (&irqs) != STORM_RETURN_SUCCESS)
  {
    console_print (&console_structure,
                   "Couldn't read number of allocated IRQs from kernel.");
    return;
  }
  
  kernelfs_irq_info.kernelfs_class = KERNELFS_CLASS_IRQ_INFO;
  
  console_print_formatted (&console_structure,
                           "%-3s %-10s %-10s %-10s %-10s %s\n",
                           "IRQ", "Process ID", "Cluster ID",
                           "Thread ID", "Occurred", "Description");

  for (index = 0; index < irqs; index++)
  {
    kernelfs_irq_info.which = index;
    if (system_call_kernelfs_entry_read (&kernelfs_irq_info) !=
        STORM_RETURN_SUCCESS)
    {
      console_print (&console_structure, "Failed to get IRQ information");
    }
    else
    {
      console_print_formatted (&console_structure,
                               "%-3u %-10u %-10u %-10u %-10u %s\n",
                               kernelfs_irq_info.level,
                               kernelfs_irq_info.process_id,
                               kernelfs_irq_info.cluster_id,
                               kernelfs_irq_info.thread_id,
                               (unsigned int) kernelfs_irq_info.occurred,
                               kernelfs_irq_info.description);
    }
  }
}

/* Kill a thread. */

void command_kill (int number_of_arguments, char **argument)
{
  process_id_type thread_id;

  if (number_of_arguments != 2)
  {
    console_print_formatted (&console_structure, 
                             "Wrong number of arguments! Try 'help %s'.\n",
                             argument[0]);
    return;
  }

  if (string_to_number (argument[1], (unsigned int *) &thread_id, NULL) !=
      STRING_RETURN_SUCCESS)
  {
    console_print_formatted (&console_structure, 
                             "Invalid thread ID specified!");
    return;
  }

  system_call_thread_control (thread_id, THREAD_TERMINATE, 0);
}

/* Print information about used and available memory. */

void command_memory (int number_of_arguments __attribute__ ((unused)),
                     char **argument __attribute__ ((unused)))
{
  kernelfs_memory_info_type memory_info;

  memory_info.kernelfs_class = KERNELFS_CLASS_MEMORY_INFO;
  system_call_kernelfs_entry_read (&memory_info);
  console_print_formatted 
    (&console_structure, 
     "Physical memory: %luK total, %luK free, %luK used\n",
     memory_info.total_memory / 1024,  memory_info.free_memory / 1024,
     (memory_info.total_memory - memory_info.free_memory) / 1024);
  console_print_formatted 
    (&console_structure, 
     "Global memory: %lu total, %lu free, %lu used\n",
     memory_info.total_global_memory, memory_info.free_global_memory,
     memory_info.total_global_memory - memory_info.free_global_memory);
}

/* Get information about installed PCI devices. */

void command_pci (int number_of_arguments __attribute__ ((unused)),
                  char *argument[] __attribute__ ((unused)))
{
  unsigned int number_of_devices;

  pci_get_number_of_devices (&pci_structure, &number_of_devices);
  console_print_formatted (&console_structure, "Number of PCI devices: %u\n",
                           number_of_devices);
}

/* Get information about the status of the I/O ports in the system. */

void command_ports (int number_of_arguments, 
                    char *argument[] __attribute__ ((unused)))
{
  unsigned int ranges = KERNELFS_CLASS_PORT_AMOUNT;
  kernelfs_port_info_type kernelfs_port_info;
  unsigned int index;

  if (number_of_arguments != 1)
  {
    console_print_formatted (&console_structure, 
                             "Wrong number of arguments! Try 'help %s'.\n",
                             argument[0]);
    return;
  }

  if (system_call_kernelfs_entry_read (&ranges) != STORM_RETURN_SUCCESS)
  {
    console_print (&console_structure,
                   "Couldn't read information about the number of available port ranges from kernel.");
    return;
  }
  
  kernelfs_port_info.kernelfs_class = KERNELFS_CLASS_PORT_INFO;
  
  console_print_formatted (&console_structure,
                           "%-9s %-10s %-10s %-10s %s\n",
                           "Range", "Process ID",
                           "Cluster ID", "Thread ID", "Description");

  for (index = 0; index < ranges; index++)
  {
    kernelfs_port_info.port = index;
    system_call_kernelfs_entry_read (&kernelfs_port_info);

    console_print_formatted (&console_structure,
                             "%04X-%04X %-10u %-10u %-10u %s\n",
                             kernelfs_port_info.base,
                             kernelfs_port_info.base + 
                             kernelfs_port_info.length - 1,
                             kernelfs_port_info.process_id,
                             kernelfs_port_info.cluster_id,
                             kernelfs_port_info.thread_id,
                             kernelfs_port_info.description);
  }
}

/* List the processes. */

void command_processes (int number_of_arguments __attribute__ ((unused)), 
                        char **argument __attribute__ ((unused)))
{
  kernelfs_process_info_type kernelfs_process_info;
  kernelfs_thread_info_type kernelfs_thread_info;
  u32 processes = KERNELFS_CLASS_PROCESS_AMOUNT;

  system_call_kernelfs_entry_read (&processes);
  console_print (&console_structure, "PROCESS_ID THREADS NAME\n");
  kernelfs_process_info.kernelfs_class = KERNELFS_CLASS_PROCESS_INFO;

  for (kernelfs_process_info.process_number = 0;
       kernelfs_process_info.process_number < processes; 
       kernelfs_process_info.process_number++)
  {
    system_call_kernelfs_entry_read (&kernelfs_process_info);
    console_print_formatted (&console_structure, "%-10lu %-7u %s.\n",
                             kernelfs_process_info.process_id,
                             kernelfs_process_info.number_of_threads,
                             kernelfs_process_info.name);
 
    kernelfs_thread_info.process_id = kernelfs_process_info.process_id;
  }
}

/* Reboot. */

void command_reboot (int number_of_arguments __attribute__ ((unused)),
                     char **argument __attribute__ ((unused)))
{
  system_call_thread_control (THREAD_ID_KERNEL, THREAD_TERMINATE, 0);
}

/* Run a script. */

void command_run (int number_of_arguments, char *argument[])
{
  file_handle_type handle;
  u8 *buffer;
  file_verbose_directory_entry_type directory_entry;
  unsigned int where;

  if (number_of_arguments != 2)
  {
    console_print_formatted (&console_structure, 
                             "Wrong number of arguments! Try 'help %s'.\n",
                             argument[0]);
    return;
  }

  memory_allocate ((void **) &buffer, VFS_BUFFER_SIZE);
  
  string_copy (directory_entry.path_name, argument[1]);
  if (file_get_info (&vfs_structure, &directory_entry) != FILE_RETURN_SUCCESS)
  {
    console_print_formatted (&console_structure, 
                             "Could not get information about file %s.\n",
                             argument[1]);
    memory_deallocate ((void **) &buffer);
    return;
  }

  file_open (&vfs_structure, argument[1], FILE_MODE_READ, &handle);

  /* Read the file (hopefully, it's not more than
     VFS_BUFFER_SIZE...) */

  file_read (&vfs_structure, handle, directory_entry.size, buffer);

  for (where = 0; where < directory_entry.size; where++)
  {
    if (buffer[where] == '\n')
    {
      buffer[where] = '\0';
    }
  }

  where = 0;
  while (where < directory_entry.size)
  {
    console_print_formatted (&console_structure,
                             "%s\n", &buffer[where]);
    run (&buffer[where]);
    where += string_length (&buffer[where]) + 1;
  }
    
  memory_deallocate ((void **) &buffer);
}

/* Set an environment variable. */

void command_set (int number_of_arguments, char *argument[])
{
  int index;

  if (number_of_arguments != 3)
  {
    console_print_formatted (&console_structure, 
                             "Wrong number of arguments! Try 'help %s'.\n",
                             argument[0]);
    return;
  }

  for (index = 0; index < 16 && environment[index].key[0] != '\0'; index++)
  {
    if (string_compare (environment[index].key, argument[1]) == 0)
    {
      string_copy (environment[index].value, argument[2]);
      break;
    }
  }

  /* FIXME: Rewrite this to a bintree or whatever suitable... */

  if (index == 16)
  {
    /* Okay, this is a new entry in the environment. Find an empty
       spot. */

    for (index = 0; index < 16 && environment[index].key[0] != '\0'; index++);

    if (index == 16)
    {
      console_print (&console_structure, 
                     "No space left in the environment. Please kill some dolphins.\n");
      return;
    }

    string_copy (environment[index].key, argument[1]);
    string_copy (environment[index].value, argument[2]);
  }
  else if (environment[index].key[0] == '\0')
  {
    string_copy (environment[index].key, argument[1]);
    string_copy (environment[index].value, argument[2]);
  }
}

/* Show the environmental variables. */

void command_show (int number_of_arguments,
                   char *argument[] __attribute__ ((unused)))
{
  int index;

  if (number_of_arguments != 1)
  {
    console_print_formatted (&console_structure, 
                             "Wrong number of arguments! Try 'help %s'.\n",
                             argument[0]);
    return;
  }

  console_print_formatted (&console_structure, "  %-16s Value\n", "Key");
  for (index = 0; index < 16 && environment[index].key[0] != '\0'; index++)
  {
    console_print_formatted (&console_structure, 
                             "  %-16s %s\n", environment[index].key, 
                             environment[index].value);
  }
}

/* Show the contents of the given file. */

void command_show_file (int number_of_arguments,
                        char *argument[])
{
  file_handle_type handle;
  u8 *buffer;
  file_verbose_directory_entry_type directory_entry;
  u32 read_bytes = 0;

  if (number_of_arguments != 2)
  {
    console_print_formatted (&console_structure, 
                             "Wrong number of arguments! Try 'help %s'.\n",
                             argument[0]);
    return;
  }

  memory_allocate ((void **) &buffer, VFS_BUFFER_SIZE);
  
  string_copy (directory_entry.path_name, argument[1]);
  if (file_get_info (&vfs_structure, &directory_entry) != FILE_RETURN_SUCCESS)
  {
    console_print_formatted (&console_structure, 
                             "Could not get information about file %s.\n",
                             argument[1]);
    memory_deallocate ((void **) &buffer);
    return;
  }

  file_open (&vfs_structure, argument[1], FILE_MODE_READ, &handle);

  /* Read the file, in blocks of 4 Kbytes. */

  while (read_bytes < directory_entry.size)
  {
    unsigned int bytes = directory_entry.size - read_bytes;

    if (bytes > VFS_BUFFER_SIZE)
    {
      bytes = VFS_BUFFER_SIZE;
    }
    read_bytes += bytes;
    file_read (&vfs_structure, handle, bytes, buffer);
    buffer[bytes] = '\0';
    console_print (&console_structure, buffer);
  }

  memory_deallocate ((void **) &buffer);
}

/* Shut down all running processes. */

void command_shutdown (int number_of_arguments __attribute__ ((unused)),
                       char **argument __attribute__ ((unused)))
{
  /* FIXME: Let the console server handle longer messages. */

  console_clear (&console_structure);
  console_print (&console_structure, "\
\e[2J\e[37;44m







                                 \e[34;47m chaos 2000 \e[37;44m");
  console_print (&console_structure, "\n
    \e[1;37mA fatal exception 0E has occured at 0028:080054f3 in VXD VMM(01) +
    \e[1;37mC0CAC01A. The current application will be terminated.

");

  console_print (&console_structure, "\
    \e[1;37m*  Press any key to terminate the current application.
    \e[1;37m*  Press CTRL+ALT+DEL again to restart the computer. You will
    \e[1;37m   lose any unsaved information in all applications");
  console_print (&console_structure, "\
                                                                                
                             \e[1;37mPress any key to continue");
  while (TRUE);
}

/* Sleep for a given number of seconds. */

void command_sleep (int number_of_arguments, char *argument[])
{
  int seconds;

  if (number_of_arguments != 2)
  {
    console_print_formatted (&console_structure, 
                             "Wrong number of arguments! Try 'help %s'.\n",
                             argument[0]);
    return;
  }
  
  string_to_number (argument[1], &seconds, NULL);
  system_sleep (seconds * 1000);
}

/* Test command. When some functionality of storm, the libraries, the
   Universe, or whatever is a little untested, test it in this
   function. Nothing stays here forever. */

void command_test (int number_of_arguments __attribute__ ((unused)),
                   char *argument[] __attribute__ ((unused)))
{
  ipv4_receive_type receive;
  ipv4_socket_id_type socket_id;
  ipv4_send_type *send;
  ipv4_connect_type connect;
  ipv4_reconnect_type reconnect;
  u8 tftp_request[] = "\x00\x01/mnt/chaos/config/grub/menu\x00octet";
  u8 *buffer;
  unsigned int length;
  bool done = FALSE;
  u8 *file_buffer;
  unsigned int location = 0;

  memory_allocate ((void **) &file_buffer, 4096);

  connect.port = 69;
  connect.address = 192 + (168 << 8) + (1 << 16) + (234 << 24);
  connect.protocol = IPC_IPV4_PROTOCOL_UDP;

  ipv4_connect (&ipv4_structure, &connect, &socket_id);
  memory_allocate ((void **) &send, 
                   sizeof (ipv4_send_type) + sizeof (tftp_request));
  memory_allocate ((void **) &buffer, 516);
  send->length = sizeof (tftp_request);
  memory_copy (send->data, tftp_request, sizeof (tftp_request));
  send->socket_id = socket_id;
  ipv4_send (&ipv4_structure, send);

  length = 516;
  ipv4_receive (&ipv4_structure, &receive, socket_id, 
                (void **) &buffer, &length);
  reconnect.socket_id = socket_id;
  reconnect.address = receive.address;
  reconnect.port = receive.port;

  memory_copy (&file_buffer[location], buffer + 4, length - 4);
  location += length - 4;

  if (length - 4 != 512)
  {
    u16 ack[2];
    
    ack[0] = system_native_to_big_endian_u16 (4);
    ack[1] = ((u16 *) buffer)[1];
    send->length = 4;
    memory_copy (send->data, ack, 4);
    ipv4_send (&ipv4_structure, send);
  }
  else
  {
    ipv4_reconnect (&ipv4_structure, &reconnect);
    
    while (!done)
    {
      u16 ack[2];
      
      ack[0] = system_native_to_big_endian_u16 (4);
      ack[1] = ((u16 *) buffer)[1];
      send->length = 4;
      memory_copy (send->data, ack, 4);
      ipv4_send (&ipv4_structure, send);
      
      ipv4_receive (&ipv4_structure, &receive, socket_id, 
                    (void **) &buffer, &length);
      memory_copy (&file_buffer[location], buffer + 4, length - 4);
      location += length - 4;
      
      if (length != 512 + 4)
      {
        done = TRUE;
      }
    }
  }
  
  file_buffer[location] = '\0';
  console_print (&console_structure, file_buffer);

  // memory_deallocate ((void **) &send);
}

/* Show information about running threads. */

void command_threads (int number_of_arguments __attribute__ ((unused)),
                      char **argument __attribute__ ((unused)))
{
  kernelfs_process_info_type kernelfs_process_info;
  kernelfs_thread_info_type kernelfs_thread_info;
  u32 processes = KERNELFS_CLASS_PROCESS_AMOUNT;

  system_call_kernelfs_entry_read (&processes);
  console_print (&console_structure, "PROCESS_ID THREADS NAME\n");
  kernelfs_process_info.kernelfs_class = KERNELFS_CLASS_PROCESS_INFO;
  kernelfs_thread_info.kernelfs_class = KERNELFS_CLASS_THREAD_INFO;

  for (kernelfs_process_info.process_number = 0;
       kernelfs_process_info.process_number < processes; 
       kernelfs_process_info.process_number++)
  {
    system_call_kernelfs_entry_read (&kernelfs_process_info);
    console_print_formatted (&console_structure, "%-10lu %-7u %s.\n",
                             kernelfs_process_info.process_id,
                             kernelfs_process_info.number_of_threads,
                             kernelfs_process_info.name);
 
    kernelfs_thread_info.process_id = kernelfs_process_info.process_id;
    for (kernelfs_thread_info.thread_number = 0;
         kernelfs_thread_info.thread_number <
           kernelfs_process_info.number_of_threads;
         kernelfs_thread_info.thread_number++)
    {
      system_call_kernelfs_entry_read (&kernelfs_thread_info);
      console_print_formatted (&console_structure, 
                               "           %-7lu %s (%s)\n",
                               kernelfs_thread_info.thread_id,
                               kernelfs_thread_info.name,
                               kernelfs_thread_info.state);
    }
  }
}

/* Show the current time. */

void command_time (int number_of_arguments __attribute__ ((unused)),
                   char *argument[] __attribute__ ((unused)))
{
  kernelfs_time_type kernelfs_time;
  unsigned int hours, minutes, seconds;

  kernelfs_time.kernelfs_class = KERNELFS_CLASS_TIME_READ;
  system_call_kernelfs_entry_read (&kernelfs_time);

  hours = time_to_hours (kernelfs_time.time);
  minutes = time_to_minutes (kernelfs_time.time);
  seconds = time_to_seconds (kernelfs_time.time);

  console_print_formatted (&console_structure, "Current time: %02u:%02u:%02u\n", hours, minutes, 
                           seconds);
}

/* Top of Earth. */

void command_top (int number_of_arguments __attribute__ ((unused)),
                  char *argument[] __attribute__ ((unused)))
{
  kernelfs_process_info_type kernelfs_process_info;
  kernelfs_thread_info_verbose_type kernelfs_thread_info;
  u32 processes = KERNELFS_CLASS_PROCESS_AMOUNT;

  system_call_kernelfs_entry_read (&processes);
  console_print_formatted (&console_structure, 
                           "%-8s %-8s %-8s %-8s %-8s %-15s %-15s\n",
                           "Process", "Thread", "Time", "Memory", "IP",
                           "Process name", "Thread name");
  kernelfs_process_info.kernelfs_class = KERNELFS_CLASS_PROCESS_INFO;
  kernelfs_thread_info.kernelfs_class = KERNELFS_CLASS_THREAD_INFO_VERBOSE;

  for (kernelfs_process_info.process_number = 0;
       kernelfs_process_info.process_number < processes; 
       kernelfs_process_info.process_number++)
  {
    system_call_kernelfs_entry_read (&kernelfs_process_info);
    kernelfs_thread_info.process_id = kernelfs_process_info.process_id;

    for (kernelfs_thread_info.thread_number = 0;
         kernelfs_thread_info.thread_number <
           kernelfs_process_info.number_of_threads;
         kernelfs_thread_info.thread_number++)
    {
      system_call_kernelfs_entry_read (&kernelfs_thread_info);
      kernelfs_thread_info.process_name[15] = '\0';
      kernelfs_thread_info.thread_name[15] = '\0';
      console_print_formatted (&console_structure, 
                               "%-8lu %-8lu %-8lu %-8lu %08lX %-15s %-15s\n",
                               kernelfs_thread_info.process_id,
                               kernelfs_thread_info.thread_id,
                               (u32) kernelfs_thread_info.timeslices,
                               kernelfs_thread_info.main_memory / 1024,
                               /* kernelfs_thread_info.stack_memory / 1024, */
                               kernelfs_thread_info.instruction_pointer,
                               kernelfs_thread_info.process_name,
                               kernelfs_thread_info.thread_name);
    }
  }
}

/* Unset an environment variable. */

void command_unset (int number_of_arguments,
                    char *argument[])
{
  int index;
  int highest;
  int match = -1;

  if (number_of_arguments != 2)
  {
    console_print_formatted (&console_structure, 
                             "Wrong number of arguments! Try 'help %s'.\n",
                             argument[0]);
    return;
  }

  for (index = 0; index < 16 && environment[index].key[0] != '\0'; index++)
  {
    if (string_compare (environment[index].key, argument[1]) == 0)
    {
      match = index;
    }
  }

  highest = (index > 0 ? index - 1 : index);
  if (match == -1)
  {
    console_print (&console_structure, 
                   "The environment variable you tried to unset wasn't set.\n");
    return;
  }

  /* We have two cases to handle here. If this entry is the last, we
     just delete it. Otherwise, we move the last entry to this
     position and set the last as free. */

  if (highest == match)
  {
    environment[match].key[0] = '\0';
  }
  else
  {
    memory_copy (&environment[match], &environment[highest],
                 sizeof (environment_type));
    environment[highest].key[0] = '\0';
  }
}

/* Show the current uptime. */

void command_uptime (int number_of_arguments __attribute__ ((unused)),
                     char **argument __attribute__ ((unused)))
{
  u32 uptime = KERNELFS_CLASS_UPTIME_INFO;
  u32 days, hours, minutes, seconds;
  
  system_call_kernelfs_entry_read (&uptime);

  days = uptime / 86400;
  uptime %= 86400;

  hours = uptime / 3600;
  uptime %= 3600;
  
  minutes = uptime / 60;
  uptime %= 60;
  
  seconds = uptime;
    
  if (days > 0)
  {
    console_print_formatted (&console_structure, 
                             "System uptime: %lu days, %lu hours,"
                             " %lu minutes and %lu seconds.\n",
                             days, hours, minutes, seconds);
  }
  else
  {
    console_print_formatted (&console_structure, 
                             "System uptime: %lu hours, %lu minutes"
                             " and %lu seconds.\n",
                             hours, minutes, seconds);
  }
}

/* Shows the version of storm and cluido. */

void command_version (int number_of_arguments __attribute__ ((unused)),
                      char **argument __attribute__ ((unused)))
{
  kernelfs_storm_info_type storm_info;

  storm_info.kernelfs_class = KERNELFS_CLASS_STORM_INFO;
  system_call_kernelfs_entry_read (&storm_info);

  console_print_formatted (&console_structure, "storm version %s\n",
                           storm_info.version);
  console_print_formatted (&console_structure, "%s version %s\n",
                           PACKAGE_NAME, PACKAGE_VERSION);
}
