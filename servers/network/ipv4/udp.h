// Abstract: UDP prototypes and structure definitions.
// Author: Per Lundberg <per@chaosdev.io>.
//
// © Copyright 1999-2000 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

// Read more about UDP in RFC 768.

#include <system/system.h>
#include "socket.h"

// Defines/enums.
// UDP ports.
enum
{
    UDP_DHCP_REQUEST = 67,
    UDP_DHCP_REPLY = 68,
    UDP_TFTP = 69,
    UDP_RWHO = 513,
};

enum
{
    UDP_RETURN_SUCCESS,
    UDP_RETURN_INVALID_ARGUMENT,
    UDP_RETURN_DESTINATION_UNREACHABLE,
};

// Type definitions.
// An UDP header.
typedef struct
{
    uint16_t source_port;
    uint16_t destination_port;
    uint16_t length;

    // The checksum is for all of the packet.
    uint16_t check;

    // Data starts here.
    uint8_t data[0];
} __attribute__((packed)) udp_header_type;

// Function prototypes.
extern void udp_header_create(uint16_t destination_port, uint16_t source_port, uint16_t length, udp_header_type *udp_header);
extern void udp_packet_receive(ipv4_interface_type *interface, ipv4_ethernet_header_type *ethernet_header,
                               int length __attribute__((unused)), mailbox_id_type output_mailbox_id);
return_type udp_connect(uint32_t destination, unsigned int destination_port, unsigned int *socket_id);
extern void udp_init(void);
extern return_type udp_send(void *data, unsigned int length, socket_type *socket);
