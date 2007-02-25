/* $Id$ */
/* Abstract: PCI library function prototypes. */
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

#ifndef __LIBRARY_PCI_FUNCTIONS_H__
#define __LIBRARY_PCI_FUNCTIONS_H__

#include <ipc/ipc.h>
#include <system/system.h>

/*
  functions from via-rhine.c. Which should be implemented, and how?

  pci_dma_supported
  pci_resource_len
  pci_resource_start
  pci_enable_device
  pci_set_master
  request_mem_region
*/


extern return_type pci_init
  (ipc_structure_type *pci_structure, tag_type *tag);

extern return_type pci_get_number_of_devices
  (ipc_structure_type *pci_structure, unsigned int *number_of_devices);

extern return_type pci_device_exists
   (ipc_structure_type *pci_structure, pci_device_probe_type *probe, 
    pci_device_info_type **device_info, unsigned int *number_of_devices);

extern return_type pci_allocate_buffer 
  (void **physical_address, void **virtual_address, unsigned int size);

#endif /* !__LIBRARY_PCI_FUNCTIONS_H__ */
