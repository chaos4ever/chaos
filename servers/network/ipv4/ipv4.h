// Abstract: IPv4 prototypes and structure definitions.
// Author: Per Lundberg <per@chaosdev.io>.
//
// © Copyright 1999-2000 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

#include <ipv4/ipv4.h>
#include <log/log.h>
#include <mutex/mutex.h>
#include <system/system.h>

// Structures.
// IP protocol types.
enum
{
    // Dummy protocol for TCP.
    IP_PROTOCOL_IP = 0,

    // IPv6 Hop-by-Hop options.
    IP_PROTOCOL_HOPOPTS = 0,

    // Internet Control Message Protocol.
    IP_PROTOCOL_ICMP = 1,

    // Internet Group Management Protocol.
    IP_PROTOCOL_IGMP = 2,

    // IPIP tunnels (older KA9Q tunnels use 94).
    IP_PROTOCOL_IPIP = 4,

    // Transmission Control Protocol.
    IP_PROTOCOL_TCP = 6,

    // Exterior Gateway Protocol.
    IP_PROTOCOL_EGP = 8,

    // PUP protocol.
    IP_PROTOCOL_PUP = 12,

    // User Datagram Protocol.
    IP_PROTOCOL_UDP = 17,

    // XNS IDP protocol.
    IP_PROTOCOL_IDP = 22,

    // SO Transport Protocol Class 4.
    IP_PROTOCOL_TP = 29,

    // IPv6 header.
    IP_PROTOCOL_IPV6 = 41,

    // IPv6 routing header.
    IP_PROTOCOL_ROUTING = 43,

    // IPv6 fragmentation header.
    IP_PROTOCOL_FRAGMENT = 44,

    // Reservation Protocol.
    IP_PROTOCOL_RSVP = 46,

    // General Routing Encapsulation.
    IP_PROTOCOL_GRE = 47,

    // Encapsulating security payload.
    IP_PROTOCOL_ESP = 50,

    // Authentication header.
    IP_PROTOCOL_AH = 51,

    // ICMPv6.
    IP_PROTOCOL_ICMPV6 = 58,

    // IPv6 no next header.
    IP_PROTOCOL_NONE = 59,

    // IPv6 destination options.
    IP_PROTOCOL_DSTOPTS = 60,

    // Multicast Transport Protocol.
    IP_PROTOCOL_MTP = 92,

    // Encapsulation Header.
    IP_PROTOCOL_ENCAP = 98,

    // Protocol Independent Multicast.
    IP_PROTOCOL_PIM = 103,

    // Compression Header Protocol.
    IP_PROTOCOL_COMP = 108,

    // Raw IP packets.
    IP_PROTOCOL_RAW = 255,
};

// An IP header. See RFC 791 for more information.
typedef struct
{
    // Header length, in u32:s.
    uint8_t header_length: 4;
    uint8_t version: 4;
    uint8_t type_of_service;
    uint16_t total_length;
    uint16_t id;
    uint16_t fragment_offset;
    uint8_t time_to_live;
    uint8_t protocol;

    // Two-complement additive checksum.
    uint16_t checksum;
    uint32_t source_address;
    uint32_t destination_address;

    // Options start here.
    uint8_t options[0];

    // After the options, the data follows. This element can only be used if no options are used, of course.
    uint8_t data[0];
} __attribute__((packed)) ipv4_header_type;

// IP otions.
// FIXME: Clean up this crap.
typedef struct
{
    // Saved first hop address.
    uint32_t first_hop_address;
    uint8_t optlen;
    uint8_t srr;
    uint8_t rr;
    uint8_t ts;

    // Set by setsockopt?
    uint8_t is_setbyuser: 1;

    // Options in __data, rather than skb.
    uint8_t is_data: 1;

    // Strict source route.
    uint8_t is_strictroute: 1;

    // Packet destination address was our one.
    uint8_t srr_is_hit: 1;

    // IP checksum more not valid.
    uint8_t is_changed: 1;

    // Need to record addr of outgoing device.
    uint8_t rr_needaddr: 1;

    // Need to record timestamp.
    uint8_t ts_needtime: 1;

    // Need to record addr of outgoing device.
    uint8_t ts_needaddr: 1;
    uint8_t router_alert;
    uint8_t __pad1;
    uint8_t __pad2;

    // Data start here.
    uint8_t data[0];
} __attribute__((packed)) ipv4_options_type;

// A generic IP header.
typedef struct
{
    ipv4_header_type header;
    ipv4_options_type options;

    // Data starts here.
    uint8_t data[0];
} __attribute__((packed)) ipv4_packet_type;

// A protocol type definition. This provides the modular core of the IPv4 server.
typedef struct
{
    void (*function)(ipv4_interface_type *interface, ipv4_ethernet_header_type *ethernet_header, int length, mailbox_id_type output_mailbox_id);
    uint8_t type;
} ipv4_protocol_type;

// A node in the IPv4 interface list.
typedef struct
{
    struct ipv4_interface_list_type *next;
    ipv4_interface_type *interface;
    ipc_structure_type *ethernet_structure;
} ipv4_interface_list_type;

// Global variables.
extern log_structure_type log_structure;
extern ipv4_interface_list_type *interface_list;
extern mutex_type interface_list_mutex;
extern uint8_t ethernet_broadcast[];

// Function prototypes.

extern void ipv4_header_create(uint32_t destination, uint32_t source, uint8_t protocol_type, unsigned int length, ipv4_header_type *ipv4_header);
extern void ipv4_ethernet_header_create(void *destination, void *source, uint16_t protocol_type, ipv4_ethernet_header_type *buffer);
extern uint16_t ipv4_checksum(uint16_t *data, unsigned int length);
