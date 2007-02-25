/* $Id$ */
/* Abstract: PCI library. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA. */

#include <memory/memory.h>
#include <pci/pci.h>

/* Initialise a connection to the PCI server matching the 'tag'
   identification. If multiple choices are available (and less than
   ten in figure), the first one is chosen. */
/* FIXME: This semantic is not really ideal. Either always use the
   first one or use none. */

return_type pci_init (ipc_structure_type *pci_structure, tag_type *tag)
{
  mailbox_id_type mailbox_id[10];
  unsigned int services = 10;

  /* Try to resolve the pci service. */

  if (ipc_service_resolve ("pci", mailbox_id, &services, 0, tag) != 
      IPC_RETURN_SUCCESS)
  {
    return PCI_RETURN_SERVICE_UNAVAILABLE;
  }

  pci_structure->output_mailbox_id = mailbox_id[0];

  /* Connect to this service. */

  if (ipc_service_connection_request (pci_structure) != IPC_RETURN_SUCCESS)
  {
    return PCI_RETURN_SERVICE_UNAVAILABLE;
  }

  return PCI_RETURN_SUCCESS;
}

/* Get the number of PCI devices in the system. */

return_type pci_get_number_of_devices (ipc_structure_type *pci_structure,
                                       unsigned int *number_of_devices)
{
  message_parameter_type message_parameter;

  message_parameter.protocol = IPC_PROTOCOL_PCI;
  message_parameter.message_class = IPC_PCI_DEVICE_GET_AMOUNT;
  message_parameter.data = number_of_devices;
  message_parameter.length = 0;

  ipc_send (pci_structure->output_mailbox_id, &message_parameter);
  message_parameter.length = sizeof (unsigned int);
  ipc_receive (pci_structure->input_mailbox_id, &message_parameter, NULL);

  return PCI_RETURN_SUCCESS;
}

/* Get the data for the devices of the given type installed in the
   system. */

return_type pci_device_exists
   (ipc_structure_type *pci_structure, pci_device_probe_type *probe, 
    pci_device_info_type **device_info, unsigned int *number_of_devices)
{
  message_parameter_type message_parameter;
  unsigned int length = 0;

  message_parameter.protocol = IPC_PROTOCOL_PCI;
  message_parameter.message_class = IPC_PCI_DEVICE_EXISTS;
  message_parameter.data = probe;
  message_parameter.length = sizeof (pci_device_probe_type);
  message_parameter.block = TRUE;
  ipc_send (pci_structure->output_mailbox_id, &message_parameter);

  message_parameter.data = *device_info;
  message_parameter.length = length;
  ipc_receive (pci_structure->input_mailbox_id, &message_parameter, 
               &length);

  /* message_parameter.data may have been reallocated, so store its
     new address. */

  *device_info = message_parameter.data;
  *number_of_devices = length / sizeof (pci_device_info_type);

  return PCI_RETURN_SUCCESS;
}

/* Allocate a buffer for use by a PCI device. */

return_type pci_allocate_buffer (void **physical_address, 
                                 void **virtual_address, unsigned int size)
{
  if (system_call_memory_allocate (virtual_address, SIZE_IN_PAGES (size), 
                                   FALSE) != MEMORY_RETURN_SUCCESS)
  {
    return PCI_RETURN_OUT_OF_MEMORY;
  }

  system_call_memory_get_physical_address (*virtual_address, physical_address);
  return PCI_RETURN_SUCCESS;
}
