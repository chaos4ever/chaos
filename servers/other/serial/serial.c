/* $Id$ */
/* Abstract: Serial server for chaos. */
/* Authors: Martin Alvarez <malvarez@aapsa.es>
            Per Lundberg <plundis@chaosdev.org>
            Henrik Hallin <hal@chaosdev.org> */

/* Copyright 1999-2000 chaos development */

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

/* A lot of this code was borrowed from a serial port FAQ compiled by
   Christian Blum and contributed by lots of people; thank you for
   this information. */

#include "config.h"
#include "serial.h"
#include "irq.h"
#include "uart.h"

/* This has to be included from here. */

#include <serial/serial.h>

static u16 io_ports[NUMBER_OF_PORTS] =
{
  0x3F8,
  0x2F8
};

static u8 irqs[NUMBER_OF_PORTS] =
{
  4,
  3
};

port_data_type serial_port[NUMBER_OF_PORTS];
log_structure_type log_structure;

tag_type empty_tag =
{
  0, 0, ""
};

/* Initialise serial server. */

static bool init (unsigned int port_number)
{
  serial_port[port_number].baudrate = 9600;
  serial_port[port_number].data_bits = 8;
  serial_port[port_number].stop_bits = 0;
  serial_port[port_number].parity = SERIAL_PARITY_NONE;
  serial_port[port_number].rx_fifo = 14;
  serial_port[port_number].tx_fifo = 16;
  serial_port[port_number].dtr = 0;
  serial_port[port_number].rts = 0;
  serial_port[port_number].line_error = 0;
  serial_port[port_number].overrun_error = 0;
  serial_port[port_number].parity_error = 0;
  serial_port[port_number].framing_error = 0;

  serial_port[port_number].port = io_ports[port_number];
  serial_port[port_number].irq = irqs[port_number];
  serial_port[port_number].use_fifo = FALSE;
  serial_port[port_number].rx_current = 0;
  serial_port[port_number].rx_end = 0;
  serial_port[port_number].rx_pending = 0;
  serial_port[port_number].tx_current = 0;
  serial_port[port_number].tx_end = 0;
  serial_port[port_number].errors = 0;
  serial_port[port_number].lock_rx = FALSE;
  serial_port[port_number].lock_tx = FALSE;
  serial_port[port_number].target_mailbox_id = MAILBOX_ID_NONE;
    
  /* Register ports. */

  if (system_call_port_range_register (serial_port[port_number].port, 8,
                                       "Serial (UART controller)") !=
      STORM_RETURN_SUCCESS)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_EMERGENCY,
                         "Could not allocate port range %x-%x.",
                         serial_port[port_number].port, serial_port[port_number].port + 8);
    return FALSE;
  }

  serial_port[port_number].uart_type = uart_probe (port_number);
    
  if (serial_port[port_number].uart_type != UART_TYPE_NONE)
  {
    switch (serial_port[port_number].uart_type)
    {
      case UART_TYPE_8250:
      {
        log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                             "8250 UART found at 0x%X.", serial_port[port_number].port);
        break;
      }
      
      case UART_TYPE_16450:
      {
        log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                             "16450 UART found at 0x%X.", serial_port[port_number].port);
        break;
      }
      
      case UART_TYPE_16550:
      {
        log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                             "16550 UART found at 0x%X.", serial_port[port_number].port);
        break;
      }
      
      case UART_TYPE_16550A:
      {
        serial_port[port_number].use_fifo = TRUE;
        log_print_formatted (&log_structure, LOG_URGENCY_INFORMATIVE,
                             "16550A UART found at 0x%X.", serial_port[port_number].port);
        break;
      }
    }
        
    /* Initialise the port at 9600 8/N/1, enable Rx interrupts. */

    uart_set_baudrate (port_number);
    uart_set_line_settings (port_number);
    uart_irq_init (port_number);
    uart_fifo_init (port_number);
  }
  
  return TRUE;
}

/* Handle a connection request. */

static void handle_connection (unsigned int port_number,
                               mailbox_id_type reply_mailbox_id)
{
  bool done = FALSE;
  bool send;
  message_parameter_type message_parameter;
  serial_data_type data;
  ipc_structure_type ipc_structure;
  u16 x;
  u16 base;

  /* Accept the connection. */ 

  ipc_structure.output_mailbox_id = reply_mailbox_id;
  ipc_connection_establish (&ipc_structure);

  log_print (&log_structure, LOG_URGENCY_DEBUG,
             "Serial established connection.");

  /* Main loop. The connection is up, so we just handle the packets we
     get in the way we should. */

  while (!done)
  {
    message_parameter.protocol = IPC_PROTOCOL_NONE;
    message_parameter.message_class = IPC_CLASS_NONE;
    message_parameter.length = sizeof (serial_data_type);
    message_parameter.data = (void *) &data;
    message_parameter.block = TRUE;

    if (system_call_mailbox_receive
        (ipc_structure.input_mailbox_id, &message_parameter) !=
        STORM_RETURN_SUCCESS)
    {
      log_print (&log_structure, LOG_URGENCY_ERROR,
                 "Serial mailbox_receive failed.");
      continue;
    }

    send = TRUE;
    
    switch (message_parameter.message_class)
    {
      /* Someone wants to input data. */

      case IPC_SERIAL_READ:
      {
        /* Lock data access to this port. */

        while (serial_port[port_number].lock_rx == TRUE)
        {
          system_call_dispatch_next ();
        }
        serial_port[port_number].lock_rx = TRUE;

        /* TODO: Implement lists. */

        /* Check if we have enough data in input buffer. */

        /*if (serial_port[port_number].rx_pending == 0)
        {*/
	        serial_port[port_number].target_mailbox_id =  ipc_structure.output_mailbox_id;
	        serial_port[port_number].rx_pending = data.size;

	        if (unread_size (port_number) >= data.size)
	        {
	          send_data (port_number);
	        }
	          
	        send = FALSE;
	      /*}
        else
        {
          data.status = SERIAL_RETURN_ACCESS_DENIED;
          log_print (&log_structure, LOG_URGENCY_ERROR,
                     "Rx lists should be implemented asap.");
        }*/          
        serial_port[port_number].lock_rx = FALSE;
        break;
      }

      /* Someone wants to output data. */

      case IPC_SERIAL_WRITE:
      {
        log_print (&log_structure, LOG_URGENCY_DEBUG, "Serial write.");
          
        while (serial_port[port_number].lock_tx == TRUE)
        {
          system_call_dispatch_next ();
        }
        serial_port[port_number].lock_tx = TRUE;

        /* FIXME: use memory_copy? */
        /* TODO: Implement lists. */

        for (x = 0; x < data.size; x++)
        {
          serial_port[port_number].tx_buffer[serial_port[port_number].tx_end] = data.data[x];
          serial_port[port_number].tx_end++;
          serial_port[port_number].tx_end %= BUFFER_SIZE;

          if (serial_port[port_number].tx_end == serial_port[port_number].tx_current)
          {
            log_print (&log_structure, LOG_URGENCY_ERROR,
                       "Tx buffer overflow.");
          }
        }
        
        /* Enable Tx IRQ. */

        base = io_ports[port_number];
        system_port_out_u8 (base + REGISTER_IER,
                            system_port_in_u8 (base + REGISTER_IER) | 0x02);

        serial_port[port_number].lock_tx = FALSE;

        /* Answer OK. */

        data.status = SERIAL_RETURN_SUCCESS;
        break;
      }
        
      /* Set serial port parameters. */
 
      case IPC_SERIAL_CONFIG_READ:
      {
        log_print (&log_structure, LOG_URGENCY_DEBUG, "Serial config read.");

        data.baudrate = serial_port[port_number].baudrate;
        data.data_bits = serial_port[port_number].data_bits;
        data.stop_bits = serial_port[port_number].stop_bits;
        data.parity = serial_port[port_number].parity;
        data.rx_fifo = serial_port[port_number].rx_fifo;
        data.tx_fifo = serial_port[port_number].tx_fifo;
        data.dtr = (serial_port[port_number].dtr == FALSE) ? TRUE : FALSE;
        data.rts = (serial_port[port_number].rts == FALSE) ? TRUE : FALSE;

        uart_get_modem_settings (port_number);
          
        data.dsr = (serial_port[port_number].dsr == FALSE) ? TRUE : FALSE;
        data.cts = (serial_port[port_number].cts == FALSE) ? TRUE : FALSE;
        data.dcd = (serial_port[port_number].dcd == FALSE) ? TRUE : FALSE;
        data.ri = (serial_port[port_number].ri == FALSE) ? TRUE : FALSE;
                                                                                                                        
        /* Answer OK. */

        data.status = SERIAL_RETURN_SUCCESS;
        break;
      }

      /* Get serial port parameters. */
 
      case IPC_SERIAL_CONFIG_WRITE:
      {
        log_print (&log_structure, LOG_URGENCY_DEBUG, "Serial config write.");
          
        /* TODO: Check input parameters here or in library? */

        if (check_config_write (&data) != FALSE)
        {
          if ((data.mask & SERIAL_BAUDRATE_MASK) != 0)
          {
            serial_port[port_number].baudrate = data.baudrate;
            uart_set_baudrate (port_number);
           }

          if (data.mask & SERIAL_DATA_BITS_MASK)
          {
            serial_port[port_number].data_bits = data.data_bits;
          }
	          
          if (data.mask & SERIAL_STOP_BITS_MASK)
          {
            serial_port[port_number].stop_bits = (data.stop_bits == 0) ? 0 : 1;
          }

          if (data.mask & SERIAL_PARITY_MASK)
          {
            serial_port[port_number].parity = data.parity;
          }

          uart_set_line_settings (port_number);
	  
          if (data.mask & SERIAL_DTR_MASK)
          {
            serial_port[port_number].dtr = (data.dtr == FALSE) ? TRUE : FALSE;
          }
	          
          if (data.mask & SERIAL_RTS_MASK)
          {
            serial_port[port_number].rts = (data.rts == FALSE) ? TRUE : FALSE;
          }

          uart_set_modem_settings (port_number);

          if (data.mask & SERIAL_RX_FIFO_MASK)
          {
            serial_port[port_number].rx_fifo = data.rx_fifo;

            /* FIXME: Cant initialise fifo here. */

            uart_fifo_init (port_number);
          }

	           
          /* Answer OK. */

          data.status = SERIAL_RETURN_SUCCESS;
        }
        else
        {
          data.status = SERIAL_RETURN_INVALID_ARGUMENT;
        }
	        
        break;
      }

      default:
      {
        log_print (&log_structure, LOG_URGENCY_ERROR, "Unknown command.");
        data.status = SERIAL_RETURN_COMMAND_UNAVAILABLE;
        break;
      }
    }

    /* Send it to caller. */

    if (send != FALSE)
    {
      message_parameter.protocol = IPC_PROTOCOL_SERIAL;
      message_parameter.length = sizeof (data);
      message_parameter.data = (void *) &data;
      message_parameter.block = FALSE;

      system_call_mailbox_send (ipc_structure.output_mailbox_id,
                               &message_parameter);
    }
  }
}


static int task_port (u8 port_number)
{
  ipc_structure_type ipc_structure;
  u8 *service_name = "serial0";

  /* TODO: command-line options. */
  
  if (init (port_number) == FALSE)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY,
               "Failed initialisation.");
    return -1;
  }

  /* Handle IRQ. */
  /* TODO: change if 2 serial ports can have the same irq. */

  if (system_call_thread_create () == SYSTEM_RETURN_THREAD_NEW)
  {
    irq_handler (port_number);
    return -1;
  }

  /* Create service. */

  service_name[6] = 0x30 + port_number;
  if (ipc_service_create (service_name, &ipc_structure, 
                          &empty_tag) != IPC_RETURN_SUCCESS)
  {
    log_print (&log_structure, LOG_URGENCY_EMERGENCY, "Couldn't create service.");
    return -1;
  }
  
  /* Main loop.	*/

  system_call_thread_name_set ("Service handler");
  
  while (TRUE)
  {
    mailbox_id_type reply_mailbox_id;
    
    ipc_service_connection_wait (&ipc_structure);
    reply_mailbox_id = ipc_structure.output_mailbox_id;
    
    if (system_call_thread_create () == SYSTEM_RETURN_THREAD_NEW)
    {
      system_call_thread_name_set ("Handling connection");

      handle_connection (port_number, reply_mailbox_id);
    }
  }

  return 0;
}

      
/* Server entry point. */

int main (void)
{
  /* Set our name. */

  system_call_process_name_set ("serial");
  system_call_thread_name_set ("Initialising");

  log_init (&log_structure, PACKAGE_NAME, &empty_tag);
  
  /* Create threads handling COM1. */

  if (system_call_thread_create () == SYSTEM_RETURN_THREAD_NEW)
  {
    task_port (0);
    return -1;
  }

  /* Create threads handling COM2. */

  if (system_call_thread_create () == SYSTEM_RETURN_THREAD_NEW)
  {
    task_port (1);
    return -1;
  }

  return 0;
}
