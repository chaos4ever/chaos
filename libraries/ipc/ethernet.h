// Abstract: Protocol used by Ethernet servers.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013-2017 chaos development

#pragma once

#include <ipc/protocols.h>

enum
{
    IPC_ETHERNET_REGISTER_TARGET = (IPC_PROTOCOL_ETHERNET << 16),
    IPC_ETHERNET_PACKET_SEND,
    IPC_ETHERNET_PACKET_RECEIVED,
    IPC_ETHERNET_ADDRESS_GET,
};
