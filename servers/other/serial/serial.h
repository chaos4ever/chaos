#ifndef __SERIAL_H__
#define __SERIAL_H__

/* FIXME: Future versions will map all COMs. */

#define NUMBER_OF_PORTS 	2
#define BUFFER_SIZE		4096

typedef struct
{
  uint8_t uart_type;
  volatile bool lock_rx;
  volatile bool lock_tx;
  uint16_t port;
  uint16_t open;
  uint8_t irq;
  bool use_fifo;
  uint8_t rx_buffer[BUFFER_SIZE];
  uint8_t tx_buffer[BUFFER_SIZE];
  uint16_t rx_current;
  uint16_t rx_end;
  uint16_t rx_pending;
  uint16_t tx_current;
  uint16_t tx_end;
  uint16_t errors;
  uint32_t baudrate;
  uint8_t data_bits;
  bool stop_bits;
  uint8_t parity;
  uint8_t rx_fifo;
  uint8_t tx_fifo;
  bool dtr;
  bool rts;
  bool dsr;
  bool cts;
  bool dcd;
  bool ri;
  uint16_t line_error;
  uint16_t overrun_error;
  uint16_t parity_error;
  uint16_t framing_error;

  mailbox_id_type target_mailbox_id;
} port_data_type;

extern port_data_type serial_port[NUMBER_OF_PORTS];
extern log_structure_type log_structure;

#endif /* !__SERIAL_H__ */
