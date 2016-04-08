// Abstract: Packet forwarding code.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development
// © Copyright 2015-2016 chaos development

#include <memory/memory.h>

#include "arp.h"
#include "forward.h"
#include "ipv4.h"
#include "route.h"

// Set this to FALSE if you don't want packet forwarding turned on.
bool forwarding = TRUE;

void forward_packet(ipv4_ethernet_header_type *ethernet_header, unsigned int length)
{
    ipv4_header_type *ipv4_header = (ipv4_header_type *) &ethernet_header->data;
    ipv4_interface_type *interface;
    bool direct;
    u8 ethernet_address[IPV4_ETHERNET_ADDRESS_LENGTH];
    ipc_structure_type *ethernet_structure;
    message_parameter_type message_parameter;

    if (!forwarding)
    {
        log_print(&log_structure, LOG_URGENCY_INFORMATIVE, "Someone tried to route a packet through me, but I'm not acting under the role of a router");
        return;
    }

    // Find the interface to use for this packet.
    if (!route_find(ipv4_header->destination_address, &interface, &direct, &ethernet_structure))
    {
        char address[16];

        // We found no way to handle this packet.
        ipv4_address_to_string(address, ipv4_header->destination_address);
        log_print_formatted(&log_structure, LOG_URGENCY_INFORMATIVE,
                            "Couldn't route packet to %s: No applicable route found. Dropping.\n",
                            address);
        return;
    }

    // Now, do the routing. If the 'direct' flag is set, the target is located on this subnet. Otherwise, its router is.
    char from_address[16];
    char to_address[16];

    ipv4_address_to_string(from_address, ipv4_header->source_address);
    ipv4_address_to_string(to_address, ipv4_header->destination_address);
    log_print_formatted(&log_structure, LOG_URGENCY_DEBUG,
                        "Routing packet: %s -> %s, via %s",
                        from_address, to_address,
                        interface->identification);

    while (!arp_ip_to_ethernet_address(ipv4_header->destination_address, ethernet_address))
    {
        log_print_formatted(&log_structure, LOG_URGENCY_DEBUG,
                            "Sending ARP who-has for %lX on %s.",
                            ipv4_header->destination_address,
                            interface->identification);
        arp_who_has(ipv4_header->destination_address, interface, ethernet_structure);
        system_sleep(500);
    }

    // Send the packet.
    memory_copy(ethernet_header->destination_address, ethernet_address, IPV4_ETHERNET_ADDRESS_LENGTH);
    memory_copy(ethernet_header->source_address, interface->hardware_address, IPV4_ETHERNET_ADDRESS_LENGTH);
    message_parameter.data = ethernet_header;
    message_parameter.message_class = IPC_ETHERNET_PACKET_SEND;
    message_parameter.protocol = IPC_PROTOCOL_ETHERNET;
    message_parameter.length = length;
    ipc_send(ethernet_structure->output_mailbox_id, &message_parameter);
}
