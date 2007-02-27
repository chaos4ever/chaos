/* $Id$ */
/* Abstract: Realtek definitions. */
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

#ifndef __REALTEK_PCI_H__
#define __REALTEK_PCI_H__

/* Number of Tx descriptor registers. */

#define NUMBER_OF_TX_DESCRIPTORS        4

/* Symbolic offsets to registers. */

enum realtek_registers 
{
  /* Ethernet hardware address. */

  MAC0 = 0,

  /* Multicast filter. */

  MAR0 = 8,

  /* Transmit status (Four 32bit registers). */

  TxStatus0 = 0x10,

  /* Tx descriptors (also four 32bit). */

  TxAddress0 = 0x20,
  RxBuffer = 0x30, RxEarlyCount = 0x34, RxEarlyStatus = 0x36,
  ChipCommand = 0x37, RxBufferPointer = 0x38, RxBufferAddress = 0x3A,
  InterruptMask = 0x3C, InterruptStatus = 0x3E,
  TxConfig = 0x40, RxConfig = 0x44,

  /* A general-purpose counter. */

  Timer = 0x48,

  /* 24 bits valid, write clears. */

  RxMissed = 0x4C,
  Config9346 = 0x50, Config0 = 0x51, Config1 = 0x52,
  FlashRegister = 0x54, GPPinData = 0x58, GPPinDir = 0x59, MII_SMI = 0x5A,
  HaltClock = 0x5B,
  MultiInterrupt = 0x5C, TxSummary=0x60,
  MII_BMCR = 0x62, MII_BMSR = 0x64, NWayAdvert = 0x66, NWayLPAR = 0x68,
  NWayExpansion = 0x6A,

  /* Undocumented registers, but required for proper operation. */
  /* FIFO Test Mode Select */
  
  FIFOTMS = 0x70,

  /* Chip Status and Configuration Register. */

  CSCR = 0x74,

  /* Magic transceiver parameter register. */

  PARA78 = 0x78, PARA7c = 0x7C,
};

enum chip_command_bits
{
  CommandReset = 0x10, CommandRxEnable = 0x08, CommandTxEnable = 0x04,
  RxBufferEmpty = 0x01
};

/* Bits in RxConfig. */

enum rx_mode_bits 
{
  AcceptErrror = 0x20, AcceptRunt = 0x10, AcceptBroadcast = 0x08,
  AcceptMulticast = 0x04, AcceptMyPhysical = 0x02, AcceptAllPhysical = 0x01,
};

enum rx_status_bits 
{
  RxMulticast = 0x8000, RxPhysical = 0x4000, RxBroadcast = 0x2000,
  RxBadSymbol = 0x0020, RxRunt = 0x0010, RxTooLong = 0x0008,
  RxCRCError = 0x0004, RxBadAlign = 0x0002, RxStatusOK = 0x0001,
};

enum tx_status_bits 
{
  TxHostOwns = 0x2000, TxUnderrun = 0x4000, TxStatusOK = 0x8000,
  TxOutOfWindow = 0x20000000, TxAborted = 0x40000000, 
  TxCarrierLost = 0x80000000,
};

enum interrupt_status_bits
{
  PCIError = 0x8000, PCSTimeout = 0x4000,
  RxFIFOOverrun = 0x40, RxUnderrun = 0x20, RxOverflow = 0x10,
  TxError = 0x08, TxOK = 0x04, RxError = 0x02, RxOK = 0x01,
};

/* Twister tuning parameters from RealTek.  Completely undocumented,
   but required to tune bad links. */

enum CSCRBits 
{
  CSCR_LinkOKBit = 0x0400, CSCR_LinkChangeBit = 0x0800,
  CSCR_LinkStatusBits = 0x0F000, CSCR_LinkDownOffCommand = 0x003C0,
  CSCR_LinkDownCommand = 0x0F3C0,
};

/* Serial EEPROM section. */
/*  EEPROM_Control bits. */

#define EEPROM_SHIFT_CLOCK      0x04
#define EEPROM_CHIP_SELECT      0x08
#define EEPROM_DATA_WRITE       0x02
#define EEPROM_WRITE_0		0x00
#define EEPROM_WRITE_1		0x02
#define EEPROM_DATA_READ	0x01

/* ? */

#define EEPROM_ENB              (0x80 | EEPROM_CHIP_SELECT)

/* The EEPROM commands include the alway-set leading bit. */

#define EEPROM_WRITE_COMMAND    (5 << 6)
#define EEPROM_READ_COMMAND     (6 << 6)
#define EEPROM_ERASE_COMMAND    (7 << 6)

/* MII serial management: mostly bogus for now. */
/* Read and write the MII management registers using
   software-generated serial MDIO protocol.

   The maximum data clock rate is 2.5 Mhz.  The minimum timing is
   usually met by back-to-back PCI I/O cycles, but we insert a delay
   to avoid "overclocking" issues. */

#define MDIO_DIR                0x80
#define MDIO_DATA_OUT           0x04
#define MDIO_DATA_IN            0x02
#define MDIO_CLOCK              0x01
#define MDIO_WRITE0             (MDIO_DIR)
#define MDIO_WRITE1             (MDIO_DIR | MDIO_DATA_OUT)

/* Size of the in-memory receive ring. */
/* 0 == 8K, 1 == 16K, 2 == 32K, 3 == 64K */

#define RX_BUFFER_LENGTH_INDEX  3
#define RX_BUFFER_LENGTH        (8192 << RX_BUFFER_LENGTH_INDEX)

/* Size of the Tx bounce buffers -- must be at least (MTU + 14 +
 * 4). */

#define TX_BUFFER_SIZE          1536

/* PCI Tuning Parameters. Threshold is bytes transferred to chip
   before transmission starts. */
/* In bytes, rounded down to 32 byte units. */

#define TX_FIFO_THRESHOLD       256

/* The following settings are log2 (bytes) - 4:  0 == 16 bytes .. 6 == 1024. */
/* Rx buffer level before first PCI transfer.  */

#define RX_FIFO_THRESHOLD       4

/* Maximum PCI burst, '4' is 256 bytes. */

#define RX_DMA_BURST            4

/* Calculate as 16 << value. */

#define TX_DMA_BURST            4

/* FIXME: the target stuff should be replaced by a linked list */

#define MAX_NUMBER_OF_TARGETS 10

typedef struct
{
  u16 protocol_type;
  mailbox_id_type mailbox_id;
} target_type;

typedef struct
{
  u8 ethernet_address[IPV4_ETHERNET_ADDRESS_LENGTH];
  const char *product_name;
  int chip_id;
  int chip_revision;

  /* The port base and IRQ for this device. */

  u16 port_base;
  u8 irq;

  /* Index into the Rx buffer of next Rx packet. */

  unsigned int current_rx;
  unsigned int current_tx, dirty_tx, tx_flag;

  /* Tx bounce buffers. */

  u8 *tx_buffer[NUMBER_OF_TX_DESCRIPTORS];
  u8 *rx_ring;

  /* Tx bounce buffer region. */

  u8 *tx_buffers;

  /* Physical addresses of the TX and RX buffers. */

  u8 *tx_buffers_dma;
  u8 *rx_ring_dma;

  /* MII device addresses. */

  int mii_address[4];

  /* Twister tune state. */

  char twistie, twist_count;

  /* The Tx queue is full. */
  
  bool tx_full;

  /* Full-duplex operation requested. */

  bool full_duplex;
  unsigned int duplex_lock : 1;
  unsigned int default_port : 4;

  /* Secondary monitored media port. */

  unsigned int media2 : 4;

  /* Don't sense media type. */

  unsigned int medialock : 1;

  /* Media sensing in progress. */

  unsigned int mediasense : 1;

  /* The targets specify which mailbox should get the packets. */

  unsigned int number_of_targets;
  target_type target[MAX_NUMBER_OF_TARGETS];
} realtek_device_type;

#endif /* !__REALTEK_PCI_H__ */
