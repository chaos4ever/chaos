/* $Id$ */
/* Abstract: Types for the tulip driver series. */
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

/* Fixme: Should u32/u8 etc be included from storm/types.h ?! */

#include <storm/types.h>


/* Fixme: Fake types, for cutting down on compilation
   errors. _Remove_ and put in real types. =) */

typedef struct
{
  char *name;
  int if_port;
  void *private;
  int base_address;
  int address[3];
  int transmission_start;
  int irq;
  void *data;
  int tx_errors;
  int length;
  void *device;
  void *tail;
  int rx_missed_errors;
  int ifr_data;
  int flags;
  int mc_count;
  void *mc_list;
  void *next;
  void *dmi_addr;
  int subsystem_vendor;
  void *driver_data;
  int mtu;
  int mem_start;
  int watchdog_timeo;
  //  int (* open) (net_device *device);
  int (* open) (void *device);
  //  int (* hard_start_transmit) (sk_buff *skb, net_device *device);
  int (* hard_start_transmit) (void *skb, void *device);
  //  void (* tx_timeout) (net_device *device);
  void (* tx_timeout) (void *device);
  //  int (* stop) (net_device *device);
  int (* stop) (void *device);
  //  net_device_stats (* get_stats) (net_device *device);
  int (* get_stats) (void *device);
} net_device, pt_regs, sk_buff, net_device_stats,
  spinlock_t, pci_dev, media_timer, ifreq, device_mc_list;

typedef struct
{
  int expires;
  int data;
  void (* function) (unsigned long data);
} timer_list;

typedef int dma_addr_t;

typedef struct
{
  int driver_data, b, c, d, e, f, g;
} pci_device_id;




typedef struct
{
  char *chip_name;
  int io_size;

  /* CSR7 interrupt enable settings. */

  int valid_interrupts;
  int flags;

  void (*media_timer) (unsigned long data);
} tulip_chip_table;


typedef struct
{
  s32 status;
  s32 length;
  u32 buffer1;
  u32 buffer2;
} tulip_rx_desc;


typedef struct
{
  s32 status;
  s32 length;

  /* Note: We use only buffer 1.  */

  u32 buffer1;
  u32 buffer2;
} tulip_tx_desc;


typedef struct
{
  u8 type;
  u8 media;
  unsigned char *leaf_data;
} medialeaf;


typedef struct
{
  u16 default_media;
  u8 leaf_count;

  /* General purpose pin directions. */
  
  u8 csr12_direction;

  unsigned has_mii : 1;
  unsigned has_nonmii : 1;
  unsigned has_reset : 6;
  u32 csr15_direction;

  /* 21143 NWay setting. */

  u32 csr15_value;
  medialeaf mleaf[0];
} mediatable __attribute__ ((packed));


/* Fixme: Use the existing linked list library? */

struct mediainfo;

typedef struct mediainfo
{
  struct mediainfo *next;
  int info_type;
  int index;
  unsigned char *info;
} mediainfo;

typedef struct
{
  sk_buff *skb;
  dma_addr_t mapping;
} ring_info;


typedef struct
{
  const char *product_name;
  net_device *next_module;
  tulip_rx_desc *rx_ring;
  tulip_tx_desc *tx_ring;
  dma_addr_t rx_ring_dma;
  dma_addr_t tx_ring_dma;
  
  /* The saved address of a sent-in-place packet/buffer, for skfree(). */

  ring_info tx_buffers[TX_RING_SIZE];
  
  /* The addresses of receive-in-place skbuffs. */

  ring_info rx_buffers[RX_RING_SIZE];

  /* Pseudo-Tx frame to init address table. */

  u16 setup_frame[96];
  int chip_id;
  int revision;
  int flags;
  net_device_stats stats;

  /* Media selection timer. */

  timer_list timer;
  spinlock_t lock;

  /* The next free ring entry */

  unsigned int cur_rx, cur_tx;

  /* The ring entries to be free()ed. */

  unsigned int dirty_rx, dirty_tx;

  /* The Tx queue is full. */

  unsigned int tx_full : 1;

  /* Full-duplex operation requested. */

  unsigned int full_duplex : 1;
  unsigned int full_duplex_lock : 1;

  /* Multiport board faked address. */

  unsigned int fake_addr : 1;

  /* Last dev->if_port value. */

  unsigned int default_port : 4;

  /* Secondary monitored media port. */

  unsigned int media2 : 4;

  /* Don't sense media type. */

  unsigned int medialock : 1;

  /* Media sensing in progress. */

  unsigned int mediasense : 1;

  /* 21143 internal NWay. */

  unsigned int nway : 1;
  unsigned int nwayset : 1;

  /* CSR0 setting. */

  unsigned int csr0;

  /* Current CSR6 control settings. */

  u32 csr6;

  /* Serial EEPROM contents. */
  
  unsigned char eeprom[EEPROM_SIZE];

  void (*link_change) (net_device *device, int csr5);

  /* NWay capabilities advertised.  */

  u16 to_advertise;

  /* 21143 Link partner ability. */

  u16 lpar;
  u16 advertising[4];

  /* MII device addresses. */

  signed char phys[4], mii_cnt;
  mediatable *mtable;

  /* Current media index. */

  int cur_index;
  int saved_if_port;
  pci_dev *pci_device;
  int ttimer;
  int susp_rx;
  unsigned long nir;
  unsigned long base_address;

  /* Used for 8-byte alignment */

  int pad0, pad1;
} tulip_private __attribute__ ((packed));


typedef struct
{
  char *name;
  unsigned char address0;
  unsigned char address1;
  unsigned char address2;

  /* Max length below. */

  u16 new_table[32];
} eeprom_fixup;


#endif /* !__TYPES_H__ */
