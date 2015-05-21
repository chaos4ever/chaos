// Abstract: Routines for doing system calls.
// Authors: Henrik Hallin <hal@chaosdev.org>,
//          Per Lundberg <per@halleluja.nu>
//
// © Copyright 1999-2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015 chaos development

#pragma once

#include <storm/storm.h>

// TODO: Should consider automatically creating this file in system_calls.rb. It feels stupid to have to maintain this manually.
// The difficult part is the asm constraints though, can be a bit tricky...

// Inlines through New York can be dangerous -- so please use a helmet!
static inline return_type system_call_init(void)
{
    return_type return_value;

    asm volatile("lcall %1, $0"
                 : "=a" (return_value)
                 : "n" (SYSTEM_CALL_INIT << 3));

    return return_value;
}

static inline return_type system_call_timer_read(time_type *timer)
{
    return_type return_value;

    asm volatile("pushl %2\n"
                 "lcall %3, $0"
                 : "=a" (return_value),
                   "=m" (*timer)
                 : "ri" (timer),
                   "n" (SYSTEM_CALL_TIMER_READ << 3));

    return return_value;
}

// Register an IRQ level for use by the process.
static inline return_type system_call_irq_register(unsigned int irq_number, char *description)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "pushl %2\n"
                 "lcall %3, $0"
                 : "=a" (return_value)
                 : "ri" (description),
                   "ri" (irq_number),
                   "n" (SYSTEM_CALL_IRQ_REGISTER << 3));

    return return_value;
}

// Unregister an IRQ.
static inline return_type system_call_irq_unregister(unsigned int irq_number)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "lcall %2, $0"
                 : "=a" (return_value)
                 : "ri" (irq_number),
                   "n" (SYSTEM_CALL_IRQ_UNREGISTER << 3));

    return return_value;
}

// Wait for an IRQ.
static inline return_type system_call_irq_wait(unsigned int irq_number)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "lcall %2, $0"
                 : "=a" (return_value)
                 : "ri" (irq_number),
                   "n" (SYSTEM_CALL_IRQ_WAIT << 3));

    return return_value;
}

// Acknowledge that an IRQ has been raised.
static inline return_type system_call_irq_acknowledge(unsigned int irq_number)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "lcall %2, $0"
                 : "=a" (return_value)
                 : "ri" (irq_number),
                   "n" (SYSTEM_CALL_IRQ_ACKNOWLEDGE << 3));

    return return_value;
}

// Register a port range (one or more ports) for usage by the process.
static inline return_type system_call_port_range_register(u32 start, u32 length, const char *description)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "pushl %2\n"
                 "pushl %3\n"
                 "lcall %4, $0"
                 : "=a" (return_value)
                 : "ri" (description),
                   "ri" (length),
                   "ri" (start),
                   "n" (SYSTEM_CALL_PORT_RANGE_REGISTER << 3));

    return return_value;
}

// Unregister an I/O port range so that someone else may use it.
static inline return_type system_call_port_range_unregister(u32 start)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "lcall %2, $0"
                 : "=a" (return_value)
                 : "ri" (start),
                   "n" (SYSTEM_CALL_PORT_RANGE_UNREGISTER << 3));

    return return_value;
}

// Allocate memory.
// FIXME: Replace the page concept with a block or chunk concept and add a kernelfs entry for reading the architecture's block
// or chunk size.
static inline return_type system_call_memory_allocate(void **address, u32 pages, bool cacheable)
{
    return_type return_value;

    asm volatile("pushl %2\n"
                 "pushl %3\n"
                 "pushl %4\n"
                 "lcall %5, $0"
                 : "=a" (return_value),
                   "=m" (*address)
                 : "ri" (cacheable),
                   "ri" (pages),
                   "ri" (address),
                   "n" (SYSTEM_CALL_MEMORY_ALLOCATE << 3));

    return return_value;
}

static inline return_type system_call_memory_deallocate(void **address)
{
    return_type return_value;

    asm volatile("pushl %2\n"
                 "lcall %3, $0"
                 : "=a" (return_value),
                   "=m" (*address)
                 : "ri" (address),
                   "n" (SYSTEM_CALL_MEMORY_DEALLOCATE << 3));

    return return_value;
}

// Get the physical address for the given virtual pointer
static inline return_type system_call_memory_get_physical_address(void *virtual_address, void **physical_address)
{
    return_type return_value;

    asm volatile("pushl %2\n"
                 "pushl %3\n"
                 "lcall %4, $0"
                 : "=a" (return_value),
                   "=m" (*physical_address)
                 : "ri" (physical_address),
                   "ri" (virtual_address),
                   "n" (SYSTEM_CALL_MEMORY_GET_PHYSICAL_ADDRESS << 3));

    return return_value;
}

// Reserve a memory region.
static inline return_type system_call_memory_reserve(address_type start, unsigned int size, void **virtual_address)
{
    return_type return_value;

    asm volatile("pushl %2\n"
                 "pushl %3\n"
                 "pushl %4\n"
                 "lcall %5, $0"
                 : "=a" (return_value),
                   "=m" (*virtual_address)
                 : "ri" (virtual_address),
                   "ri" (size),
                   "ri" (start),
                   "n" (SYSTEM_CALL_MEMORY_RESERVE << 3));

    return return_value;
}

// Change something about the current thread.
static inline return_type system_call_thread_control(thread_id_type thread_id, unsigned int control_class, unsigned int parameter)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "pushl %2\n"
                 "pushl %3\n"
                 "lcall %4, $0"
                 : "=a" (return_value)
                 : "ri" (parameter),
                   "ri" (control_class),
                   "ri" (thread_id),
                   "n" (SYSTEM_CALL_THREAD_CONTROL << 3));

    return return_value;
}

// fork() on steroids. ;-)
static inline return_type system_call_thread_create(thread_entry_point_type *thread_entry_point, void *argument)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "pushl %2\n"
                 "lcall %3, $0"
                 : "=a" (return_value)
                 : "ri" (argument),
                   "ri" (thread_entry_point),
                   "n" (SYSTEM_CALL_THREAD_CREATE << 3));

    return return_value;
}

// Unblock a parent.
static inline return_type system_call_process_parent_unblock(void)
{
    return_type return_value;

    asm volatile("lcall %1, $0"
                 : "=a" (return_value)
                 : "n" (SYSTEM_CALL_PROCESS_PARENT_UNBLOCK << 3));

    return return_value;
}

// Set the name of the current process.
static inline return_type system_call_process_name_set(const char *name)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "lcall %2, $0"
                 : "=a" (return_value)
                 : "ri" (name),
                   "n" (SYSTEM_CALL_PROCESS_NAME_SET << 3));

    return return_value;
}

// Set the name of the current thread.
static inline return_type system_call_thread_name_set(const char *name)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "lcall %2, $0"
                 : "=a" (return_value)
                 : "ri" (name),
                   "n" (SYSTEM_CALL_THREAD_NAME_SET << 3));

    return return_value;
}

// Read an entry from the kernel "file system".
static inline return_type system_call_kernelfs_entry_read(void *buffer)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "lcall %2, $0"
                 : "=a" (return_value)
                 : "ri" (buffer),
                   "n" (SYSTEM_CALL_KERNELFS_ENTRY_READ << 3)
                 : "memory");

    return return_value;
}

// Dispatch the next task.
static inline return_type system_call_dispatch_next(void)
{
    return_type return_value;

    asm volatile ("lcall %1, $0"
                  : "=&a" (return_value)
                  : "n" (SYSTEM_CALL_DISPATCH_NEXT << 3));

    return return_value;
}

// Create a new mailbox.
static inline return_type system_call_mailbox_create(mailbox_id_type *mailbox_id, unsigned int size,
    process_id_type user_process_id, cluster_id_type user_cluster_id, thread_id_type user_thread_id)
{
    return_type return_value;

    asm volatile("pushl %2\n"
                 "pushl %3\n"
                 "pushl %4\n"
                 "pushl %5\n"
                 "pushl %6\n"
                 "lcall %7, $0"
                 : "=a" (return_value),
                   "=m" (mailbox_id)
                 : "ri" (user_thread_id),
                   "ri" (user_cluster_id),
                   "ri" (user_process_id),
                   "ri" (size),
                   "ri" (mailbox_id),
                   "n" (SYSTEM_CALL_MAILBOX_CREATE << 3));

    return return_value;
}

// Destroy the given mailbox ID.
static inline return_type system_call_mailbox_destroy(mailbox_id_type mailbox_id)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "lcall %2, $0"
                 : "=a" (return_value)
                 : "ri" (mailbox_id),
                   "n" (SYSTEM_CALL_MAILBOX_DESTROY << 3));

    return return_value;
}

// Send a message to the given mailbox.
static inline return_type system_call_mailbox_send(mailbox_id_type mailbox_id, message_parameter_type *message_parameter)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "pushl %2\n"
                 "lcall %3, $0"
                 : "=a" (return_value)
                 : "ri" (message_parameter),
                   "ri" (mailbox_id),
                   "n" (SYSTEM_CALL_MAILBOX_SEND << 3)
                 : "memory");

    return return_value;
}

// Receive a message from the given mailbox.
static inline return_type system_call_mailbox_receive(mailbox_id_type mailbox_id,  message_parameter_type *message_parameter)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "pushl %2\n"
                 "lcall %3, $0"
                 : "=a" (return_value)
                 : "ri" (message_parameter),
                   "ri" (mailbox_id),
                   "n" (SYSTEM_CALL_MAILBOX_RECEIVE << 3)
                 : "memory");

    return return_value;
}

// Create a new service.
static inline return_type system_call_service_create(const char *protocol_name, mailbox_id_type *mailbox_id, tag_type *identification)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "pushl %2\n"
                 "pushl %3\n"
                 "lcall %4, $0"
                 : "=a" (return_value)
                 : "ri" (identification),
                   "ri" (mailbox_id),
                   "ri" (protocol_name),
                   "n" (SYSTEM_CALL_SERVICE_CREATE << 3)
                 : "memory");

    return return_value;
}

// Get all the services for the given protocol.
static inline return_type system_call_service_get(const char *protocol_name, service_parameter_type *service_parameter,
    tag_type *identification)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "pushl %2\n"
                 "pushl %3\n"
                 "lcall %4, $0"
                 : "=a" (return_value)
                 : "ri" (identification),
                   "ri" (service_parameter),
                   "ri" (protocol_name),
                   "n" (SYSTEM_CALL_SERVICE_GET << 3)
                 : "memory");

    return return_value;
}

// Get the number of protocols supported by the system.
static inline return_type system_call_service_protocol_get_amount(unsigned int *number_of_protocols)
{
    return_type return_value;

    asm volatile("pushl %2\n"
                 "lcall %3, $0"
                 : "=a" (return_value),
                   "=ri" (number_of_protocols)
                 : "ri" (number_of_protocols),
                   "n" (SYSTEM_CALL_SERVICE_PROTOCOL_GET_AMOUNT << 3));

    return return_value;
}

// Get a list of the protocols supported by the system.
static inline return_type system_call_service_protocol_get(unsigned int *maximum_protocols, service_protocol_type *protocol_info)
{
    return_type return_value;

    asm volatile("pushl %3\n"
                 "pushl %4\n"
                 "lcall %5, $0"
                 : "=a" (return_value),
                   "=ri" (*protocol_info),
                   "=ri" (*maximum_protocols)
                 : "ri" (protocol_info),
                   "ri" (maximum_protocols),
                   "n" (SYSTEM_CALL_SERVICE_PROTOCOL_GET << 3));

    return return_value;
}

// Fork ( ;) ) off a new process.
static inline return_type system_call_process_create(process_create_type *process_data)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "lcall %2, $0"
                 : "=a" (return_value)
                 : "ri" (process_data),
                   "n" (SYSTEM_CALL_PROCESS_CREATE << 3));

    return return_value;
}

// Register a DMA channel for usage.
static inline return_type system_call_dma_register(unsigned int channel, void **dma_buffer)
{
    return_type return_value;

    asm volatile("pushl %2\n"
                 "pushl %3\n"
                 "lcall %4, $0"
                 : "=a" (return_value),
                   "=ri" (*dma_buffer)
                 : "ri" (dma_buffer),
                   "ri" (channel),
                   "n" (SYSTEM_CALL_DMA_REGISTER << 3));

    return return_value;
}

// Unregister a DMA channel.
static inline return_type system_call_dma_unregister(unsigned int channel)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "lcall %2, $0"
                 : "=a" (return_value)
                 : "ri" (channel),
                   "n" (SYSTEM_CALL_DMA_UNREGISTER << 3));

    return return_value;
}

// Perform a DMA transfer on the given channel with the given parameters.
static inline return_type system_call_dma_transfer(unsigned int channel, unsigned int buffer_size, unsigned int operation,
    unsigned int transfer_mode, unsigned int autoinit)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "pushl %2\n"
                 "pushl %3\n"
                 "pushl %4\n"
                 "pushl %5\n"
                 "lcall %6, $0"
                 : "=a" (return_value)
                 : "ri" (autoinit),
                   "ri" (transfer_mode),
                   "ri" (operation),
                   "ri" (buffer_size),
                   "ri" (channel),
                   "n" (SYSTEM_CALL_DMA_TRANSFER << 3));

    return return_value;
}

// Cancel a DMA transfer.
static inline return_type dma_transfer_cancel(unsigned int channel)
{
    return_type return_value;

    asm volatile("pushl %1\n"
                 "lcall %2, $0"
                 : "=a" (return_value)
                 : "ri" (channel),
                   "n" (SYSTEM_CALL_DMA_TRANSFER_CANCEL << 3));

    return return_value;
}

