/* $Id$ */
/* Abstract: Defines for the tulip driver series. */
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
 
#ifndef __DEFINES_H__
#define __DEFINES_H__

/* Fixme: Fake define, remove through recoding. */

#define HZ 0
#define jiffies 0
#define PCI_ANY_ID 0
#define PCI_DMA_TO_DEVICE 0
#define PCI_DMA_FROM_DEVICE 0
#define SA_SHIRQ 0
#define SIOCDEVPRIVATE 0
#define ENODEV 0
#define CAP_NET_ADMIN 0
#define EPERM 0
#define EOPNOTSUPP 0
#define IFF_PROMISC 0
#define IFF_ALLMULTI 0
#define ENOMEM 0
#define PCI_REVISION_ID 0


/* Keep the ring sizes a power of two for efficiency.
   Making the Tx ring too large decreases the effectiveness of channel
   bonding and packet priority.
   There are no ill effects from too-large receive rings. */

#define TX_RING_SIZE 16
#define RX_RING_SIZE 32

/* Size of each temporary Rx buffer. */

#define PKT_BUF_SZ 1536

/* Ring-wrap flag in length field, use for last ring entry.
     0x01000000 means chain on buffer2 address,
     0x02000000 means use the ring start address in CSR2/3.

   Note: Some work-alike chips do not function correctly in chained
   mode.  The ASIX chip works only in chained mode.  Thus we indicates
   ring mode, but always write the 'next' field for chained mode as
   well.
*/

#define DESC_RING_WRAP 0x02000000


/*  EEPROM_Ctrl bits: */

/* EEPROM shift clock. */

#define EE_SHIFT_CLK 0x02

/* EEPROM chip select. */

#define EE_CS 0x01

/* Data from the Tulip to EEPROM. */

#define EE_DATA_WRITE 0x04
#define EE_WRITE_0 0x01
#define EE_WRITE_1 0x05

/* Data from the EEPROM chip. */

#define EE_DATA_READ 0x08
#define EE_ENB (0x4800 | EE_CS)

/* Delay between EEPROM clock transitions.

   Even at 33Mhz current PCI implementations don't overrun the EEPROM
   clock.  We add a bus turn-around to insure that this remains
   true. */

#define eeprom_delay()	system_port_in_u32 (ee_address)

/* The EEPROM commands include the alway-set leading bit. */

#define EE_READ_CMD (6)


/* 2 << EEPROM_ADDRLEN */

#define EEPROM_SIZE 128

/* The maximum data clock rate is 2.5 Mhz.  The minimum timing is
   usually met by back-to-back PCI I/O cycles, but we insert a delay
   to avoid "overclocking" issues or future 66Mhz PCI. */

#define mdio_delay() system_port_in_u32 (mdio_addr)

/* Read and write the MII registers using software-generated serial
   MDIO protocol.  It is just different enough from the EEPROM
   protocol to not share code.  The maxium data clock rate is 2.5
   Mhz. */

#define MDIO_SHIFT_CLK 0x10000
#define MDIO_DATA_WRITE0 0x00000
#define MDIO_DATA_WRITE1 0x20000

/* Ignore the 0x02000 databook setting. */

#define MDIO_ENB 0x00000
#define MDIO_ENB_IN 0x40000
#define MDIO_DATA_READ 0x80000

#define RUN_AT(x) (jiffies + (x))


#endif /* !__DEFINES_H__ */
