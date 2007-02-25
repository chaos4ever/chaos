/* $Id$ */
/* Abstract: Defines used by the serial library. */
/* Author: Martin Alvarez <malvarez@aapsa.es> */

/* Copyright 2000 chaos development. */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#ifndef __LIBRARY_SERIAL_DEFINES_H__
#define __LIBRARY_SERIAL_DEFINES_H__

/* Serial devices. */

enum
{
  SERIAL_TTYS0,
  SERIAL_TTYS1,
};

/* Parity */
enum
{
  SERIAL_PARITY_NONE,
  SERIAL_PARITY_ODD,
  SERIAL_PARITY_EVEN,
  SERIAL_PARITY_ONE,
  SERIAL_PARITY_ZERO
};

/* Control modes. */

#define SERIAL_BAUDRATE_MASK      0x0001
#define SERIAL_DATA_BITS_MASK     0x0002
#define SERIAL_STOP_BITS_MASK     0x0004
#define SERIAL_PARITY_MASK        0x0008
#define SERIAL_RX_FIFO_MASK       0x0010
#define SERIAL_TX_FIFO_MASK       0x0020
#define SERIAL_DTR_MASK           0x0040
#define SERIAL_RTS_MASK           0x0080
#define SERIAL_DSR_MASK           0x0100
#define SERIAL_CTS_MASK           0x0200
#define SERIAL_DCD_MASK           0x0400
#define SERIAL_RI_MASK            0x0800


/* Read modes. */

#define SERIAL_TIMEOUT  16

enum
{
  SERIAL_BLOCKED,
  SERIAL_BLOCKED_CHAR,
};

/* Buffer size. */

#define SERIAL_BUFFER_SIZE  100
  
#endif /* !__LIBRARY_SERIAL_DEFINES_H__ */
