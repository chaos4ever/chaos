// Abstract: Ethernet library type defintions and function prototypes.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

#include <system/system.h>

// IEEE 802.3 ethernet magic constants. The frame sizes omit the preamble and FCS/CRC (frame check sequence).
#define IPV4_ETHERNET_ADDRESS_LENGTH         6
#define IPV4_ETHERNET_HEADER_LENGTH          14

// Minimum number of bytes in frame minus FCS.
#define IPV4_ETHERNET_MINIMUM_LENGTH         60

// Maximum number of bytes in payload.
#define IPV4_ETHERNET_DATA_LENGTH            1500

// Maximum number of bytes in frame minus FCS.
#define IPV4_ETHERNET_FRAME_LENGTH           1514

// These are the protocol ID's we support.
#define IPV4_ETHERNET_PROTOCOL_IPV4          0x0800
#define IPV4_ETHERNET_PROTOCOL_ARP           0x0806
#define IPV4_ETHERNET_PROTOCOL_IPV6          0x86DD

// This is an ethernet frame header.
typedef struct
{
    u8 destination_address[IPV4_ETHERNET_ADDRESS_LENGTH];
    u8 source_address[IPV4_ETHERNET_ADDRESS_LENGTH];
    u16 protocol_type;
    u8 data[0];
} __attribute__((packed)) ipv4_ethernet_header_type;
