/* $Id$ */
/* Abstract: Management of the DMA hardware. It is used for
   transferring memory blocks to and from hardware. */
/* Author: Anders Ohrt <doa@chaosdev.org> */

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
   USA */

#include <storm/generic/defines.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/memory.h>
#include <storm/generic/memory_physical.h>
#include <storm/generic/memory_virtual.h>
#include <storm/generic/port.h>
#include <storm/generic/return_values.h>
#include <storm/generic/debug.h>

#include <storm/ia32/dma.h>

/* Unavailable channels. */

#define DMA_CHANNEL_MEMORY_REFRESH   0
#define DMA_CHANNEL_CASCADE          4

/* Controller registers. */

static const unsigned int dma_controller[NUMBER_OF_CONTROLLERS] =
{
  0x08, 0xD0
};

static const unsigned int dma_master_reset[NUMBER_OF_CONTROLLERS] =
{
  0x0D, 0xDA
};

static const unsigned int dma_master_mask[NUMBER_OF_CONTROLLERS] =
{
  0x0F, 0xDE
};

static const unsigned int dma_mask[NUMBER_OF_CONTROLLERS] =
{
  0x0A, 0xD4
};

static const unsigned int dma_mode[NUMBER_OF_CONTROLLERS] = 
{
  0x0B, 0xD6
};

static const unsigned int dma_flip_flop[NUMBER_OF_CONTROLLERS] =
{
  0x0C, 0xD8
};

/* Channel registers. */

static const unsigned int dma_page[NUMBER_OF_CHANNELS] =
{
  0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A 
};

static const unsigned int dma_address[NUMBER_OF_CHANNELS] = 
{
  0x00, 0x02, 0x04, 0x06, 0xC0, 0xC4, 0xC8, 0xCC 
};

static const unsigned int dma_count[NUMBER_OF_CHANNELS] =
{
  0x01, 0x03, 0x05, 0x07, 0xC2, 0xC6, 0xCA, 0xCE
};

#define DMA_RESET 0
#define DMA_MASK_ALL_CHANNELS 0x0F
#define CANCEL_DMA_TRANSFER 0x04

/* 
  Command Register specification:

   7 6 5 4 3 2 1 0
   | | | | | | | \---/ 0 - Memory-to-memory disable
   | | | | | | |     \ 1 - Memory-to-memory enable
   | | | | | | |
   | | | | | | |     / 0 - Channel 0 address hold disable
   | | | | | | \----|  1 - Channel 0 address hold enable
   | | | | | |       \ X - If bit 0 = 0
   | | | | | |
   | | | | | \-------/ 0 - Controller enable
   | | | | |         \ 1 - Controller disable
   | | | | |
   | | | | |         / 0 - Normal timing
   | | | | \--------|  1 - Compressed timing
   | | | |           \ X - If bit 0 = 1
   | | | |
   | | | \-----------/ 0 - Fixed priority
   | | |             \ 1 - Rotating priority
   | | |
   | | |             / 0 - Late write selection
   | | \------------|  1 - Extended write selection
   | |               \ X - If bit 3 = 1
   | |
   | \---------------/ 0 - DREQ sense active high
   |                 \ 0 - DREQ sense active low
   |
   \-----------------/ 0 - DACK sense active low
                     \ 1 - DACK sense active high
*/

#define DMA_COMMAND_MEMORY_TO_MEMORY_DISABLE (0)
#define DMA_COMMAND_MEMORY_TO_MEMORY_ENABLE (BIT_VALUE (0))

//#define DMA_COMMAND_CHANNEL_0_ADDRESS_HOLD_DISABLE (0)
#define DMA_COMMAND_CHANNEL_0_ADDRESS_HOLD_ENABLE (BIT_VALUE (1))

//#define DMA_COMMAND_CONTROLLER_ENABLE (0)
#define DMA_COMMAND_CONTROLLER_DISABLE (BIT_VALUE (2))

//#define DMA_COMMAND_NORMAL_TIMING (0)
#define DMA_COMMAND_COMPRESSED_TIMING (BIT_VALUE (3))

//#define DMA_COMMAND_FIXED_PRIORITY (0)
#define DMA_COMMAND_ROTATING_PRIORITY (BIT_VALUE (4))

//#define DMA_COMMAND_LATE_WRTE_SELECTION (0)
#define DMA_COMMAND_EXTENDED_WRITE_SELECTION (BIT_VALUE (5))

//#define DMA_COMMAND_DREQ_SENSE_ACTIVE_HIGH (0)
#define DMA_COMMAND_DREQ_SENSE_ACTIVE_LOW (BIT_VALUE (6))

//#define DMA_COMMAND_DACK_SENSE_ACTIVE_LOW (0)
#define DMA_COMMAND_DACK_SENSE_ACTIVE_HIGH (BIT_VALUE (7))

/*
  Mode Register specification:

   7 6 5 4 3 2 1 0
   | | | | | | \-\---/ 00 - Channel 0, 01 - Channel 1
   | | | | | |       \ 10 - Channel 2, 11 - Channel 3
   | | | | | |
   | | | | | |       / 00 - Verify transfer, 01 - Write transfer
   | | | | \-\------|  10 - Read transfer, 11 - Illegal
   | | | |           \ XX - If bits 6-7 = 11
   | | | |
   | | | \-----------/ 0 - Autoinitialization disabled
   | | |             \ 1 - Autoinitialization enabled
   | | |
   | | \-------------/ 0 - Address increment
   | |               \ 1 - Address decrement
   | |
   \-\---------------/ 00 - Demand, 01 - Single
                     \ 10 - Block, - 11 Cascade
*/

#define DMA_MODE_READ   (BIT_VALUE (3))
#define DMA_MODE_WRITE  (BIT_VALUE (2))
#define DMA_MODE_VERIFY (0)

#define DMA_MODE_AUTOINIT_ENABLE (BIT_VALUE (4))
#define DMA_MODE_AUTOINIT_DISABLE (0)

#define DMA_MODE_ADDRESS_INCREMENT (0)
#define DMA_MODE_ADDRESS_DECREMENT (BIT_VALUE (5))

#define DMA_MODE_DEMAND  (0)
#define DMA_MODE_SINGLE  (BIT_VALUE (6))
#define DMA_MODE_BLOCK   (BIT_VALUE (7))
#define DMA_MODE_CASCADE (BIT_VALUE (6) | BIT_VALUE (7))

static volatile dma_type dma_channel[NUMBER_OF_CHANNELS];
static bool more_than_16M;

/* Initialise the DMA code. */

void dma_init (void)
{
//  unsigned int controller;
  unsigned int channel;

  /* DMA. */

  port_range_link (0x0000, 32, "Primary DMA controller");
  port_range_link (0x0080, 16, "DMA page register");
  port_range_link (0x00C0, 32, "Secondary DMA controller");

  /* FIXME: Reserve all ports under DMA RESERVED PORTS */

  /* Master reset to all controllers */
  /* FIXME: The next piece of code made everything NOT working.
            I don't know why because it should work. */

/*
  for (controller = 0; controller < NUMBER_OF_CONTROLLERS; controller++)
  {
    port_out_u8 (dma_master_reset[controller], DMA_RESET);
  }
*/
  /* Mask all DMA channels in all controllers */
/*
  for (controller = 0; controller < NUMBER_OF_CONTROLLERS; controller++)
  {
    port_out_u8 (dma_master_mask[controller], DMA_MASK_ALL_CHANNELS);
  }
*/
  /* Configure DMA transfers. */

  /* Enable DMA controller, fixed priority (0 highest), disable memory
     to memory transfers. */
/*
  for (controller = 0; controller < NUMBER_OF_CONTROLLERS; controller++)
  {
    port_out_u8 (dma_controller[controller],
                 DMA_COMMAND_MEMORY_TO_MEMORY_DISABLE);
  }
*/
  more_than_16M = (memory_physical_get_number_of_pages() >
                   (16 * MB) / SIZE_PAGE);

  /* Set up the data structure. */

  for (channel = 0; channel < NUMBER_OF_CHANNELS; channel++)
  {
    dma_channel[channel].spinlock = MUTEX_SPIN_UNLOCKED;
    dma_channel[channel].process_id = PROCESS_ID_NONE;
    dma_channel[channel].cluster_id = CLUSTER_ID_NONE;
    dma_channel[channel].thread_id = THREAD_ID_NONE;
  }
}

/* Perform a DMA transfer on the given DMA channel. */

return_type dma_transfer (unsigned int channel, unsigned int buffer_size,
                          unsigned int operation, unsigned int transfer_mode,
			  unsigned int autoinit)
{
  /* Transfer mode. */

  u8 mode;

  /* Controller channel number. */

  u8 controller_channel;

  /* Controller number. */

  u8 controller;

  /* Make sure that the given DMA channel has been reserved by the
     calling process. */

  if (channel >= NUMBER_OF_CHANNELS)
  {
    return STORM_RETURN_INVALID_DMA_CHANNEL;
  }

  if (dma_channel[channel].process_id != current_tss->process_id)
  {
    return STORM_RETURN_ACCESS_DENIED;
  }

  /* Calculate controller channel. */

  controller_channel = channel % NUMBER_OF_CONTROLLERS;
  controller = channel / NUMBER_OF_CONTROLLERS;

  /* FIXME: Mutex? */

  /* Mask channnel. */

  port_out_u8 (dma_mask[controller], 0x04 | controller_channel);

  /* Reset flip-flop. */

  port_out_u8 (dma_flip_flop[controller], 0);

  /* There are 4 transfer types demand, single, block and cascade. In
     theory floppy dma channel 2 works with single transfer but I have
     tested it with block transfer and works ok. Anyway maybe another
     parameter to this function could be transfer mode, even autoinit
     option could be another parameter. */

  /* bits 76 are 00 demand, 01 single, 10 block, 11 cascade */

  if (operation == STORM_DMA_OPERATION_WRITE)
  {
    /* Write transfer without autoinit, with adress increment and it's a
       block transfer. */

    mode = (DMA_MODE_WRITE | DMA_MODE_AUTOINIT_DISABLE |
            DMA_MODE_ADDRESS_INCREMENT | DMA_MODE_BLOCK);
  }
  else if (operation == STORM_DMA_OPERATION_READ)
  {
    /* Read transfer without autoinit, with adress increment and
       it's a block transfer. */
    /*
      mode = (DMA_MODE_READ | DMA_MODE_AUTOINIT_DISABLE |
      DMA_MODE_ADDRESS_INCREMENT | DMA_MODE_BLOCK);*/
    
    /* DMA_MODE_BLOCK doesn't work with the SoundBlaster */
    /* FIXME: Implement parameters for MODE (block/single) and AUTOINIT-mode */
    
    mode = (DMA_MODE_READ | DMA_MODE_ADDRESS_INCREMENT);


  }
  else
  {
    return STORM_RETURN_INVALID_ARGUMENT;
  }

  /* Set up transfer in selected channel. */

  switch (transfer_mode)
  {
    case STORM_DMA_TRANSFER_MODE_DEMAND: 
    {
      mode |= DMA_MODE_DEMAND;
      break;
    }

    case STORM_DMA_TRANSFER_MODE_SINGLE:
    {
      mode |= DMA_MODE_SINGLE;
      break;
    }

    case STORM_DMA_TRANSFER_MODE_BLOCK:
    {
      mode |= DMA_MODE_BLOCK;
      break;
    }

    case STORM_DMA_TRANSFER_MODE_CASCADE:
    {
      mode |= DMA_MODE_CASCADE;
      break;
    }
  }

  if (autoinit == STORM_DMA_AUTOINIT_ENABLE)
  {
    mode |= DMA_MODE_AUTOINIT_ENABLE;
  }
  else
  {
    mode |= DMA_MODE_AUTOINIT_DISABLE;
  }

  port_out_u8 (dma_mode[controller], mode | controller_channel);

  /* Low, high and page buffer address. */

  port_out_u8 (dma_address[channel],
               (u8) ((unsigned int) dma_channel[channel].physical_buffer));
  port_out_u8 (dma_address[channel],
               (u8) ((unsigned int) dma_channel[channel].physical_buffer >> 8));
  port_out_u8 (dma_page[channel],
               (u8) ((unsigned int) dma_channel[channel].physical_buffer >> 16));

  /* Low, high count. */

  port_out_u8 (dma_count[channel], (u8) (buffer_size - 1));
  port_out_u8 (dma_count[channel], (u8) ((buffer_size - 1) >> 8));

  /* Release_channel. */

  port_out_u8 (dma_mask[controller], controller_channel);

  /* FIXME: Mutex ends? */

  return TRUE;
}

/* Cancel a DMA transfer. */

return_type dma_transfer_cancel (unsigned int channel)
{
  unsigned char controller_channel;

  if (channel >= NUMBER_OF_CHANNELS)
  {
    return STORM_RETURN_INVALID_DMA_CHANNEL;
  }

  /* No need to worry if transfer is really in progress. */
  /* Could be called anytime but would be well after a DMA
     transfer. */

  if ((dma_channel[channel].process_id != current_tss->process_id) ||
      (dma_channel[channel].cluster_id != current_tss->cluster_id) ||
      (dma_channel[channel].thread_id != current_tss->thread_id))
  {
    return STORM_RETURN_BUSY;
  }

  /* Calculate controller channel. */

  controller_channel = channel % NUMBER_OF_CONTROLLERS;

  /* Mask that channel. */

  port_out_u8 (dma_mask[channel], CANCEL_DMA_TRANSFER | controller_channel);

  return STORM_RETURN_SUCCESS;
}

/* Register the given DMA channel. */

return_type dma_register (unsigned int channel, void **dma_buffer)
{
  /* Make sure the DMA channel number is in the acceptable range. */

  if (channel >= NUMBER_OF_CHANNELS)
  {
    return STORM_RETURN_INVALID_DMA_CHANNEL;
  }

  /* Lock this channel's mutex. */

  mutex_spin_lock (dma_channel[channel].spinlock);

  /* Make sure we're not trying to screw someone else. */

  if (dma_channel[channel].process_id != PROCESS_ID_NONE)
  {
    mutex_spin_unlock (dma_channel[channel].spinlock);
    return STORM_RETURN_BUSY;
  }

  /* Allocate this channel. */

  dma_channel[channel].process_id = current_tss->process_id;
  dma_channel[channel].cluster_id = current_tss->cluster_id;
  dma_channel[channel].thread_id = current_tss->thread_id;

  /* If we have more that 16MB of memory, memory will be reserved for
     us already, so we just grab it. The #if-statements are a quick fix
     so that it assumes we have got more than 16M always */

#if FALSE
  if (more_than_16M)
#endif
  {
    int virtual_page;
    int physical_page = (BASE_DMA + (64 * KB) * channel) / SIZE_PAGE;

    return_type virtual_allocation;

    /* Now, try to allocate virtual memory for the mapping of the
       physical buffer. */

    mutex_kernel_wait (&memory_mutex);

    virtual_allocation = memory_virtual_allocate ((void *) &virtual_page,
                                                  (64 * KB) / SIZE_PAGE);

    mutex_kernel_signal (&memory_mutex);

    /* If it worked, we map the virtual page to the physical
       buffer. */

    if (virtual_allocation == STORM_RETURN_SUCCESS)
    {
      memory_virtual_map (virtual_page, physical_page,
                          (64 * KB) / SIZE_PAGE, PAGE_WRITABLE |
                          PAGE_NON_PRIVILEGED | PAGE_CACHE_DISABLE);

      dma_channel[channel].virtual_buffer =
        (void *) (virtual_page * SIZE_PAGE);
      dma_channel[channel].physical_buffer =
        (void *) (physical_page * SIZE_PAGE);
    }

    /* If it didn't work, we free the channel and exit. */

    else
    {
      dma_channel[channel].process_id = PROCESS_ID_NONE;
      mutex_spin_unlock (dma_channel[channel].spinlock);
      return STORM_RETURN_OUT_OF_MEMORY;
    }
  }

  /* If we have less that 16MB of memory, any memory allocated will be
     okay, so we just allocate 64KB. */

#if FALSE
  else /* !more_than_16M */
  {
    int physical_page;
    int virtual_page;
    return_type physical_allocation;
    return_type virtual_allocation;

    /* First, we allocate 64KB of physical memory. */

    mutex_kernel_wait (&memory_mutex);
    physical_allocation = memory_physical_allocate ((void *) &physical_page,
                                                    (64 * KB) / SIZE_PAGE,
                                                    "DMA buffers");
    mutex_kernel_signal (&memory_mutex);

    /* If it worked, we allocate virtual memory needed to map the
       physical memory. */

    if (physical_allocation == STORM_RETURN_SUCCESS)
    {
      mutex_kernel_wait (&memory_mutex);
      virtual_allocation = memory_virtual_allocate
        ((void *) &virtual_page, (64 * KB) / SIZE_PAGE);
      mutex_kernel_signal (&memory_mutex);
      
      if (virtual_allocation == STORM_RETURN_SUCCESS)
      {
        memory_virtual_map (virtual_page, physical_page,
                            (64 * KB) / SIZE_PAGE, PAGE_WRITABLE |
                            PAGE_NON_PRIVILEGED | PAGE_CACHE_DISABLE);
        dma_channel[channel].virtual_buffer =
          (void *) (virtual_page * SIZE_PAGE);
        dma_channel[channel].physical_buffer =
          (void *) (physical_page * SIZE_PAGE);
      }
      else
      {
        memory_deallocate ((void **) &dma_channel[channel].virtual_buffer);
        dma_channel[channel].process_id = PROCESS_ID_NONE;
        mutex_spin_unlock (dma_channel[channel].spinlock);

        return STORM_RETURN_OUT_OF_MEMORY;
      }
    }
    else /* physical_allocation != STORM_RETURN_SUCCESS */
    {
      dma_channel[channel].process_id = PROCESS_ID_NONE;
      mutex_spin_unlock (dma_channel[channel].spinlock);

      return STORM_RETURN_OUT_OF_MEMORY;
    }
  }
#endif
  *dma_buffer = dma_channel[channel].virtual_buffer;
  mutex_spin_unlock (dma_channel[channel].spinlock);

  return STORM_RETURN_SUCCESS;
}

/* Unregister the given DMA channel. */

return_type dma_unregister (unsigned int channel)
{
  /* Make sure the DMA channel number is in the acceptable range. */

  if (channel >= NUMBER_OF_CHANNELS)
  {
    return STORM_RETURN_INVALID_DMA_CHANNEL;
  }

  mutex_spin_lock (dma_channel[channel].spinlock);

  /* Make sure we're not trying to screw someone else. */
  
  if ((dma_channel[channel].process_id != current_tss->process_id) ||
      (dma_channel[channel].cluster_id != current_tss->cluster_id) ||
      (dma_channel[channel].thread_id = current_tss->thread_id))
  {
    mutex_spin_unlock (dma_channel[channel].spinlock);

    return STORM_RETURN_BUSY;
  }

  /* Deallocate this channel. */

  if (!more_than_16M)
  {
    memory_physical_deallocate
      ((unsigned int) dma_channel[channel].physical_buffer / SIZE_PAGE);
  }

  memory_virtual_deallocate
    ((unsigned int ) dma_channel[channel].virtual_buffer / SIZE_PAGE);

  dma_channel[channel].process_id = PROCESS_ID_NONE;
  dma_channel[channel].cluster_id = CLUSTER_ID_NONE;
  dma_channel[channel].thread_id = THREAD_ID_NONE;
  
  mutex_spin_unlock (dma_channel[channel].spinlock);

  return STORM_RETURN_SUCCESS;
}
