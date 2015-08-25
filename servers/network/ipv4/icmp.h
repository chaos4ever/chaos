// Abstract: ICMP prototypes.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2015 chaos development

#pragma once

enum
{
    ICMP_ECHO_REPLY = 0,
    ICMP_ECHO_REQUEST = 8,
};

// The ICMP header type
typedef struct
{
    u8 type;
    u8 data[0];
} __attribute__((packed)) icmp_header_type;

// An ICMP echo packet.
typedef struct
{
    icmp_header_type header;
    u8 code;
    u16 checksum;
    u16 identifier;
    u16 sequence_number;
    u8 data[0];
} __attribute__((packed)) icmp_echo_type;

// Function prototypes.
extern void icmp_packet_receive(ipv4_interface_type *interface, ipv4_ethernet_header_type *ethernet_header,
                                int length, mailbox_id_type output_mailbox_id);
