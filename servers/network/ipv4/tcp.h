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
    u16 source_port;

    // The destination port number.
    u16 destination_port;

    // The sequence number of the first data octet.
    u32 sequence_number;

    // If ack is set, contains the value of the next sequence number.
    u32 acknowledgement_number;
    u16 reserved : 4;

    // The number of 32 bit words in the TCP header.
    u16 data_offset : 4;

    // No more data from sender.
    u16 finish : 1;

    // Synchronise sequence numbers.
    u16 synchronise : 1;

    // Reset the connection.
    u16 reset : 1;

    // Push function.
    u16 push : 1;

    // Acknowledgement field significant.
    u16 acknowledge : 1;

    // Urgent pointer field signicant.
    u16 urgent : 1;
    u16 reserved2 : 2;

    // The number of data octets the sender is willing to accept.
    u16 window;

    // Checksum of header, data and the 'pseudo header (see RFC for more information).
    u16 checksum;

    // Points to the sequence number of the octet following the urgent data.
    u16 urgent_pointer;

    // End of header; start of options.
} __attribute__((packed)) tcp_header_type;

// Pseudo header used for calculating checksums.
typedef struct
{
    u32 source_address;
    u32 destination_address;
    u8 protocol_type;

    // Must be zero!
    u8 zero;
    u16 tcp_length;
} __attribute__((packed)) tcp_pseudo_header_type;

// Function prototypes.
extern void tcp_packet_receive(ipv4_interface_type *interface,
                               ipv4_ethernet_header_type *old_ethernet_header,
                               int length __attribute__((unused)),
                               mailbox_id_type output_mailbox_id);
