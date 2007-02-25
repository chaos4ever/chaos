/* $Id$ */
/* Abstract: Types used by the serial library. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

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

#ifndef __LIBRARY_SERIAL_TYPES_H__
#define __LIBRARY_SERIAL_TYPES_H__

/* FIXME: Serial message structure. */

typedef struct
{
  u8 data[SERIAL_BUFFER_SIZE];
  u16 size;
  u32 mask;
  u32 baudrate;
  u8 data_bits;
  bool stop_bits;
  u8 parity;
  u8 rx_fifo;
  u8 tx_fifo;
  bool dtr;
  bool rts;
  bool dsr;
  bool cts;
  bool dcd;
  bool ri;
  return_type status;
} serial_data_type;

#endif /* !__LIBRARY_SERIAL_TYPES_H__ */
