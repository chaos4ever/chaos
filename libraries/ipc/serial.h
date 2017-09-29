// Abstract: Serial server IPC message types.
// Author: Martin Alvarez <malvarez@aapsa.es>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013-2017 chaos development

#pragma once

// Message types.
enum
{
    IPC_SERIAL_READ,
    IPC_SERIAL_WRITE,
    IPC_SERIAL_CONFIG_READ,
    IPC_SERIAL_CONFIG_WRITE,
};
