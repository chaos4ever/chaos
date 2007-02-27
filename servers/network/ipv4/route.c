/* $Id$ */
/* Abstract: Handle routing of packets. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

/* This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#include "ipv4.h"
#include "route.h"

/* Find the way to route packets to this address. */

bool route_find (u32 address, ipv4_interface_type **interface,
                 bool *direct, ipc_structure_type **ethernet_structure)
{
  ipv4_interface_list_type *entry;

  mutex_wait (interface_list_mutex);
  entry = interface_list;

  while (entry != NULL)
  {
    /* Check if this address is on this interface's network. */

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
    mutex_signal (interface_list_mutex);
    return FALSE;
  }

  *interface = entry->interface;
  *ethernet_structure = entry->ethernet_structure;
  mutex_signal (interface_list_mutex);

  *direct = FALSE;
  return TRUE;
}
