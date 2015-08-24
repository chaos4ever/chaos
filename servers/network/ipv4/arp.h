// Abstract: ARP protocol stuff.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2015 chaos development

#pragma once

#include <ipv4/ipv4.h>
#include <system/system.h>

typedef struct
{
    u16 hardware_address_space;
    u16 protocol_address_space;
    u8 hardware_address_length;
    u8 protocol_address_length;
    u16 opcode;
    u8 sender_hardware_address[IPV4_ETHERNET_ADDRESS_LENGTH];
    u32 sender_protocol_address;
    u8 target_hardware_address[IPV4_ETHERNET_ADDRESS_LENGTH];
    u32 target_protocol_address;
} __attribute__((packed)) arp_packet_type;

// An entry in the ARP cache.
typedef struct
{
    struct arp_cache_entry_type *next;

    u8 ethernet_address[IPV4_ETHERNET_ADDRESS_LENGTH];
    u32 ip_address;

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

extern bool arp_ip_to_ethernet_address(u32 ip_address, u8 ethernet_address[]);

extern void arp_insert_entry(u32 ip_address, u8 ethernet_address[]);
extern void arp_who_has(u32 ip_address, ipv4_interface_type *interface, ipc_structure_type *ethernet_structure);

extern unsigned int arp_get_number_of_entries(void);
extern arp_cache_entry_type *arp_get_entry(unsigned int which);
