/* $Id$ */
/* Abstract: Function definitions for the serial library. */
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

#ifndef __LIBRARY_SERIAL_FUNCTIONS_H__
#define __LIBRARY_SERIAL_FUNCTIONS_H__

#include <ipc/ipc.h>
#include <system/system.h>

extern return_type serial_init (ipc_structure_type *ipc_structure, 
                                tag_type *tag);
extern return_type serial_read (ipc_structure_type *ipc_structure, 
                                unsigned char *data, unsigned int size);
extern return_type serial_write (ipc_structure_type *ipc_structure,
                                 void *string, unsigned int size);
extern return_type serial_config_line
  (ipc_structure_type *ipc_structure, unsigned int baudrate, 
   unsigned int data_bits, unsigned int stop_bits, unsigned int parity);

#endif /* !__LIBRARY_SERIAL_FUNCTIONS_H__ */
