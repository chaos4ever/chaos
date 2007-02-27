/* $Id$ */
/* Abstract: Definitions for the Via-rhine driver. */
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

#ifndef __VIA_RHINE_H__
#define __VIA_RHINE_H__

#include "config.h"
#include "types.h"
#include "enumerations.h"


/* 1 normal messages, 0 quiet .. 7 verbose. */

static int debug = 1;
static int max_interrupt_work = 20;


/* Set the copy breakpoint for the copy-only-tiny-frames
   scheme. Setting to > 1518 effectively disables this feature. */

static int receive_copybreak = 0;

/* Used to pass the media type, etc.
   Both 'options[]' and 'full_duplex[]' should exist for driver
   interoperability.
   The media type is usually passed in 'options[]'.
*/

/* More are supported, limit only on options */

#define MAX_UNITS 8
static int options[MAX_UNITS] = {-1, -1, -1, -1, -1, -1, -1, -1};
static int full_duplex[MAX_UNITS] = {-1, -1, -1, -1, -1, -1, -1, -1};


/* Maximum number of multicast addresses to filter
   (vs. rx-all-multicast).  The Rhine has a 64 element 8390-like hash
   table.  */

static const int multicast_filter_limit = 32;


/* Operational parameters that are set at compile time. */

/* Keep the ring sizes a power of two for compile efficiency.
   The compiler will convert <unsigned>'%'<2^N> into a bit mask.
   Making the Send ring too large decreases the effectiveness of channel
   bonding and packet priority.
   There are no ill effects from too-large receive rings. */

#define SEND_RING_SIZE	16

/* Limit ring entries actually used.  */

#define SEND_QUEUE_LEN	10
#define RECEIVE_RING_SIZE	16


/* Operational parameters that usually are not changed. */

/* Time in jiffies before concluding the transmitter is hung. */

#define SEND_TIMEOUT  (2*HZ)


/* Size of each temporary Receive buffer. */

#define PKT_BUF_SZ		1536


/* This driver was written to use PCI memory space, however most
   versions of the Rhine only work correctly with I/O space
   accesses. */

#if defined(VIA_USE_MEMORY)
#warning "Many adapters using the VIA Rhine chip are not configured to work"
#warning "with PCI memory space accesses."
#else
#define USE_IO
#endif


/* This table drives the PCI probe routines.  It's mostly boilerplate
   in all of the drivers, and will likely be provided by some future
   kernel.  Note the matching code -- the first table entry matchs all
   56** cards but second only the 1234 card. */


#if defined(VIA_USE_MEMORY)
#define RHINE_IOTYPE (PCI_USES_MEM | PCI_USES_MASTER | PCI_ADDR1)
#define RHINEII_IOSIZE 4096
#else
#define RHINE_IOTYPE (PCI_USES_IO  | PCI_USES_MASTER | PCI_ADDR0)
#define RHINEII_IOSIZE 256
#endif

/* directly indexed by enum via_rhine_chips, above */

static via_rhine_chip_info_type via_rhine_chip_info[] = 
{
  {
    "VIA VT86C100A Rhine", RHINE_IOTYPE, 128, CanHaveMII | ReqSendAlign
  },
  {
    "VIA VT6102 Rhine-II", RHINE_IOTYPE, RHINEII_IOSIZE, CanHaveMII | HasWOL
  },
  {
    "VIA VT3043 Rhine",    RHINE_IOTYPE, 128, CanHaveMII | ReqSendAlign
  }
};

#if FALSE
static pci_device_id via_rhine_pci_table[] __attribute__ ((unused)) = 
{
  {
    0x1106, 0x6100, PCI_ANY_ID, PCI_ANY_ID, 0, 0, VT86C100A
  },
  {
    0x1106, 0x3065, PCI_ANY_ID, PCI_ANY_ID, 0, 0, VT6102
  },
  {
    0x1106, 0x3043, PCI_ANY_ID, PCI_ANY_ID, 0, 0, VT3043
  },
  /* Terminate list. */
  {
    0, 0, 0, 0, 0, 0, 0
  }
};
#endif

#endif /* __VIA_RHINE_H__ */
