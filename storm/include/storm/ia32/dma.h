/* $Id$ */
/* Abstract: Function prototypes for DMA support. */
/* Author: Anders Ohrt <doa@chaosdev.org>
           Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

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

#ifndef __STORM_IA32_DMA_H__
#define __STORM_IA32_DMA_H__

#include <storm/ia32/defines.h>
#include <storm/ia32/mutex.h>
#include <storm/ia32/types.h>
#include <storm/dma.h>

/* Defines. */

#define NUMBER_OF_CONTROLLERS              2
#define NUMBER_OF_CHANNELS_PER_CONTROLLER  4
#define NUMBER_OF_CHANNELS                 (NUMBER_OF_CONTROLLERS * \
                                           NUMBER_OF_CHANNELS_PER_CONTROLLER)

/* Type definitions. */

typedef struct
{
  /* The process ID owning this DMA channel. */

  process_id_type process_id;
  cluster_id_type cluster_id;
  thread_id_type thread_id;

  /* This is the address of the buffer that the memory is put in, both
     it's physical and virtual address. */

  void *physical_buffer;
  void *virtual_buffer;

  /* A mutex surrounding the critical parts of the code, so we avoid
     multiple threads accessing the same DMA channel simultaneously. */

  spinlock_type spinlock;
} dma_type;

/* Function prototypes. */

extern void dma_init (void) INIT_CODE;
extern return_type dma_register (unsigned int channel, void **dma_buffer);
extern return_type dma_unregister (unsigned int channel);
return_type dma_transfer (unsigned int channel, unsigned int buffer_size,
                          unsigned int operation, unsigned int transfer_mode,
            			  unsigned int autoinit);
return_type dma_transfer_cancel (unsigned int channel);

#endif	/* !__STORM_IA32_DMA_H__ */
