// Abstract: Handle routing of packets.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development
// © Copyright 2015-2016 chaos development

#include "ipv4.h"
#include "route.h"

// Find the way to route packets to this address.
bool route_find(u32 address, ipv4_interface_type **interface, bool *direct, ipc_structure_type **ethernet_structure)
{
    ipv4_interface_list_type *entry;

    mutex_wait(interface_list_mutex);
    entry = interface_list;

    while (entry != NULL)
    {
        // Check if this address is on this interface's network.
        if (entry->interface->up &&
                (entry->interface->ip_address & entry->interface->netmask) ==
                (address & entry->interface->netmask))
        {
            break;
        }

        entry = (ipv4_interface_list_type *) entry->next;
    }

    if (entry == NULL)
    {
        mutex_signal(&interface_list_mutex);
        return FALSE;
    }

    *interface = entry->interface;
    *ethernet_structure = entry->ethernet_structure;
    mutex_signal(&interface_list_mutex);

    *direct = FALSE;
    return TRUE;
}
