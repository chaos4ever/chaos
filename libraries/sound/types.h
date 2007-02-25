/* Abstract: Types used by the sound library. */
/* Author: Erik Moren <nemo@chaosdev.org> */

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

#ifndef __LIBRARY_SOUND_TYPES_H__
#define __LIBRARY_SOUND_TYPES_H__

/* FIXME: Sound message structure. */

typedef struct
{
  unsigned int frequency;
  unsigned int bits;
  /* This is the size of the next sample played, or the size of the
     buffers used when streaming data */
  unsigned int length;

  /* A pointer to the data sent */
  u8 data[0];

} __attribute__ ((packed)) sound_message_type;

#endif /* !__LIBRARY_SOUND_TYPES_H__ */
