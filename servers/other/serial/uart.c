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

/* Check for a UART at the given location. */

unsigned int uart_probe (unsigned int port_number)
{
  u16 base, x, olddata;
  
  base = serial_port[port_number].port;

  /* Check if a UART is present anyway. */

  olddata = system_port_in_u8 (base + REGISTER_MCR);
  system_port_out_u8 (base + REGISTER_MCR, 0x10);

  if ((system_port_in_u8 (base + REGISTER_MSR) & 0xF0) == 0xF0)
  {
    return UART_TYPE_NONE;
  }

  system_port_out_u8 (base + REGISTER_MCR, 0x1F);

  if ((system_port_in_u8 (base + REGISTER_MSR) & 0xF0) != 0xF0)
  {
    return UART_TYPE_NONE;
  }

  system_port_out_u8 (base + REGISTER_MCR, olddata);
  
  /* Next thing to do is look for the scratch register. */

  olddata = system_port_in_u8 (base + REGISTER_SCR);
  system_port_out_u8 (base + REGISTER_SCR, 0x55);

  if (system_port_in_u8 (base + REGISTER_SCR) != 0x55)
  {
    return UART_TYPE_8250;
  }

  system_port_out_u8 (base + REGISTER_SCR, 0xAA);

  if (system_port_in_u8 (base + REGISTER_SCR) != 0xAA)
  {
    return UART_TYPE_8250;
  }

  /* We don't need to restore it if it's not there. */

  system_port_out_u8 (base + REGISTER_SCR, olddata); 

  /* Then check if there's a FIFO. */

  system_port_out_u8 (base + REGISTER_IIR, 1);
  x = system_port_in_u8 (base + REGISTER_IIR);

  /* Some old-fashioned software relies on this! */
  
  system_port_out_u8 (base + REGISTER_IIR, 0x0);
 
  if ((x & 0x80) == 0)
  {
    return UART_TYPE_16450;
  }
  
  if ((x & 0x40) == 0)
  {
    return UART_TYPE_16550;
  }
  
  return UART_TYPE_16550A;
}

/* Initialize UART interrupts. */

void uart_irq_init (unsigned int port_number)
{
  u16 base;
  
  base = serial_port[port_number].port;

  /* MCR. Enable interrupt to PIC. */

  system_port_out_u8 (base + REGISTER_MCR, 0x08);

  /* Enable Rx and Line interrupts. */

  system_port_out_u8 (base + REGISTER_IER, 0x05);
}

/* Initialize FIFO. */

void uart_fifo_init (unsigned int port_number)
{
  u16 base;
  u8 data;
  
  base = serial_port[port_number].port;

  /* Reset FIFO, enable FIFO, 14 bytes Rx FIFO. */

  if (serial_port[port_number].use_fifo != FALSE)
  {
    data = 0xC7;

    if (serial_port[port_number].rx_fifo <= 1)
    {
      data = 0x07;
    }
    else if (serial_port[port_number].rx_fifo <= 4)
    {
      data = 0x47;
    }
    else if (serial_port[port_number].rx_fifo <= 8)
    {
      data = 0x87;
    }
    
    system_port_out_u8 (base + REGISTER_FCR, data);
  } 
}

void uart_set_baudrate (unsigned int port_number)
{
  u16 base;
  
  base = serial_port[port_number].port;

  /* Set DLAB. */

  system_port_out_u8 (base + REGISTER_LCR,
                      system_port_in_u8 (base + REGISTER_LCR) | 0x80);

  /* Baud rate divisor. */

  system_port_out_u16 (base,
                       (CLOCK_FREQUENCY / 16) / serial_port[port_number].baudrate);

  /* Clear DLAB. */

  system_port_out_u8 (base + REGISTER_LCR,
                      system_port_in_u8 (base + REGISTER_LCR) & 0x7F);
}

void uart_set_line_settings (unsigned int port_number)
{
  u16 base;
  u8 value;
  
  base = serial_port[port_number].port;

  value = 0;

  /* Data bits. */

  value |= serial_port[port_number].data_bits - 5;

  /* Stop bits. */

  if (serial_port[port_number].stop_bits != 0)
  {
    value |= 0x04;
  }

  /* Parity. */

  switch (serial_port[port_number].parity)
  {
    case SERIAL_PARITY_NONE:
    {
      break;
    }

    case SERIAL_PARITY_ODD:
    {
      value |= 0x08;
      break;
    }

    case SERIAL_PARITY_EVEN:
    {
      value |= 0x18;
      break;
    }

    case SERIAL_PARITY_ZERO:
    {
      value |= 0x38;
      break;
    }

    case SERIAL_PARITY_ONE:
    {
      value |= 0x28;
      break;
    }

    default:
    {
      log_print (&log_structure, LOG_URGENCY_EMERGENCY,
                 "Inconsistent internal parity value.");
      break;
    }
  }

  /* Write LCR register. */

  system_port_out_u8 (base + REGISTER_LCR, value);
}


void uart_set_modem_settings (unsigned int port_number)
{
  u16 base;
  u8 value;
  
  base = serial_port[port_number].port;

  /* Enable interrupt to PIC. */

  value = 0x08;

  /* DTR value. */

  value |= serial_port[port_number].dtr;

  /* RTS value. */

  value |= serial_port[port_number].rts >> 1;
  
  system_port_out_u8 (base + REGISTER_MCR, value);
}

void uart_get_modem_settings (unsigned int port_number)
{
  u16 base;
  u8 value;
  
  base = serial_port[port_number].port;

  value = system_port_in_u8 (base + REGISTER_MSR);

  /* CTS value. */

  serial_port[port_number].cts = (value & 0x10) >> 4;

  /* DSR value. */

  serial_port[port_number].dsr = (value & 0x20) >> 5;

  /* RI value. */

  serial_port[port_number].ri = (value & 0x40) >> 6;

  /* DCD value. */

  serial_port[port_number].dcd = (value & 0x80) >> 7;
}

/* Let's send a message for a pending read command. */

void send_data (unsigned int port_number)
{
  message_parameter_type message_parameter;
  serial_data_type data;
  u16 tmp;
  
  /* Send it to caller. */

  message_parameter.protocol = IPC_PROTOCOL_SERIAL;
  message_parameter.message_class = IPC_SERIAL_READ;
  message_parameter.length = sizeof (data);
  message_parameter.data = &data;
  message_parameter.block = FALSE;

  tmp = 0;
  while (tmp != serial_port[port_number].rx_pending)
  {
    data.data[tmp] = serial_port[port_number].rx_buffer[serial_port[port_number].rx_current];
    tmp++;
    serial_port[port_number].rx_current++;
    serial_port[port_number].rx_current %= BUFFER_SIZE;
  }

  data.status = SERIAL_RETURN_SUCCESS;
  data.size = serial_port[port_number].rx_pending;
  
  system_call_mailbox_send (serial_port[port_number].target_mailbox_id,
                            &message_parameter);
}

/* Check how many characters are unread in the input buffer. */
/* FIXME: Rename this function. */

unsigned int unread_size (unsigned int port_number)
{
  u16 size;

  if (serial_port[port_number].rx_end >= serial_port[port_number].rx_current)
  {
    size = (serial_port[port_number].rx_end -
            serial_port[port_number].rx_current);
  }
  else
  {
    size = (BUFFER_SIZE + serial_port[port_number].rx_end -
            serial_port[port_number].rx_current);
  }
  
  return size;
}

bool check_config_write (serial_data_type *data)
{
  if ((data->mask & SERIAL_BAUDRATE_MASK) != 0)
  {
    if (data->baudrate > CLOCK_FREQUENCY / 16)
    {
      return FALSE;
    }
  }
  
  if (data->mask & SERIAL_DATA_BITS_MASK)
  {
    if (data->data_bits < 5 || data->data_bits > 8)
    {
      return FALSE;
    }
  }
  
  if (data->mask & SERIAL_PARITY_MASK)
  {
    if (data->parity != SERIAL_PARITY_NONE &&
        data->parity != SERIAL_PARITY_ODD &&
        data->parity != SERIAL_PARITY_EVEN &&
        data->parity != SERIAL_PARITY_ZERO &&
        data->parity != SERIAL_PARITY_ONE)
    {
      return FALSE;
    }
  }
  
  if (data->mask & SERIAL_RX_FIFO_MASK)
  {
    if (data->rx_fifo != 1 && data->rx_fifo != 4 &&
        data->rx_fifo != 8 && data->rx_fifo != 14)
    {
      return FALSE;
    }
  }
  
  return TRUE;
}
