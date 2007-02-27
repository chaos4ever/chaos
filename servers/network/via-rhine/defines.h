/* $Id$ */
/* Abstract: Defines used by the via-rhine driver series. */
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


/* Fixme: Fakes. Remove these. */
/* --------------------------- */
#define NUMBER_OF_SEND_DESCRIPTORS 1
#define HZ 0
#define jiffies 0
#define KERN_WARNING 0
/* --------------------------- */


/* Keep the ring sizes a power of two for compile efficiency.
   The compiler will convert <unsigned>'%'<2^N> into a bit mask.
   Making the Tx ring too large decreases the effectiveness of channel
   bonding and packet priority.
   There are no ill effects from too-large receive rings. */

#define TX_RING_SIZE	16

/* Limit ring entries actually used.  */

#define TX_QUEUE_LEN	10
#define RX_RING_SIZE	16


#endif /* __DEFINES_H__ */
