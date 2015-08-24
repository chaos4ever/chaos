// Abstract: Socket handling.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2000 chaos development
// © Copyright 2015 chaos development

#pragma once

#include "packet.h"

// Defines/enums.
#define SOCKET_HASH_SIZE        1024

enum
{
    SOCKET_RETURN_SUCCESS,
    SOCKET_RETURN_INVALID_ARGUMENT
};

// Type definitions.
typedef struct
{
    struct socket_type *next;
    struct socket_type *previous;

    unsigned int protocol_type;
    unsigned int source_port;
    unsigned int destination_port;

    // FIXME: We should have a source address too, so that different applications can listen to different interfaces.
    ipv4_address_type destination_address;

    unsigned int socket_id;

    // A list of packet waiting on this socket.
    packet_list_type *packet_list;
} socket_type;

// Function prototypes.
extern void socket_init(void);
extern ipv4_socket_id_type socket_get_free(void);
extern socket_type *socket_find(ipv4_socket_id_type socket_id);
extern void socket_insert(socket_type *socket);
extern void socket_delete(socket_type *socket);
extern ipv4_socket_id_type socket_get_hash(ipv4_socket_id_type socket_id);
extern return_type socket_send(ipv4_socket_id_type socket_id, unsigned int length, void *data);
extern void socket_receive(ipv4_socket_id_type socket_id, mailbox_id_type mailbox_id);
