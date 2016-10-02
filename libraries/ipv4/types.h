// Abstract: IPv4 types.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

#include <system/system.h>

typedef unsigned int ipv4_socket_id_type;
typedef uint32_t ipv4_address_type;

typedef struct
{
    ipv4_socket_id_type socket_id;
    unsigned int length;
    uint8_t data[0];
} ipv4_send_type;

typedef struct
{
    ipv4_address_type address;
    uint16_t port;
} ipv4_receive_type;

typedef struct
{
    ipv4_address_type address;
    uint16_t port;
    unsigned int protocol;
} ipv4_connect_type;

typedef struct
{
    ipv4_socket_id_type socket_id;
    ipv4_address_type address;
    uint16_t port;
} ipv4_reconnect_type;

typedef struct
{
    // FIXME: We can't use the define yet... but we should, in some way.
    uint8_t ethernet_address[6];
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

    uint32_t ip_address;
    uint32_t netmask;
    uint32_t gateway;
    uint8_t hardware_address[IPV4_ETHERNET_ADDRESS_LENGTH];
} ipv4_interface_type;
