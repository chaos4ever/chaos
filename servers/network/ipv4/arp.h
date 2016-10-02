// Abstract: ARP protocol stuff.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

#include <ipv4/ipv4.h>
#include <system/system.h>

typedef struct
{
    uint16_t hardware_address_space;
    uint16_t protocol_address_space;
    uint8_t hardware_address_length;
    uint8_t protocol_address_length;
    uint16_t opcode;
    uint8_t sender_hardware_address[IPV4_ETHERNET_ADDRESS_LENGTH];
    uint32_t sender_protocol_address;
    uint8_t target_hardware_address[IPV4_ETHERNET_ADDRESS_LENGTH];
    uint32_t target_protocol_address;
} __attribute__((packed)) arp_packet_type;

// An entry in the ARP cache.
typedef struct
{
    struct arp_cache_entry_type *next;

    uint8_t ethernet_address[IPV4_ETHERNET_ADDRESS_LENGTH];
    uint32_t ip_address;

    // The time when this entry was inserted; will be used to remove old entries. (not implemented yet)
    time_type time;
} arp_cache_entry_type;

enum
{
    ARP_REQUEST = 1,
    ARP_REPLY = 2,
};

// Function prototypes.
extern void arp_packet_receive(ipv4_interface_type *interface, ipv4_ethernet_header_type *ethernet_header,
                               int length __attribute__((unused)), mailbox_id_type output_mailbox_id);

extern bool arp_ip_to_ethernet_address(uint32_t ip_address, uint8_t ethernet_address[]);

extern void arp_insert_entry(uint32_t ip_address, uint8_t ethernet_address[]);
extern void arp_who_has(uint32_t ip_address, ipv4_interface_type *interface, ipc_structure_type *ethernet_structure);

extern unsigned int arp_get_number_of_entries(void);
extern arp_cache_entry_type *arp_get_entry(unsigned int which);
