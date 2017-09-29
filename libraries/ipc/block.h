// Abstract: Protocol used for communicating with block devices.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013-2017 chaos development

#pragma once

#include <ipc/protocols.h>

enum
{
    IPC_BLOCK_READ = (IPC_PROTOCOL_BLOCK << 16),
    IPC_BLOCK_WRITE,
    IPC_BLOCK_GET_INFO,
};

// This structure is passed into IPC_BLOCK_READ.
typedef struct
{
    unsigned int start_block_number;
    unsigned int number_of_blocks;
} ipc_block_read_type;

// IPC_BLOCK_GET_INFO returns this structure.
typedef struct
{
    unsigned int block_size;
    unsigned int number_of_blocks;
    bool writable;
    bool readable;
    char label[64];
} ipc_block_info_type;
