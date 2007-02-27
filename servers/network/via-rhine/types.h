/* $Id$ */
/* Abstract: Types used by the via-rhine driver seriers. */
/* Author: Anders Ohrt <doa@chaosdev.org> */

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

#ifndef __TYPES_H__
#define __TYPES_H__


#include "defines.h"


/* Fixme: Fakes. Remove these. */
/* --------------------------- */
typedef int sk_buff;
typedef int pt_regs;
typedef int net_device_stats ;
typedef int ifreq;
/* --------------------------- */


typedef struct
{
  u32 status;
  u32 length;
  void *address;
  void *next_descriptor;
} descriptor_type;


typedef struct
{
  const char *name;
  u16 pci_flags;
  int io_size;
  int drv_flags;
} chip_info_type;


typedef struct
{
  int expires;
  int data;
  void (* function) (unsigned long data);
} timer_type;


typedef struct
{
  /* Descriptor rings */
  descriptor_type *rx_ring;
  descriptor_type *tx_ring;
  //  dma_addr_t rx_ring_dma;
  //  dma_addr_t tx_ring_dma;

  /* The addresses of receive-in-place skbuffs. */
  //  sk_buff *rx_skbuff[RX_RING_SIZE];
  //  dma_addr_t rx_skbuff_dma[RX_RING_SIZE];

  /* The saved address of a sent-in-place packet/buffer, for later free(). */
  //  sk_buff *tx_skbuff[TX_RING_SIZE];
  //  dma_addr_t tx_skbuff_dma[TX_RING_SIZE];

  /* Tx bounce buffers */
  unsigned char *tx_buf[TX_RING_SIZE];
  unsigned char *tx_bufs;
  //  dma_addr_t tx_bufs_dma;

  //  pci_dev *pdev;
  //  net_device_stats stats;

  /* Media monitoring timer. */

  timer_type timer;
  //  spinlock_t lock;

  /* Frequently used values: keep some adjacent for cache effect. */
  int chip_id, drv_flags;
  descriptor_type*rx_head_desc;
  unsigned int cur_rx, dirty_rx;		/* Producer/consumer ring indices */
  unsigned int cur_tx, dirty_tx;
  unsigned int rx_buf_sz;				/* Based on MTU+slack. */
  u16 chip_cmd;						/* Current setting for ChipCmd */

  /* These values are keep track of the transceiver/media in use. */
  unsigned int full_duplex:1;			/* Full-duplex operation requested. */
  unsigned int duplex_lock:1;
  unsigned int default_port:4;		/* Last dev->if_port value. */
  u8 tx_thresh, rx_thresh;

  /* MII transceiver section. */
  u16 advertising;					/* NWay media advertisement */
  unsigned char phys[2];				/* MII device addresses. */
  u16 mii_status;						/* last read MII status */
} net_device_private_type __attribute__ ((packed));


typedef struct
{
#if FALSE
  u8 ethernet_address[IPV4_ETHERNET_ADDRESS_LENGTH];
  const char *product_name;
  int chip_id;
  int chip_revision;

  /* The port base and IRQ for this device. */

  u16 port_base;
  u8 irq;

  /* Index into the receive buffer of next receive packet. */

  unsigned int current_receive;
  unsigned int current_send, dirty_send, send_flag;

  /* Send bounce buffers. */

  u8 *send_buffer[NUMBER_OF_SEND_DESCRIPTORS];
  u8 *receive_ring;

  /* Send bounce buffer region. */

  u8 *send_buffers;

  /* Physical addresses of the send and receive buffers. */

  u8 *send_buffers_dma;
  u8 *receive_ring_dma;

  /* MII device addresses. */

  int mii_address[4];

  /* Twister tune state. */

  char twistie, twist_count;

  /* The send queue is full. */
  
  bool send_queue_full;

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
  //  target_type target[MAX_NUMBER_OF_TARGETS];
#endif
  int a;
} via_rhine_device_type __attribute__ ((packed));


typedef struct
{
  char *name;
  net_device_private_type *private;
  u16 base_address;
} net_device;


#endif /* __TYPES_H__ */
