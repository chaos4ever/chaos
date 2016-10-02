// Abstract: Packet list.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

typedef struct
{
    struct packet_list *next;
    struct packet_list *previous;

    unsigned int length;
    void *data;

    // The source port and address of this packet, since they may be just anything for UDP packets.
    ipv4_address_type source_address;
    uint16_t source_port;
} packet_list_type;
