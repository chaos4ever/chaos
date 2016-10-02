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
    uint8_t operation;

    // 1 = 10Base-X. FIXME: Support this field properly.
    uint8_t hardware_type;

    uint8_t header_length;

    // Set to zero by client and updated by relay agents.
    uint8_t hops;

    // Transaction ID, chosen by client and used to identify the session.
    uint32_t transaction_id;

    // Filled in by the client and shows the seconds elapsed since the address requisition started.
    uint16_t seconds;
    uint16_t flags;

    // Only filled in if IP is up.
    uint32_t client_ip_address;

    // Used by BOOTREPLY.
    uint32_t assigned_ip_address;
    uint32_t next_server_ip_address;
    uint32_t relay_agent_ip_address;
    uint8_t client_hardware_address[16];

    // Optional server host name.
    uint8_t server_host_name[64];

    // Boot file name.
    uint8_t file[128];

    // Should be 99, 130, 83, 99
    uint8_t magic_cookie[4];

    // DHCP options are stored right here.
    uint8_t dhcp_options[0];
} __attribute__((packed)) dhcp_message_type;

typedef struct
{
    // 53.
    uint8_t code;

    // 1.
    uint8_t length;
    uint8_t type;
} __attribute__((packed)) dhcp_option_message_type;

typedef struct
{
    // 50.
    uint8_t code;

    // 1.
    uint8_t length;
    uint32_t ip;
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
