// Abstract: IPv4 protocol
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2013-2017 chaos development

#pragma once

enum
{
    IPC_IPV4_INTERFACE_CONFIGURE = (IPC_PROTOCOL_IPV4 << 16),
    IPC_IPV4_INTERFACE_QUERY,

    // FIXME: Think about how this should really work.
    IPC_IPV4_CONNECT,
    IPC_IPV4_RECONNECT,
    IPC_IPV4_LISTEN,
    IPC_IPV4_SEND,
    IPC_IPV4_RECEIVE,
    IPC_IPV4_SET_HOST_NAME,
    IPC_IPV4_GET_HOST_NAME,
    IPC_IPV4_INTERFACE_GET_AMOUNT,
    IPC_IPV4_INTERFACE_GET_NUMBER,
    IPC_IPV4_SET_FLAGS,
    IPC_IPV4_GET_FLAGS,
    IPC_IPV4_ARP_GET_AMOUNT,
    IPC_IPV4_ARP_GET_NUMBER,
};

enum
{
    IPC_IPV4_PROTOCOL_UDP,
    IPC_IPV4_PROTOCOL_TCP
};

// Flags.
enum
{
    IPC_IPV4_FLAG_FORWARD = (1 << 0),
};
