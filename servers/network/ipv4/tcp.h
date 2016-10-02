// Abstract: TCP prototypes.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

// See RFC 793 for more information about what these are for.
typedef struct
{
    // The source port number.
    uint16_t source_port;

    // The destination port number.
    uint16_t destination_port;

    // The sequence number of the first data octet.
    uint32_t sequence_number;

    // If ack is set, contains the value of the next sequence number.
    uint32_t acknowledgement_number;
    uint16_t reserved : 4;

    // The number of 32 bit words in the TCP header.
    uint16_t data_offset : 4;

    // No more data from sender.
    uint16_t finish : 1;

    // Synchronise sequence numbers.
    uint16_t synchronise : 1;

    // Reset the connection.
    uint16_t reset : 1;

    // Push function.
    uint16_t push : 1;

    // Acknowledgement field significant.
    uint16_t acknowledge : 1;

    // Urgent pointer field signicant.
    uint16_t urgent : 1;
    uint16_t reserved2 : 2;

    // The number of data octets the sender is willing to accept.
    uint16_t window;

    // Checksum of header, data and the 'pseudo header (see RFC for more information).
    uint16_t checksum;

    // Points to the sequence number of the octet following the urgent data.
    uint16_t urgent_pointer;

    // End of header; start of options.
} __attribute__((packed)) tcp_header_type;

// Pseudo header used for calculating checksums.
typedef struct
{
    uint32_t source_address;
    uint32_t destination_address;
    uint8_t protocol_type;

    // Must be zero!
    uint8_t zero;
    uint16_t tcp_length;
} __attribute__((packed)) tcp_pseudo_header_type;

// Function prototypes.
extern void tcp_packet_receive(ipv4_interface_type *interface,
                               ipv4_ethernet_header_type *old_ethernet_header,
                               int length __attribute__((unused)),
                               mailbox_id_type output_mailbox_id);
