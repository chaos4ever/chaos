// Abstract: DHCP structures and function prototypes.
// Author: Per Lundberg <per@chaosdev.io>.
//
// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

#include <ipc/ipc.h>
#include <system/system.h>

#include "ipv4.h"
#include "udp.h"

// For a full description of how this works, see RFC 2131.
enum
{
    BOOTREQUEST = 1,
    BOOTREPLY
};

// Optional tags.
enum
{
    DHCP_OPTION_REQUESTED_IP = 50,
    DHCP_OPTION_MESSAGE_TYPE = 53,
};

// DHCP message types.
enum
{
    DHCPDISCOVER = 1,
    DHCPOFFER,
    DHCPREQUEST,
    DHCPDECLINE,
    DHCPACK,
    DHCPNAK,
    DHCPRELEASE,
    DHCPINFORM
};

// A standard DHCP message.
typedef struct
{
    // Message type. 1 = BOOTREQUEST, 2 = BOOTREPLY.
    u8 operation;

    // 1 = 10Base-X. FIXME: Support this field properly.
    u8 hardware_type;

    u8 header_length;

    // Set to zero by client and updated by relay agents.
    u8 hops;

    // Transaction ID, chosen by client and used to identify the session.
    u32 transaction_id;

    // Filled in by the client and shows the seconds elapsed since the address requisition started.
    u16 seconds;
    u16 flags;

    // Only filled in if IP is up.
    u32 client_ip_address;

    // Used by BOOTREPLY.
    u32 assigned_ip_address;
    u32 next_server_ip_address;
    u32 relay_agent_ip_address;
    u8 client_hardware_address[16];

    // Optional server host name.
    u8 server_host_name[64];

    // Boot file name.
    u8 file[128];

    // Should be 99, 130, 83, 99
    u8 magic_cookie[4];

    // DHCP options are stored right here.
    u8 dhcp_options[0];
} __attribute__((packed)) dhcp_message_type;

typedef struct
{
    // 53.
    u8 code;

    // 1.
    u8 length;
    u8 type;
} __attribute__((packed)) dhcp_option_message_type;

typedef struct
{
    // 50.
    u8 code;

    // 1.
    u8 length;
    u32 ip;
} __attribute__((packed)) dhcp_option_requested_ip_type;

typedef struct
{
    ipv4_ethernet_header_type ethernet_header;
    ipv4_header_type ipv4_header;
    udp_header_type udp_header;
    dhcp_message_type dhcp_message;
} __attribute__((packed)) dhcp_packet_type;

// Function prototypes.
extern void dhcp_assign(ipv4_interface_type *ipv4_interface, ipc_structure_type *ethernet_structure);
