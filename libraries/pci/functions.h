// Abstract: PCI library function prototypes.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2000 chaos development
// © Copyright 2015 chaos development

#pragma once

#include <ipc/ipc.h>
#include <system/system.h>

extern return_type pci_init(ipc_structure_type *pci_structure, tag_type *tag);
extern return_type pci_get_number_of_devices(ipc_structure_type *pci_structure, unsigned int *number_of_devices);
extern return_type pci_device_exists(ipc_structure_type *pci_structure, pci_device_probe_type *probe,
                                     pci_device_info_type **device_info, unsigned int *number_of_devices);

extern return_type pci_allocate_buffer(void **physical_address, void **virtual_address, unsigned int size);
