// Abstract: Global DMA defines.
// Authors: Henrik Hallin <hal@chaosdev.org>
//          Erik Moren <nemo@chaosdev.org>

// © Copyright 2000, 2013 chaos development.

// TODO: Can probably be moved to ia32. DMA is a very x86-specific thing...

#pragma once

enum
{
    STORM_DMA_OPERATION_WRITE,
    STORM_DMA_OPERATION_READ,
    STORM_DMA_OPERATION_VERIFY
};

enum
{
    STORM_DMA_TRANSFER_MODE_DEMAND,
    STORM_DMA_TRANSFER_MODE_SINGLE,
    STORM_DMA_TRANSFER_MODE_BLOCK,
    STORM_DMA_TRANSFER_MODE_CASCADE
};

enum
{
    STORM_DMA_AUTOINIT_ENABLE,
    STORM_DMA_AUTOINIT_DISABLE
};
