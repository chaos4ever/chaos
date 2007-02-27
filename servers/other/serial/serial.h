#ifndef __SERIAL_H__
#define __SERIAL_H__

/* FIXME: Future versions will map all COMs. */

#define NUMBER_OF_PORTS 	2
#define BUFFER_SIZE		4096

typedef struct
{
  u8 uart_type;
  volatile bool lock_rx;
  volatile bool lock_tx;
  u16 port;
  u16 open;
  u8 irq;
  bool use_fifo;
  u8 rx_buffer[BUFFER_SIZE];
  u8 tx_buffer[BUFFER_SIZE];
  u16 rx_current;
  u16 rx_end;
  u16 rx_pending;
  u16 tx_current;
  u16 tx_end;
  u16 errors;
  u32 baudrate;
  u8 data_bits;
  bool stop_bits;
  u8 parity;
  u8 rx_fifo;
  u8 tx_fifo;
  bool dtr;
  bool rts;
  bool dsr;
  bool cts;
  bool dcd;
  bool ri;
  u16 line_error;
  u16 overrun_error;
  u16 parity_error;
  u16 framing_error;

  mailbox_id_type target_mailbox_id;
} port_data_type;

extern port_data_type serial_port[NUMBER_OF_PORTS];
extern log_structure_type log_structure;

#endif /* !__SERIAL_H__ */
