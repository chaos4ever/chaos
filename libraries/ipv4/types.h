// Abstract: IPv4 types.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development
// © Copyright 2015 chaos development

#pragma once

#include <system/system.h>

typedef unsigned int ipv4_socket_id_type;
typedef u32 ipv4_address_type;

typedef struct
{
    ipv4_socket_id_type socket_id;
    unsigned int length;
    u8 data[0];
} ipv4_send_type;

typedef struct
{
    ipv4_address_type address;
    u16 port;
} ipv4_receive_type;

typedef struct
{
    ipv4_address_type address;
    u16 port;
    unsigned int protocol;
} ipv4_connect_type;

typedef struct
{
    ipv4_socket_id_type socket_id;
    ipv4_address_type address;
    u16 port;
} ipv4_reconnect_type;

typedef struct
{
    // FIXME: We can't use the define yet... but we should, in some way.
    u8 ethernet_address[6];
    ipv4_address_type ip_address;
} ipv4_arp_entry_type;

// IP configuration.
typedef struct
{
    char identification[IPV4_INTERFACE_IDENTIFICATION_LENGTH];

    // Is the specified interface up?
    bool up;

    // Is the specified interface using DHCP? */
    bool dhcp;

    u32 ip_address;
    u32 netmask;
    u32 gateway;
    u8 hardware_address[IPV4_ETHERNET_ADDRESS_LENGTH];
} ipv4_interface_type;
