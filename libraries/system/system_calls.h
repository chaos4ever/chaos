/* $Id$ */
/* Abstract: Routines for doing system calls. */
/* Authors: Henrik Hallin <hal@chaosdev.org>,
            Per Lundberg <plundis@chaosdev.org> */

/* Copyright 1999-2000 chaos development. */

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

#ifndef __LIBRARY_SYSTEM_SYSTEM_CALLS_H__
#define __LIBRARY_SYSTEM_SYSTEM_CALLS_H__

#include <storm/storm.h>

/* Inlines through New York can be dangerous -- so please use a
   helmet! */

static inline return_type system_call_init (void)
{
  return_type return_value;

  asm volatile
    ("lcall %1, $0"
     : "=a" (return_value)
     : "n" (SYSTEM_CALL_INIT << 3));

  return return_value;
}

/* Read the value of the system timer. */

static inline return_type system_call_timer_read (time_type *timer)
{
  return_type return_value;

  asm volatile 
  ("\
    pushl %2
    lcall %3, $0"
   : "=a" (return_value),
     "=m" (*timer)
   : "g" (timer),
     "n" (SYSTEM_CALL_TIMER_READ << 3));

  return return_value;
}

/* Register an IRQ level for use by the process. */

static inline return_type system_call_irq_register (unsigned int irq_number,
                                                    char *description)
{
  return_type return_value;

  asm volatile 
  ("\
    pushl %1
    pushl %2
    lcall %3, $0
   "
   : "=a" (return_value)
   : "g" (description),
     "g" (irq_number),
     "n" (SYSTEM_CALL_IRQ_REGISTER << 3));

  return return_value;
}

/* Unregister an IRQ. */

static inline return_type system_call_irq_unregister (unsigned int irq_number)
{
  return_type return_value;

  asm volatile 
  ("\
    pushl %1
    lcall %2, $0"
   : "=a" (return_value)
   : "g" (irq_number),
     "n" (SYSTEM_CALL_IRQ_UNREGISTER << 3));

  return return_value;
}

/* Wait for an IRQ. */

static inline return_type system_call_irq_wait (unsigned int irq_number)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %1
    lcall %2, $0
   "
   : "=a" (return_value)
   : "g" (irq_number),
     "n" (SYSTEM_CALL_IRQ_WAIT << 3));

  return return_value;
}

/* Acknowledge that an IRQ has been raised. */

static inline return_type system_call_irq_acknowledge (unsigned int irq_number)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %1
    lcall %2, $0
   "
   : "=a" (return_value)
   : "g" (irq_number),
     "n" (SYSTEM_CALL_IRQ_ACKNOWLEDGE << 3));

  return return_value;
}

/* Register a port range (one or more ports) for usage by the
   program. */

static inline return_type system_call_port_range_register
  (u32 start, u32 length, char *description)
{
  return_type return_value;

  asm volatile 
  ("\
    pushl %1
    pushl %2
    pushl %3
    lcall %4, $0
   "
   : "=a" (return_value)
   : "g" (description),
     "g" (length),
     "g" (start),
     "n" (SYSTEM_CALL_PORT_RANGE_REGISTER << 3));

  return return_value;
}  

/* Unregister an I/O port range so that someone else may use it. */

static inline return_type system_call_port_range_unregister (u32 start)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %1
    lcall %2, $0
   "
   : "=a" (return_value)
   : "g" (start),
     "n" (SYSTEM_CALL_PORT_RANGE_UNREGISTER << 3));

  return return_value;
}

/* Allocate memory. */
/* FIXME: Replace the page concept with a block or chunk concept and
   add a kernelfs entry for reading the architecture's block or chunk
   size. */

static inline return_type system_call_memory_allocate (void **address,
                                                       u32 pages,
                                                       bool cacheable)
{
  return_type return_value;

  asm volatile 
  ("\
    pushl %2
    pushl %3
    pushl %4
    lcall %5, $0
   "
   : "=a" (return_value),
     "=g" (*address)
   : "g" (cacheable),
     "g" (pages),
     "g" (address),
     "n" (SYSTEM_CALL_MEMORY_ALLOCATE << 3));

  return return_value;
}

/* Deallocate memory. */

static inline return_type system_call_memory_deallocate (void **address)
{
  return_type return_value;

  asm volatile 
  ("\
    pushl %2
    lcall %3, $0
   "
   : "=a" (return_value),
     "=g" (*address)
   : "g" (address),
     "n" (SYSTEM_CALL_MEMORY_DEALLOCATE << 3));

  return return_value;
}

/* Get the physical address for the given virtual pointer */

static inline return_type system_call_memory_get_physical_address
  (void *virtual_address, void **physical_address)
{
  return_type return_value;

  asm volatile 
  ("\
    pushl %2
    pushl %3
    lcall %4, $0
   "
   : "=a" (return_value),
     "=g" (*physical_address)
   : "g" (physical_address),
     "g" (virtual_address),
     "n" (SYSTEM_CALL_MEMORY_GET_PHYSICAL_ADDRESS << 3));

  return return_value;
}

/* Reserve a memory region. */

static inline return_type system_call_memory_reserve 
  (address_type start, unsigned int size, void **virtual_address)
{
  return_type return_value;

  asm volatile 
  ("\
    pushl %2
    pushl %3
    pushl %4
    lcall %5, $0
   "
   : "=a" (return_value),
     "=g" (*virtual_address)
   : "g" (virtual_address),
     "g" (size),
     "g" (start),
     "n" (SYSTEM_CALL_MEMORY_RESERVE << 3));

  return return_value;
}

/* Change something about the current thread. */

static inline return_type system_call_thread_control
  (thread_id_type thread_id, unsigned int control_class,
   unsigned int parameter)
{
  return_type return_value;

  asm volatile 
  ("\
    pushl %1
    pushl %2
    pushl %3
    lcall %4, $0
   "
   : "=a" (return_value)
   : "g" (parameter),
     "g" (control_class),
     "g" (thread_id),
     "n" (SYSTEM_CALL_THREAD_CONTROL << 3));

  return return_value;
}

/* fork() on steroids. ;-) */

static inline return_type system_call_thread_create (void)
{
  return_type return_value;

  asm volatile 
  ("\
    lcall %1, $0
   "
   : "=a" (return_value)
   : "n" (SYSTEM_CALL_THREAD_CREATE << 3));

  return return_value;
}

/* Unblock a parent. */

static inline return_type system_call_process_parent_unblock (void)
{
  return_type return_value;

  asm volatile 
  ("\
    lcall %1, $0
   "
   : "=a" (return_value)
   : "n" (SYSTEM_CALL_PROCESS_PARENT_UNBLOCK << 3));

  return return_value;
}

/* Set the name of the current process. */

static inline return_type system_call_process_name_set (const char *name)
{
  return_type return_value;

  asm volatile 
  ("\
    pushl %1
    lcall %2, $0
   "
   : "=a" (return_value)
   : "g" (name),
     "n" (SYSTEM_CALL_PROCESS_NAME_SET << 3));

  return return_value;
}

/* Set the name of the current thread. */

static inline return_type system_call_thread_name_set (const char *name)
{
  return_type return_value;

  asm volatile 
  ("\
    pushl %1
    lcall %2, $0
   "
   : "=a" (return_value)
   : "g" (name),
     "n" (SYSTEM_CALL_THREAD_NAME_SET << 3));

  return return_value;
}

/* Read an entry from the kernel "file system". */

static inline return_type system_call_kernelfs_entry_read (void *buffer)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %2
    lcall %3, $0
   "
   : "=a" (return_value),
     "=g" (*(int *) buffer)
   : "g" (buffer),
     "n" (SYSTEM_CALL_KERNELFS_ENTRY_READ << 3));

  return return_value;
}

/* Dispatch the next task. */

static inline return_type system_call_dispatch_next (void)
{
  return_type return_value;

  asm volatile ("lcall %1, $0"
                : "=&a" (return_value)
                : "n" (SYSTEM_CALL_DISPATCH_NEXT << 3));
  return return_value;
}

/* Create a new mailbox. */

static inline return_type system_call_mailbox_create 
  (mailbox_id_type *mailbox_id, unsigned int size,
   process_id_type user_process_id, cluster_id_type user_cluster_id,
   thread_id_type user_thread_id)
{
  return_type return_value;

  asm volatile
  ("\
     pushl %2
     pushl %3
     pushl %4
     pushl %5
     pushl %6
     lcall %7, $0
   "
   : "=a" (return_value),
     "=g" (*mailbox_id)
   : "g" (user_thread_id),
     "g" (user_cluster_id),
     "g" (user_process_id),
     "g" (size),
     "g" (mailbox_id),
     "n" (SYSTEM_CALL_MAILBOX_CREATE << 3));

  return return_value;
}

/* Destroy the given mailbox ID. */

static inline return_type system_call_mailbox_destroy
  (mailbox_id_type mailbox_id)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %1
    lcall %2, $0
   "
   : "=a" (return_value)
   : "g" (mailbox_id),
     "n" (SYSTEM_CALL_MAILBOX_DESTROY << 3));

  return return_value;
}

/* Send a message to the given mailbox. */

static inline return_type system_call_mailbox_send
  (mailbox_id_type mailbox_id, message_parameter_type *message_parameter)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %2
    pushl %3
    lcall %4, $0
   "
   : "=a" (return_value),
     "=g" (*message_parameter)
   : "g" (message_parameter),
     "g" (mailbox_id),
     "n" (SYSTEM_CALL_MAILBOX_SEND << 3));

  return return_value;
}

/* Receive a message from the given mailbox. */

static inline return_type system_call_mailbox_receive
  (mailbox_id_type mailbox_id,  message_parameter_type *message_parameter)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %2
    pushl %3
    lcall %4, $0
   "
   : "=a" (return_value),
     "=g" (*message_parameter)
   : "g" (message_parameter),
     "g" (mailbox_id),
     "n" (SYSTEM_CALL_MAILBOX_RECEIVE << 3));

  return return_value;
}

/* Create a new service. */

static inline return_type system_call_service_create
  (const char *protocol_name, mailbox_id_type *mailbox_id,
   tag_type *identification)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %2
    pushl %3
    pushl %4
    lcall %5, $0
   "
   : "=a" (return_value),
     "=g" (*mailbox_id)
   : "g" (identification),
     "g" (mailbox_id),
     "g" (protocol_name),
     "n" (SYSTEM_CALL_SERVICE_CREATE << 3));

  return return_value;
}

/* Get all the services for the given protocol. */

static inline return_type system_call_service_get
  (const char *protocol_name, service_parameter_type *service_parameter,
   tag_type *identification)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %2
    pushl %3
    pushl %4
    lcall %5, $0
   "
   : "=a" (return_value),
     "=g" (*service_parameter)
   : "g" (identification),
     "g" (service_parameter),
     "g" (protocol_name),
     "n" (SYSTEM_CALL_SERVICE_GET << 3));

  return return_value;
}

/* Get the number of protocols supported by the system. */

static inline return_type system_call_service_protocol_get_amount
  (unsigned int *number_of_protocols)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %2
    lcall %3, $0
   "
   : "=a" (return_value),
     "=g" (number_of_protocols)
   : "g" (number_of_protocols),
     "n" (SYSTEM_CALL_SERVICE_PROTOCOL_GET_AMOUNT << 3));

  return return_value;
}

/* Get a list of the protocols supported by the system. */

static return_type system_call_service_protocol_get
  (unsigned int *maximum_protocols, service_protocol_type *protocol_info)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %3
    pushl %4
    lcall %5, $0
   "
   : "=a" (return_value),
     "=g" (*protocol_info),
     "=g" (*maximum_protocols)
   : "g" (protocol_info),
     "g" (maximum_protocols),
     "n" (SYSTEM_CALL_SERVICE_PROTOCOL_GET << 3));

  return return_value;
}

/* Fork ( ;) ) off a new process. */

static inline return_type system_call_process_create (process_create_type *process_data)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %1
    lcall %2, $0
   "
   : "=a" (return_value)
   : "g" (process_data),
     "n" (SYSTEM_CALL_PROCESS_CREATE << 3));

  return return_value;
}

/* Register a DMA channel for usage. */

static inline return_type system_call_dma_register 
  (unsigned int channel, void **dma_buffer)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %2
    pushl %3
    lcall %4, $0
   "
   : "=a" (return_value),
     "=g" (*dma_buffer)
   : "g" (dma_buffer),
     "g" (channel),
     "n" (SYSTEM_CALL_DMA_REGISTER << 3));

  return return_value;
}

/* Unregister a DMA channel. */

static inline return_type system_call_dma_unregister 
  (unsigned int channel)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %1
    lcall %2, $0
   "
   : "=a" (return_value)
   : "g" (channel),
     "n" (SYSTEM_CALL_DMA_UNREGISTER << 3));

  return return_value;
}

/* Perform a DMA transfer on the given channel with the given
   parameters. */

static inline return_type system_call_dma_transfer
  (unsigned int channel, unsigned int buffer_size, unsigned int operation,
   unsigned int transfer_mode, unsigned int autoinit)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %1
    pushl %2
    pushl %3
    pushl %4
    pushl %5
    lcall %6, $0
   "
   : "=a" (return_value)
   : "g" (autoinit),
     "g" (transfer_mode),
     "g" (operation),
     "g" (buffer_size),
     "g" (channel),
     "n" (SYSTEM_CALL_DMA_TRANSFER << 3));

  return return_value;
}

/* Cancel a DMA transfer. */

static inline return_type dma_transfer_cancel (unsigned int channel)
{
  return_type return_value;

  asm volatile
  ("\
    pushl %1
    lcall %2, $0
   "
   : "=a" (return_value)
   : "g" (channel),
     "n" (SYSTEM_CALL_DMA_TRANSFER_CANCEL << 3));

  return return_value;
}

#endif /* !__LIBRARY_SYSTEM_SYSTEM_CALLS_H__ */
