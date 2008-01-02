/* $Id$ */
/* Abstract: Log server for chaos. This server's purpose is to collect
             debug and/or warnings from servers and from the
             kernel. */
/* Author: Per Lundberg <plundis@chaosdev.org>
           Henrik Hallin <hal@chaosdev.org> */

/* Copyright 2000 chaos development. */
/* Copyright 2007 chaos development. */

#include "config.h"

static console_structure_type console_structure_server;
static console_structure_type console_structure_kernel;

#define TITLE_FOREGROUND CONSOLE_COLOUR_GRAY
#define TITLE_BACKGROUND CONSOLE_COLOUR_BLUE
#define TITLE_ATTRIBUTE CONSOLE_ATTRIBUTE_BRIGHT

/* FIXME: This is ugly, but it really should not be exported. */

#define LOG_URGENCY_KERNEL (LOG_URGENCY_DEBUG + 1)

int urgency_colour[][3] =
{
  [LOG_URGENCY_EMERGENCY] =   { CONSOLE_COLOUR_GRAY,
                                CONSOLE_COLOUR_BLACK,
                                CONSOLE_ATTRIBUTE_BRIGHT },
  [LOG_URGENCY_ERROR] =       { CONSOLE_COLOUR_GRAY, 
                                CONSOLE_COLOUR_BLACK,
                                CONSOLE_ATTRIBUTE_BRIGHT },
  [LOG_URGENCY_WARNING] =     { CONSOLE_COLOUR_GRAY,
                                CONSOLE_COLOUR_BLACK,
                                CONSOLE_ATTRIBUTE_RESET },
  [LOG_URGENCY_INFORMATIVE] = { CONSOLE_COLOUR_GRAY,
                                CONSOLE_COLOUR_BLACK,
                                CONSOLE_ATTRIBUTE_RESET },
  [LOG_URGENCY_DEBUG] =       { CONSOLE_COLOUR_GRAY,
                                CONSOLE_COLOUR_BLACK,
                                CONSOLE_ATTRIBUTE_RESET },
  [LOG_URGENCY_KERNEL] =      { CONSOLE_COLOUR_GRAY,
                                CONSOLE_COLOUR_BLACK,
                                CONSOLE_ATTRIBUTE_RESET }
};

bool urgency_accept[] =
{
  [LOG_URGENCY_EMERGENCY] = TRUE,
  [LOG_URGENCY_ERROR] = TRUE,
  [LOG_URGENCY_WARNING] = TRUE,
  [LOG_URGENCY_INFORMATIVE] = TRUE,
  [LOG_URGENCY_DEBUG] = TRUE,
  [LOG_URGENCY_KERNEL] = TRUE
};

/* An empty tag list. */

tag_type empty_tag =
{
  0, 0, ""
};

/* Add a message to the log. Low-level function called from the
   wrappers. */

static void log_add (console_structure_type *console, char *title,
                     ipc_log_print_type *ipc_log_print)
{
  if (urgency_accept[ipc_log_print->urgency])
  {
    /* Print the log message with correct attributes. */

    console_attribute_set (console,
                           urgency_colour[ipc_log_print->urgency][0],
                           urgency_colour[ipc_log_print->urgency][1],
                           urgency_colour[ipc_log_print->urgency][2]);
    console_print_formatted (console, " [%s] %s ",
                             ipc_log_print->log_class, ipc_log_print->message);

    /* Go to next line (with correct colour). */

    console_attribute_set (console,
                           CONSOLE_COLOUR_GRAY,
                           CONSOLE_COLOUR_BLACK,
                           CONSOLE_ATTRIBUTE_RESET);
    console_print (console, "\n");

    /* Print the console title bar without moving the cursor. */

    console_attribute_set (console,
                           TITLE_FOREGROUND,
                           TITLE_BACKGROUND,
                           TITLE_ATTRIBUTE);
    console_print_formatted (console, "\e[s\e[1;1H%s\e[K\e[u", title);
  }
}

/* Handle an IPC connection request. */

static void handle_connection (mailbox_id_type reply_mailbox_id)
{
  message_parameter_type message_parameter;
  ipc_structure_type ipc_structure;
  bool done = FALSE;
  u32 *data;
  u32 **data_pointer = &data;
  unsigned int data_size = 1024;

  memory_allocate ((void **) data_pointer, data_size);

  /* Accept the connection. */ 

  ipc_structure.output_mailbox_id = reply_mailbox_id;
  ipc_connection_establish (&ipc_structure);

  message_parameter.data = data;
  message_parameter.block = TRUE;

  while (!done)
  {
    message_parameter.protocol = IPC_PROTOCOL_LOG;
    message_parameter.message_class = IPC_CLASS_NONE;
    message_parameter.length = data_size;
    
    if (ipc_receive (ipc_structure.input_mailbox_id, &message_parameter,
                     &data_size) != IPC_RETURN_SUCCESS)
    {
      continue;
    }
    
    switch (message_parameter.message_class)
    {
      case IPC_LOG_PRINT:
      {
        ipc_log_print_type *ipc_log_print = (ipc_log_print_type *) data;

        log_add (&console_structure_server,
                 " " PACKAGE_NAME " version " PACKAGE_VERSION " server console.",
                 ipc_log_print);

        break;
      }
    }
  }
}

return_type main (void)
{
  ipc_structure_type ipc_structure;

  /* Initialise the memory library. */

  memory_init ();

  /* Set our name. */

  system_process_name_set (PACKAGE_NAME);

  /* Unblock any servers that may be waiting after us. FIXME: This
     should be done later than this to eliminate unecessary
     waiting. */

  system_call_process_parent_unblock ();

  /* Create another thread to handle the server logging. */

  if (system_thread_create () == SYSTEM_RETURN_THREAD_OLD)
  {
    /* Open a new console for the log. */
  
    if (console_init (&console_structure_server, &empty_tag,
                      IPC_CONSOLE_CONNECTION_CLASS_CLIENT) !=
        CONSOLE_RETURN_SUCCESS)
    {
      return -1;
    }
    
    if (ipc_service_create ("log", &ipc_structure, 
                            &empty_tag) != IPC_RETURN_SUCCESS)
    {
      return -1;
    }
    
    if (console_open (&console_structure_server, 80, 50, 4,
                      VIDEO_MODE_TYPE_TEXT) != 
        CONSOLE_RETURN_SUCCESS)
    {
      return -1;
    }

    console_clear (&console_structure_server);

    /* Print the titlebar. */

    console_attribute_set (&console_structure_server,
                           TITLE_FOREGROUND,
                           TITLE_BACKGROUND,
                           TITLE_ATTRIBUTE);
    console_print (&console_structure_server,
                   " " PACKAGE_NAME " version " PACKAGE_VERSION " server console.\e[K\n");
    
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
      }
    }    
  }

  /* Kernel log handling thread. */

  else
  {
    kernelfs_log_type kernelfs_log;
    ipc_log_print_type *ipc_log_print;
    ipc_log_print_type **ipc_log_print_pointer = &ipc_log_print;

    memory_allocate ((void **) ipc_log_print_pointer, 1000);
    
    kernelfs_log.block = TRUE;
    kernelfs_log.kernelfs_class = KERNELFS_CLASS_LOG_READ;
    kernelfs_log.max_string_length = 1000;
    kernelfs_log.string = ipc_log_print->message;

    ipc_log_print->urgency = LOG_URGENCY_KERNEL;
    string_copy (ipc_log_print->log_class, "storm");

    system_thread_name_set ("Kernel log handler");

    /* Open a new console for the log. */
  
    if (console_init (&console_structure_kernel, &empty_tag,
                      IPC_CONSOLE_CONNECTION_CLASS_CLIENT) !=
        CONSOLE_RETURN_SUCCESS)
    {
      return -1;
    }
    
    if (console_open (&console_structure_kernel, 80, 50, 4, VIDEO_MODE_TYPE_TEXT) != 
        CONSOLE_RETURN_SUCCESS)
    {
      return -1;
    }

    console_clear (&console_structure_kernel);

    /* Print the titlebar. */

    console_attribute_set (&console_structure_kernel,
                           TITLE_FOREGROUND,
                           TITLE_BACKGROUND,
                           TITLE_ATTRIBUTE);
    console_print (&console_structure_kernel,
                   " " PACKAGE_NAME " version " PACKAGE_VERSION " kernel console.\e[K\n");

    while (TRUE)
    {
      system_call_kernelfs_entry_read (&kernelfs_log);

      log_add (&console_structure_kernel,
               " " PACKAGE_NAME " version " PACKAGE_VERSION " kernel console.",
               ipc_log_print);
    }    
  }
}
