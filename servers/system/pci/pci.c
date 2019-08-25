// Abstract: PCI server. (It should also support AGP sometime, but currently, it hardly does PCI right...)
// Author: Per Lundberg <per@chaosdev.io>. Heavily inspired by the Linux source, but not by far as obnoxious...
//
// © Copyright 1999 chaos development

#include <ipc/ipc.h>
#include <log/log.h>
#include <string/string.h>
#include <system/system.h>

#include "config.h"
#include "pci.h"

static log_structure_type log_structure;

// Linked list of all PCI devices.
static pci_device_type *pci_device_list = NULL;
static pci_device_type **pci_device_list_pointer = &pci_device_list;

// Linked list of all buses.
static pci_bus_type *root_bus = NULL;

tag_type empty_tag =
{
    0, 0, ""
};

// Functions for accessing PCI configuration space with type 1 accesses.
#define CONFIG_COMMAND(device, where) \
   (0x80000000 | (device->bus->number << 16) | \
    (device->device_function << 8) | \
    (where & ~3))

static uint8_t pci_type1_read_config_uint8_t(pci_device_type *device, int where)
{
    system_port_out_u32(PCI_BASE, CONFIG_COMMAND(device, where));
    return system_port_in_uint8_t(PCI_DATA + (where & 3));
}

static uint16_t pci_type1_read_config_uint16_t(pci_device_type *device, int where)
{
    system_port_out_uint16_t(PCI_BASE, CONFIG_COMMAND(device, where));
    return system_port_in_uint16_t(PCI_DATA + (where & 2));
}

static uint32_t pci_type1_read_config_u32(pci_device_type *device, int where)
{
    system_port_out_u32(PCI_BASE, CONFIG_COMMAND(device, where));
    return system_port_in_u32(PCI_DATA);
}

static void pci_type1_write_config_uint8_t(pci_device_type *device, int where,
                                      uint8_t value)
{
    system_port_out_u32(PCI_BASE, CONFIG_COMMAND(device, where));
    system_port_out_uint8_t(PCI_DATA + (where & 3), value);
}

static void pci_type1_write_config_uint16_t(pci_device_type *device, int where,
                                       uint16_t value)
{
    system_port_out_u32(PCI_BASE, CONFIG_COMMAND(device, where));
    system_port_out_uint16_t(PCI_DATA + (where & 2), value);
}

static void pci_type1_write_config_u32(pci_device_type *device, int where,
                                       uint32_t value)
{
    system_port_out_u32(PCI_BASE, CONFIG_COMMAND(device, where));
    system_port_out_u32(PCI_DATA, value);
}

static pci_operation_type pci_type1_operation =
{
    pci_type1_read_config_uint8_t,
    pci_type1_read_config_uint16_t,
    pci_type1_read_config_u32,
    pci_type1_write_config_uint8_t,
    pci_type1_write_config_uint16_t,
    pci_type1_write_config_u32
};

// Functions for accessing PCI configuration space with type 2 accesses.
#define IOADDR(device_function, where) ((0xC000 | ((device_function & 0x78) << 5)) + where)

#define FUNC(device_function) (((device_function & 7) << 1) | 0xF0)

#define SET(device) \
    system_port_out_uint8_t (PCI_BASE, FUNC (device->device_function)); \
    system_port_out_uint8_t (PCI_BASE + 2, device->bus->number);

static uint8_t pci_type2_read_config_uint8_t(pci_device_type *device, int where)
{
    uint8_t return_value;

    SET(device);
    return_value = system_port_in_uint8_t(IOADDR(device->device_function, where));
    system_port_out_uint8_t(PCI_BASE, 0);
    return return_value;
}

static uint16_t pci_type2_read_config_uint16_t(pci_device_type *device, int where)
{
    uint16_t return_value;

    SET(device);
    return_value = system_port_in_uint16_t(IOADDR(device->device_function, where));
    system_port_out_uint8_t(PCI_BASE, 0);
    return return_value;
}

static uint32_t pci_type2_read_config_u32(pci_device_type *device, int where)
{
    uint16_t return_value;

    SET(device);
    return_value = system_port_in_uint16_t(IOADDR(device->device_function, where));
    system_port_out_uint8_t(PCI_BASE, 0);
    return return_value;
}

static void pci_type2_write_config_uint8_t(pci_device_type *device, int where, uint8_t value)
{
    SET(device);
    system_port_out_uint16_t(IOADDR(device->device_function, where), value);
    system_port_out_uint8_t(PCI_BASE, 0);
}

static void pci_type2_write_config_uint16_t(pci_device_type *device, int where, uint16_t value)
{
    SET(device);
    system_port_out_uint16_t(IOADDR(device->device_function, where), value);
    system_port_out_uint8_t(PCI_BASE, 0);
}

static void pci_type2_write_config_u32(pci_device_type *device, int where, uint32_t value)
{
    SET(device);
    system_port_out_u32(IOADDR(device->device_function, where), value);
    system_port_out_uint8_t(PCI_BASE, 0);
}

static pci_operation_type pci_type2_operation =
{
    pci_type2_read_config_uint8_t,
    pci_type2_read_config_uint16_t,
    pci_type2_read_config_u32,
    pci_type2_write_config_uint8_t,
    pci_type2_write_config_uint16_t,
    pci_type2_write_config_u32
};

extern pci_device_id_type pci_device_id[];
extern pci_vendor_id_type pci_vendor_id[];

// The operations we should use to access the PCI host.
static pci_operation_type *pci_operation = NULL;

// Meta-functions for reading and writing PCI data.
static uint32_t pci_read_config_uint32_t(pci_device_type *device, int where)
{
    return ((pci_operation_type *) device->bus->operation)->read_uint32_t(device, where);
}

static uint16_t pci_read_config_uint16_t(pci_device_type *device, int where)
{
    return ((pci_operation_type *) device->bus->operation)->read_uint16_t(device, where);
}

static uint8_t pci_read_config_uint8_t(pci_device_type *device, int where)
{
    return ((pci_operation_type *) device->bus->operation)->read_uint8_t(device, where);
}

static void pci_write_config_uint32_t(pci_device_type *device, int where, uint32_t data)
{
    ((pci_operation_type *) device->bus->operation)->write_uint32_t(device, where, data);
}

// Handle an IPC connection request.
static void handle_connection(mailbox_id_type *reply_mailbox_id)
{
    system_thread_name_set("Handling connection");

    message_parameter_type message_parameter;
    ipc_structure_type ipc_structure;
    bool done = FALSE;
    uint32_t *data;
    uint32_t **data_pointer = &data;
    unsigned int data_size = 1024;

    memory_allocate((void **) data_pointer, data_size);

    // Accept the connection.
    ipc_structure.output_mailbox_id = *reply_mailbox_id;
    ipc_connection_establish(&ipc_structure);

    message_parameter.block = TRUE;

    while (!done)
    {
        message_parameter.data = data;
        message_parameter.protocol = IPC_PROTOCOL_PCI;
        message_parameter.message_class = IPC_CLASS_NONE;
        message_parameter.length = data_size;

        if (ipc_receive(ipc_structure.input_mailbox_id, &message_parameter, &data_size) != IPC_RETURN_SUCCESS)
        {
            continue;
        }

        switch (message_parameter.message_class)
        {
            // Get the resource information for the device matching the input vendor and device ID.
            case IPC_PCI_DEVICE_EXISTS:
            {
                pci_device_probe_type *probe = (pci_device_probe_type *) data;
                unsigned int devices = 0;
                pci_device_type *device = pci_device_list;
                pci_device_info_type *device_info = NULL;
                pci_device_info_type **device_info_pointer = &device_info;
                unsigned int counter = 0;

                while (device != NULL)
                {
                    if (probe->vendor_id == device->vendor_id &&
                        probe->device_id == device->device_id)
                    {
                        devices++;
                    }

                    device = (pci_device_type *) device->next;
                }

                // Allocate memory to hold this many devices.
                memory_allocate((void **) device_info_pointer, devices * sizeof(pci_device_info_type));

                device = pci_device_list;

                while (device != NULL && counter < devices)
                {
                    if (probe->vendor_id == device->vendor_id &&
                        probe->device_id == device->device_id)
                    {
                        memory_copy(&device_info[counter].resource, device->resource,
                                    sizeof(pci_resource_type) * PCI_NUMBER_OF_RESOURCES);
                        counter++;
                    }

                    device_info->irq = device->irq;
                    device_info->vendor_id = device->vendor_id;
                    device_info->device_id = device->device_id;

                    device = (pci_device_type *) device->next;
                }

                message_parameter.data = device_info;
                message_parameter.length = devices * sizeof(pci_device_info_type);
                ipc_send(ipc_structure.output_mailbox_id, &message_parameter);
                memory_deallocate((void **) device_info_pointer);

                break;
            }
        }
    }
}

// Detect the presence of a PCI host, and if found, return a pointer to the functions that should be used when accessing it.
// Otherwise, return NULL.
static pci_operation_type *pci_detect(void)
{
    pci_operation_type *operation = NULL;

    system_port_out_uint8_t(PCI_BASE, 0);
    system_port_out_uint8_t(PCI_BASE + 2, 0);

    if ((system_port_in_uint8_t(PCI_BASE) == 0) && (system_port_in_uint8_t(PCI_BASE + 2) == 0))
    {
        operation = &pci_type2_operation;
    }
    else
    {
        uint32_t tmp = system_port_in_u32(PCI_BASE);
        system_port_out_u32(PCI_BASE, 0x80000000);

        if (system_port_in_u32(PCI_BASE) == 0x80000000)
        {
            operation = &pci_type1_operation;
        }

        system_port_out_u32(PCI_BASE, tmp);
    }

    return operation;
}

// Read interrupt line.
static void pci_read_irq(pci_device_type *device)
{
    unsigned int irq;

    irq = pci_read_config_uint8_t(device, PCI_INTERRUPT_PIN);

    if (irq != 0)
    {
        irq = pci_read_config_uint8_t(device, PCI_INTERRUPT_LINE);
    }

    device->irq = irq;
}

// Translate the low bits of the PCI base to the resource type.
static unsigned int pci_get_resource_type(unsigned int flags)
{
    if ((flags & PCI_BASE_ADDRESS_SPACE_IO) != 0)
    {
        return PCI_RESOURCE_IO;
    }

    if ((flags & PCI_BASE_ADDRESS_MEM_PREFETCH) != 0)
    {
        return PCI_RESOURCE_MEMORY | PCI_RESOURCE_PREFETCH;
    }

    return PCI_RESOURCE_MEMORY;
}

// Read PCI base addresses.
static void pci_read_bases(pci_device_type *device, unsigned int amount, int rom)
{
    unsigned int position, register_number, next;

    // FIXME: Find a better name for the 'l' variable.
    uint32_t l, size;
    pci_resource_type *resource;

    for (position = 0; position < amount; position = next)
    {
        next = position + 1;
        resource = &device->resource[position];
        resource->name = device->name;
        register_number = PCI_BASE_ADDRESS_0 + (position << 2);

        l = pci_read_config_uint32_t(device, register_number);
        pci_write_config_uint32_t(device, register_number, UINT32_MAX);
        size = pci_read_config_uint32_t(device, register_number);
        pci_write_config_uint32_t(device, register_number, l);

        if (size == 0 || size == 0xFFFFFFFF)
        {
            continue;
        }

        if (l == 0xFFFFFFFF)
        {
            l = 0;
        }

        if ((l & PCI_BASE_ADDRESS_SPACE) == PCI_BASE_ADDRESS_SPACE_MEMORY)
        {
            resource->start = l & PCI_BASE_ADDRESS_MEM_MASK;
            size = ~(size & PCI_BASE_ADDRESS_MEM_MASK);
        }
        else
        {
            resource->start = l & PCI_BASE_ADDRESS_IO_MASK;
            size = ~(size & PCI_BASE_ADDRESS_IO_MASK) & 0xFFFF;
        }

        resource->end = resource->start + (unsigned long) size;
        resource->flags |= (l & 0xF) | pci_get_resource_type(l);

        if ((l & (PCI_BASE_ADDRESS_SPACE | PCI_BASE_ADDRESS_MEM_TYPE_MASK)) ==
                (PCI_BASE_ADDRESS_SPACE_MEMORY | PCI_BASE_ADDRESS_MEM_TYPE_64))
        {
            l = pci_read_config_uint32_t(device, register_number + 4);
            next++;

            if (l != 0)
            {
                log_print_formatted(&log_structure, LOG_URGENCY_WARNING,
                                    "Unable to handle 64-bit address for device %s",
                                    device->slot_name);
                resource->start = 0;
                resource->flags = 0;
                continue;
            }
        }
    }

    if (rom != 0)
    {
        device->rom_base_reg = rom;
        resource = &device->resource[PCI_ROM_RESOURCE];
        l = pci_read_config_uint32_t(device, rom);
        pci_write_config_uint32_t(device, rom, ~PCI_ROM_ADDRESS_ENABLE);
        size = pci_read_config_uint32_t(device, rom);
        pci_write_config_uint32_t(device, rom, l);

        if (l == 0xFFFFFFFF)
        {
            l = 0;
        }

        if (size != 0 && size != 0xFFFFFFFF)
        {
            resource->flags = ((l & PCI_ROM_ADDRESS_ENABLE) |
                               PCI_RESOURCE_MEMORY | PCI_RESOURCE_PREFETCH |
                               PCI_RESOURCE_READONLY | PCI_RESOURCE_CACHEABLE);
            resource->start = l & PCI_ROM_ADDRESS_MASK;
            size = ~(size & PCI_ROM_ADDRESS_MASK);
            resource->end = resource->start + (unsigned long) size;
        }

        resource->name = device->name;
    }
}

// Fill in class and map information of a device.
static bool pci_setup_device(pci_device_type *device)
{
    uint32_t class;

    // SMBus: Intel Corporation Sunrise Point-LP SMBus
    // Our (borrowed-from-Linux-ca-1999) PCI probing code causes the machine to reboot on this
    // device. This is a stupid workaround for now; since we don't support SMBus this is not
    // a big deal but it's still rather silly... Issue about fixing this permanently:
    // https://github.com/chaos4ever/chaos/issues/134
    if (device->vendor_id == 0x8086 && device->device_id == 0x9d23)
    {
        return FALSE;
    }

    // Set the name.
    string_print(device->slot_name, "%02x:%02x.%d", device->bus->number,
                 PCI_SLOT(device->device_function),
                 PCI_FUNC(device->device_function));
    string_print(device->name, "PCI device %04x:%04x",
                 device->vendor_id, device->device_id);

    // Read the 3-byte class. (?)
    class = pci_read_config_uint32_t(device, PCI_CLASS_REVISION);
    class >>= 8;
    device->class = class;
    class >>= 8;

    // Handle erroneous cases first. (In Linux, they use evil gotos for this...)
    if ((device->header_type == PCI_HEADER_TYPE_BRIDGE && class != PCI_CLASS_BRIDGE_PCI) ||
            (device->header_type == PCI_HEADER_TYPE_CARDBUS && class != PCI_CLASS_BRIDGE_CARDBUS))
    {
        log_print_formatted(&log_structure, LOG_URGENCY_WARNING,
                            "%s: class %x doesn't match header type %02x. Ignoring class.",
                            device->slot_name, class, device->header_type);
        device->class = PCI_CLASS_NOT_DEFINED;
        return TRUE;
    }

    switch (device->header_type)
    {
        // Standard header.
        case PCI_HEADER_TYPE_NORMAL:
        {
            pci_read_irq(device);
            pci_read_bases(device, 6, PCI_ROM_ADDRESS);
            device->subsystem_vendor_id = pci_read_config_uint16_t(device, PCI_SUBSYSTEM_VENDOR_ID);
            device->subsystem_device_id = pci_read_config_uint16_t(device, PCI_SUBSYSTEM_ID);
            break;
        }

        // Bridge header.
        case PCI_HEADER_TYPE_BRIDGE:
        {
            pci_read_bases(device, 2, PCI_ROM_ADDRESS1);
            break;
        }

        // CardBus bridge header.
        case PCI_HEADER_TYPE_CARDBUS:
        {
            pci_read_irq(device);
            pci_read_bases(device, 1, 0);
            device->subsystem_vendor_id = pci_read_config_uint16_t(device, PCI_CARDBUS_SUBSYSTEM_VENDOR_ID);
            device->subsystem_device_id = pci_read_config_uint16_t(device, PCI_CARDBUS_SUBSYSTEM_ID);
            break;
        }

        // Unknown header.
        default:
        {
            log_print_formatted(&log_structure, LOG_URGENCY_WARNING, "Device %s has unknown header type %02x, ignoring.",
                                device->slot_name, device->header_type);
            return FALSE;
        }
    }

    // We found a fine healthy device, go go go...
    return TRUE;
}

// Read the config data for a PCI device, sanity-check it and fill in the device structure...
static pci_device_type *pci_scan_device(pci_device_type *input_device)
{
    pci_device_type *device;
    pci_device_type **device_pointer = &device;
    uint32_t vendor_id;

    vendor_id = pci_read_config_uint32_t(input_device, PCI_VENDOR_ID);

    // Some broken boards return 0 or ~0 if a slot is empty.
    if (vendor_id == 0xFFFFFFFF || vendor_id == 0x00000000 || vendor_id == 0x0000FFFF || vendor_id == 0xFFFF0000)
    {
        return NULL;
    }

    memory_allocate((void **) device_pointer, sizeof(pci_device_type));

    if (device == NULL)
    {
        return NULL;
    }

    memory_copy(device, input_device, sizeof(pci_device_type));

    device->vendor_id = vendor_id & 0xFFFF;
    device->device_id = (vendor_id >> 16) & 0xFFFF;

    // Assume 32-bit PCI; let 64-bit PCI cards (which are far rarer) set this higher, assuming the system even supports it.
    if (!pci_setup_device(device))
    {
        memory_deallocate((void **) device_pointer);
    }

    return device;
}

// Scan the given PCI slot.
static pci_device_type *pci_scan_slot(pci_device_type *input_device)
{
    pci_device_type *device;
    pci_device_type *first_device = NULL;
    int function = 0;
    bool is_multi = FALSE;
    uint8_t header_type;

    for (function = 0; function < 8; function++, input_device->device_function++)
    {
        if (function != 0 && !is_multi)
        {
            continue;
        }

        header_type = pci_read_config_uint8_t(input_device, PCI_HEADER_TYPE);
        input_device->header_type = header_type & 0x7F;
        device = pci_scan_device(input_device);

        if (device == NULL)
        {
            continue;
        }

        // FIXME: Maybe set the device name here?
        if (function == 0)
        {
            is_multi = (header_type & 0x80) == 0x80;
            first_device = device;
        }

        // Add this device to the linked list.
        list_node_insert((list_type **) pci_device_list_pointer, (list_type *) device);
    }

    return first_device;
}

// Scan the given bus.
static pci_bus_type *pci_scan_bus(int bus_number, pci_operation_type *operation)
{
    pci_bus_type *bus;
    pci_bus_type **bus_pointer = &bus;
    unsigned int device_function;
    pci_device_type device;

    memory_allocate((void **) bus_pointer, sizeof(pci_bus_type));
    bus->number = bus->secondary = bus_number;
    bus->operation = (struct pci_operation_type *) operation;

    device.bus = bus;

    // Find all the devices on this bus.
    for (device_function = 0; device_function < 0x100; device_function += 8)
    {
        device.device_function = device_function;
        pci_scan_slot(&device);
    }

    return bus;
}

// Initialisation code.
static bool init(void)
{
    // Set our names.
    system_process_name_set(PACKAGE_NAME);
    system_thread_name_set("Initialising");

    log_init(&log_structure, PACKAGE_NAME, &empty_tag);

    // Register the ports we need.
    system_call_port_range_register(PCI_BASE, PCI_PORTS, "PCI controller");

    pci_operation = pci_detect();

    if (pci_operation == NULL)
    {
        return FALSE;
    }

    // Scan this bus.
    root_bus = pci_scan_bus(0, pci_operation);

    return TRUE;
}

// Main function.
int main(void)
{
    ipc_structure_type ipc_structure;
    pci_device_type *device;

    // Initialise the PCI support.
    if (!init())
    {
        log_print(&log_structure, LOG_URGENCY_EMERGENCY, "No compatible PCI host found.");
        return -1;
    }

    device = pci_device_list;

    while (device != NULL)
    {
        log_print_formatted(&log_structure, LOG_URGENCY_DEBUG, "%s IRQ %u", device->name, device->irq);
        device = (pci_device_type *) device->next;
    }

    // Create the service.
    if (ipc_service_create("pci", &ipc_structure, &empty_tag) != IPC_RETURN_SUCCESS)
    {
        log_print(&log_structure, LOG_URGENCY_EMERGENCY, "Couldn't create pci service.");
        return -1;
    }

    system_thread_name_set("Service handler");

    system_call_process_parent_unblock();

    // Main loop. Wait for messages and answer if someone asks us an appropriate question.
    while (TRUE)
    {
        mailbox_id_type reply_mailbox_id;

        ipc_service_connection_wait(&ipc_structure);
        reply_mailbox_id = ipc_structure.output_mailbox_id;

        system_thread_create((thread_entry_point_type *) handle_connection, &reply_mailbox_id);
    }
}
