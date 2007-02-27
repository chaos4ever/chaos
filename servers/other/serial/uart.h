#ifndef __UART_H__
#define __UART_H__

#include <serial/serial.h>

/* UART ports. */

#define REGISTER_DATA		0
#define REGISTER_BAUD		0
#define REGISTER_IER		1
#define REGISTER_IIR		2
#define REGISTER_FCR		2
#define REGISTER_LCR		3
#define REGISTER_MCR		4
#define REGISTER_LSR		5
#define REGISTER_MSR		6
#define REGISTER_SCR		7

#define CLOCK_FREQUENCY		1843200

enum
{
  UART_TYPE_NONE,
  UART_TYPE_8250,
  UART_TYPE_16450,
  UART_TYPE_16550,
  UART_TYPE_16550A
} uart_type;

/* Functions. */

extern unsigned int uart_probe (unsigned int port_number);
extern void uart_set_baudrate (unsigned int port_number);
extern void uart_set_line_settings (unsigned int port_number);
extern void uart_set_modem_settings (unsigned int port_number);
extern void uart_get_modem_settings (unsigned int port_number);
extern void uart_irq_init (unsigned int port_number);
extern void uart_fifo_init (unsigned int port_number);
extern void send_data (unsigned int port_number);
extern unsigned int unread_size (unsigned int port_number);
extern bool check_config_write (serial_data_type *data);

#endif /* !__UART_H__ */
