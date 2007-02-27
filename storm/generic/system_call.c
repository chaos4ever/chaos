/* $Id$ */
/* Abstract: System call wrapper functions. */
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

/* This file contains a wrapper for each system call function, which
   is responsible for locking the right mutex. Also, it serves as an
   abstraction layer for the architecture dependent parts of the
   kernel. */

#include <storm/current-arch/dma.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/init.h>
#include <storm/generic/memory.h>
#include <storm/generic/process.h>
#include <storm/generic/port.h>
#include <storm/generic/return_values.h>
#include <storm/generic/string.h>
#include <storm/generic/system_call.h>
#include <storm/generic/thread.h>
#include <storm/generic/time.h>

return_type system_call_dispatch_next (void)
{
  dispatch_next ();
  return STORM_RETURN_SUCCESS;
}

return_type system_call_thread_name_set (char *name)
{
  memory_set_u8 (current_tss->thread_name, 0, MAX_THREAD_NAME_LENGTH);
  string_copy_max (current_tss->thread_name, name,
                   MAX_THREAD_NAME_LENGTH - 1);
  return STORM_RETURN_SUCCESS;
}

return_type system_call_thread_create (void)
{
  return thread_create ();
}

return_type system_call_thread_control 
  (thread_id_type thread_id, unsigned int class, unsigned int parameter)
{
  return thread_control (thread_id, class, parameter);
}

/* Returns the uptime in milliseconds. Its precision depends on the
   task switch rate, which is different on different architectures. */

return_type system_call_timer_read (time_type *timer)
{
  *timer = (timeslice * 1000) / hz;
  return STORM_RETURN_SUCCESS;
}

return_type system_call_process_name_set (char *name)
{
  return process_name_set (name);
}

return_type system_call_process_parent_unblock (void)
{
  return process_parent_unblock ();
}

return_type system_call_memory_reserve
  (address_type start, unsigned int size, void **virtual_address)
{
  u32 virtual_page_number;

  mutex_kernel_wait (&memory_mutex);
  memory_virtual_allocate (&virtual_page_number, SIZE_IN_PAGES (size));
  memory_virtual_map (virtual_page_number,
                      GET_PAGE_NUMBER (start), SIZE_IN_PAGES (size),
                      PAGE_WRITABLE | PAGE_NON_PRIVILEGED | 
                      PAGE_CACHE_DISABLE);
  mutex_kernel_signal (&memory_mutex);
  *virtual_address = (void *) (virtual_page_number * SIZE_PAGE);
  return STORM_RETURN_SUCCESS;
}

return_type system_call_port_range_register
  (unsigned int start, unsigned int ports, char *description)
{
  return port_range_register (start, ports, description);
}

return_type system_call_port_range_unregister (unsigned int start)
{
  return port_range_unregister (start);
}

return_type system_call_memory_allocate 
  (void **address, u32 pages, bool cacheable)
{
  return memory_allocate (address, pages, cacheable);
}

return_type system_call_memory_deallocate (void **address)
{
  return memory_deallocate (address);
}

return_type system_call_irq_wait (unsigned int irq_number)
{
  return irq_wait (irq_number);
}

return_type system_call_irq_acknowledge (unsigned int irq_number)
{
  return irq_acknowledge (irq_number);
}

return_type system_call_irq_register
  (unsigned int irq_number, char *description)
{
  return irq_register (irq_number, description);
}

return_type system_call_process_create (process_create_type *process_data)
{
  return process_create (process_data);
}

return_type system_call_init (void)
{
  return init ();
}

return_type system_call_memory_get_physical_address 
  (void *virtual_address, void **physical_address)
{
  return memory_get_physical_address (virtual_address, physical_address);
}

return_type system_call_dma_register
  (unsigned int channel, void **dma_buffer)
{
  return dma_register (channel, dma_buffer);
}

extern return_type system_call_dma_unregister (unsigned int channel)
{
  return dma_unregister (channel);
}

return_type system_call_dma_transfer 
  (unsigned int channel, unsigned int buffer_size, unsigned int operation,
   unsigned int transfer_mode, unsigned int autoinit)
{
  return dma_transfer (channel, buffer_size, operation, transfer_mode,
                       autoinit);
}

return_type system_call_dma_transfer_cancel (unsigned int channel)
{
  return dma_transfer_cancel (channel);
}

return_type system_call_kernelfs_entry_read
  (kernelfs_generic_type *kernelfs_generic)
{
  return kernelfs_entry_read (kernelfs_generic);
}

return_type system_call_mailbox_create
  (mailbox_id_type *mailbox_id, unsigned int size,
   process_id_type user_process_id, cluster_id_type user_cluster_id,
   thread_id_type user_thread_id)
{
  return mailbox_create (mailbox_id, size, user_process_id, user_cluster_id,
                         user_thread_id);
}

extern return_type system_call_mailbox_destroy
  (mailbox_id_type mailbox_id)
{
  return mailbox_destroy (mailbox_id);
}

extern return_type system_call_mailbox_flush (mailbox_id_type mailbox_id)
{
  return mailbox_flush (mailbox_id);
}

extern return_type system_call_mailbox_send
  (mailbox_id_type mailbox_id, message_parameter_type *message_parameter)
{
  return mailbox_send (mailbox_id, message_parameter);
}

extern return_type system_call_mailbox_receive
  (mailbox_id_type mailbox_id, message_parameter_type *message_parameter)
{
  return mailbox_receive (mailbox_id, message_parameter);
}

extern return_type system_call_service_create
  (const char *protocol_name,  mailbox_id_type *mailbox_id,
   tag_type *identification)
{
  return service_create (protocol_name, mailbox_id, identification);
}

return_type system_call_service_destroy (mailbox_id_type mailbox_id)
{
  return service_destroy (mailbox_id);
}

extern return_type system_call_service_get 
  (const char *protocol_name, service_parameter_type *service_parameter,
   tag_type *identification_mask)
{
  return service_get (protocol_name, service_parameter, identification_mask);
}

extern return_type system_call_service_protocol_get_amount 
  (unsigned int *number_of_protocols)
{
  return service_protocol_get_amount (number_of_protocols);
}

extern return_type system_call_service_protocol_get
  (unsigned int *maximum_protocols, service_protocol_type *protocol_info)
{
  return service_protocol_get (maximum_protocols, protocol_info);
}

extern return_type system_call_irq_unregister (unsigned int irq_number)
{
  return irq_unregister (irq_number);
}
