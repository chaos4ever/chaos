// Abstract: Protocol for communicating with the PCI server.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#pragma once

#include <ipc/protocols.h>

enum
{
    // Used for functions wishing to return a return value.
    IPC_PCI_RETURN_VALUE  = (IPC_PROTOCOL_PCI << 16),

    // Check if a device with the given vendor and device ID exists.
    IPC_PCI_DEVICE_EXISTS,

    // Get the amount of PCI devices in this system.
    IPC_PCI_DEVICE_GET_AMOUNT,

    // Get brief information about all devices in the system.
    IPC_PCI_DEVICE_GET_INFO,

    // Get state for a given device.
    IPC_PCI_DEVICE_GET_STATE,

    // Set PCI configuration state.
    IPC_PCI_DEVICE_SET_STATE,
};

// Return values.
enum
{
    // One or more devices of the specified type exists in the system.
    IPC_PCI_RETURN_DEVICE_EXISTS,

    // No device of the given type could be found.
    IPC_PCI_RETURN_DEVICE_ABSENT,
};

// Miscellaneous defines.
#define PCI_NUMBER_OF_RESOURCES         12
#define PCI_NUMBER_OF_REGULAR_RESOURCES 6

// IO resources have these defined flags.
// Bus-specific bits.
#define PCI_RESOURCE_BITS               0x000000FF

// Resource type.
#define PCI_RESOURCE_IO                 0x00000100
#define PCI_RESOURCE_MEMORY             0x00000200
#define PCI_RESOURCE_IRQ                0x00000400
#define PCI_RESOURCE_DMA                0x00000800

// No side effects.
#define PCI_RESOURCE_PREFETCH           0x00001000
#define PCI_RESOURCE_READONLY           0x00002000
#define PCI_RESOURCE_CACHEABLE          0x00004000
#define PCI_RESOURCE_RANGELENGTH        0x00008000
#define PCI_RESOURCE_SHADOWABLE         0x00010000

#define PCI_RESOURCE_UNSET              0x20000000
#define PCI_RESOURCE_AUTO               0x40000000

// Driver has marked this resource busy.
#define PCI_RESOURCE_BUSY               0x80000000

// This structure is passed into IPC_PCI_DEVICE_EXISTS.
typedef struct
{
    uint16_t vendor_id;
    uint16_t device_id;
} pci_device_probe_type;

// Resources are tree-like, allowing nesting etc..
typedef struct
{
    const char *name;
    unsigned long start, end;
    unsigned long flags;
} pci_resource_type;

typedef struct
{
    unsigned int irq;
    pci_resource_type resource[PCI_NUMBER_OF_RESOURCES];
} pci_device_info_type;
