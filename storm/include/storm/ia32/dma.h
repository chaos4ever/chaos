// Abstract: Function prototypes for DMA support.
// Author: Anders Öhrt <doa@chaosdev.org>
//         Per Lundberg <per@halleluja.nu>
//
// © Copyright 1999-2000, 2013 chaos development.

#pragma once

#include <storm/ia32/defines.h>
#include <storm/ia32/mutex.h>
#include <storm/ia32/types.h>
#include <storm/dma.h>

#define NUMBER_OF_CONTROLLERS              2
#define NUMBER_OF_CHANNELS_PER_CONTROLLER  4
#define NUMBER_OF_CHANNELS                 (NUMBER_OF_CONTROLLERS * \
                                            NUMBER_OF_CHANNELS_PER_CONTROLLER)

typedef struct
{

    // The process ID owning this DMA channel.

    process_id_type process_id;
    cluster_id_type cluster_id;
    thread_id_type thread_id;

    // This is the address of the buffer that the memory is put in, both
    //     it's physical and virtual address.

    void *physical_buffer;
    void *virtual_buffer;

    // A mutex surrounding the critical parts of the code, so we avoid
    //     multiple threads accessing the same DMA channel simultaneously.

    spinlock_type spinlock;
} dma_type;

extern void dma_init(void) INIT_CODE;
extern return_type dma_register(unsigned int channel, void **dma_buffer);
extern return_type dma_unregister(unsigned int channel);
return_type dma_transfer(unsigned int channel, unsigned int buffer_size, unsigned int operation, unsigned int transfer_mode,
                         unsigned int autoinit);
return_type dma_transfer_cancel(unsigned int channel);
