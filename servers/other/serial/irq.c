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

/* Handle an IRQ. */

static void handle_irq (unsigned int port_number)
{
  u8 iir;
  u8 data;
  u8 tx_chars;
  u16 base;
  u16 tmp = 0;

  base = serial_port[port_number].port;
  iir = system_port_in_u8 (base + REGISTER_IIR);

  while ((iir & 0x01) == 0)
  {
    iir &= 0x06;
    iir >>= 1;
    
    if (iir == 0)
    {
      /* Modem status. */

      log_print (&log_structure, LOG_URGENCY_DEBUG, "Serial modem IRQ.");
      (void) system_port_in_u8 (base + REGISTER_MSR);
    }
    else if (iir == 1)
    {
      /* Ready to transmit. */

      tx_chars = 1;

      if (serial_port[port_number].use_fifo != FALSE)
      {
        tx_chars = serial_port[port_number].tx_fifo;
      }
      
      while (serial_port[port_number].lock_tx == TRUE)
      {
        system_call_dispatch_next ();
      }
      serial_port[port_number].lock_tx = TRUE;

      while (tx_chars != 0)
      {
        tx_chars--;
        
        if (serial_port[port_number].tx_current != serial_port[port_number].tx_end)
        {
          /* Send one more character. */

          data = serial_port[port_number].tx_buffer[serial_port[port_number].tx_current];
          serial_port[port_number].tx_current++;
          serial_port[port_number].tx_current %= BUFFER_SIZE;
          system_port_out_u8 (base + REGISTER_DATA, data);
        }
        else
        {
          /* No more data. Disable Tx interrupt. */

          data = system_port_in_u8 (base + REGISTER_IER);
          data &= 0xFD;
          system_port_out_u8 (base + REGISTER_IER, data);
          break;
        }
      }
      serial_port[port_number].lock_tx = FALSE;
    }
    else if (iir == 2)
    {
      /* Rx data available. */

      while (serial_port[port_number].lock_rx == TRUE)
      {
        system_call_dispatch_next ();
      }
      serial_port[port_number].lock_rx = TRUE;

      do
      {
        tmp++;
        data = system_port_in_u8 (base + REGISTER_DATA);
        serial_port[port_number].rx_buffer[serial_port[port_number].rx_end] =
          data;
        serial_port[port_number].rx_end++;
        serial_port[port_number].rx_end %= BUFFER_SIZE;

        if (serial_port[port_number].rx_end ==
            serial_port[port_number].rx_current)
        {
          log_print (&log_structure, LOG_URGENCY_ERROR,
                     "Rx buffer overflow.");
        }
      } while ((system_port_in_u8 (base + REGISTER_LSR) & 0x01) == 0x01);

      log_print_formatted (&log_structure, LOG_URGENCY_DEBUG,
                           "Received %d chars.", tmp);

      /* Check for pending messages to send. */
      
      if (serial_port[port_number].rx_pending != 0 &&
          unread_size (port_number) >= serial_port[port_number].rx_pending)
      {
        send_data (port_number);
        serial_port[port_number].rx_pending = 0;
      }

      serial_port[port_number].lock_rx = FALSE;

      /* FIXME: Prevent Tx lockup. */
  
    }
    else
    {
      /* Receiver line status. */

      log_print (&log_structure, LOG_URGENCY_DEBUG, "Serial line IRQ.");
      serial_port[port_number].line_error++;

      data = system_port_in_u8 (base + REGISTER_LSR);

      if ((data & 0x02) != 0)
      {
        serial_port[port_number].overrun_error++;
        log_print (&log_structure, LOG_URGENCY_DEBUG, "Overrun error.");
      }
      if ((data & 0x04) != 0)
      {
        serial_port[port_number].parity_error++;
        log_print (&log_structure, LOG_URGENCY_DEBUG, "Parity error.");
      }
      if ((data & 0x08) != 0)
      {
        log_print (&log_structure, LOG_URGENCY_DEBUG, "Framing error.");
        serial_port[port_number].framing_error++;
      }
      
      (void) system_port_in_u8 (base + REGISTER_DATA);
      serial_port[port_number].errors++;
    }
    
    iir = system_port_in_u8 (base + REGISTER_IIR);
  }
}

/* Interrupt handler. */

void irq_handler (unsigned int port_number)
{
  system_call_thread_name_set ("IRQ handler");

  if (system_call_irq_register (serial_port[port_number].irq, "Serial (UART)")
      != STORM_RETURN_SUCCESS)
  {
    log_print_formatted (&log_structure, LOG_URGENCY_EMERGENCY,
               "Could not allocate IRQ %d.", serial_port[port_number].irq);
    return;
  }

  while (TRUE)
  {
    system_call_irq_wait (serial_port[port_number].irq);
    handle_irq (port_number);
    system_call_irq_acknowledge (serial_port[port_number].irq);
  }
}
