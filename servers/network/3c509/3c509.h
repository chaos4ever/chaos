/* $Id$ */
/* Abstract: 3C509 header file. */
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

#ifndef __3C509_H__
#define __3C509_H__

/* To minimize the size of the driver source I only define operating
   constants if they are used several times.  You'll need the manual
   anyway if you want to understand driver details. */
/* Offsets from base I/O address. */

#define ETHERLINK3_DATA         0x00
#define ETHERLINK3_COMMAND      0x0E
#define ETHERLINK3_STATUS       0x0E
#define EEPROM_READ             0x80

#define ETHERLINK3_IO_EXTENT    16

#define ETHERLINK3WINDOW(win_num) outw(SelectWindow + (win_num), \
                                  io_address + ETHERLINK3_COMMAND)

/* The top five bits written to ETHERLINK3_COMMAND are a command, the lower
   11 bits are the parameter, if applicable. */

enum 
{
  TotalReset = 0 << 11, SelectWindow = 1 << 11, StartCoax = 2 << 11,
  RxDisable = 3 << 11, RxEnable = 4 << 11, RxReset = 5 << 11,
  RxDiscard = 8 << 11, TxEnable = 9 << 11, TxDisable = 10 << 11,
  TxReset = 11 << 11, FakeIntr = 12 << 11, AckIntr = 13 << 11,
  SetIntrEnb = 14 << 11, SetStatusEnb = 15 << 11, SetRxFilter = 16 << 11,
  SetRxThreshold = 17 << 11, SetTxThreshold = 18 << 11,
  SetTxStart = 19 << 11, StatsEnable = 21 << 11, StatsDisable = 22 << 11,
  StopCoax = 23 << 11,
};

enum
{
  InterruptLatch = 0x0001, AdapterFailure = 0x0002, TxComplete = 0x0004,
  TxAvailable = 0x0008, RxComplete = 0x0010, RxEarly = 0x0020,
  IntReq = 0x0040, StatsFull = 0x0080, CommandBusy = 0x1000, 
};

/* The SetRxFilter command accepts the following classes: */

enum
{
  RxStation = 1, RxMulticast = 2, RxBroadcast = 4, RxProm = 8 
};

/* Register window 1 offsets, the window used in normal operation. */

#define TX_FIFO         0x00
#define RX_FIFO         0x00
#define RX_STATUS       0x08
#define TX_STATUS       0x0B
#define TX_FREE         0x0C

/* Window 0: Set IRQ line in bits 12-15. */

#define WINDOW0_IRQ     0x08

/* Window 4: Various transcvr/media bits. */

#define WINDOW4_MEDIA   0x0A

/* Enable link beat and jabber for 10baseT. */

#define MEDIA_TP        0x00C0

/* FIXME: the target stuff should be replaced by a linked list */

#define MAX_NUMBER_OF_TARGETS 10

typedef struct
{
  u16 protocol_type;
  mailbox_id_type mailbox_id;
} target_type;

typedef struct
{
  int tbusy;
  volatile bool no_irq;
  int irq;
  long int base_addr;
  int mem_start;
  int mem_end;
  int interface_port;
  u8 ethernet_address[6];
  int priv;
  u8 name[256];
  int start;
  u32 *input_buffer;
  target_type target[MAX_NUMBER_OF_TARGETS];
  int number_of_targets;
} device_type;


static int etherlink3_open (device_type *dev);
static return_type etherlink3_start_transmit (void *data, u32 length,
                                       device_type *dev);
static void update_stats (device_type *dev);
static int etherlink3_receive (device_type *dev);
static void etherlink3_interrupt (device_type *device);

/* Time in jiffies before concluding the transmitter is hung. */

#define TX_TIMEOUT  (400 * HZ / 1000)

#endif /* !__3C509_H__ */
