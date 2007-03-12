/* $Id$ */
/* Abstract: System call functions. */
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

#ifndef __STORM_GENERIC_SYSTEM_CALL_H__
#define __STORM_GENERIC_SYSTEM_CALL_H__

#include <storm/generic/kernelfs.h>
#include <storm/generic/mailbox.h>
#include <storm/generic/service.h>
#include <storm/generic/tag.h>
#include <storm/generic/types.h>

extern return_type system_call_dispatch_next (void);
extern return_type system_call_thread_name_set (char *name);
extern return_type system_call_thread_create (void);

extern return_type system_call_thread_control 
  (thread_id_type thread_id, unsigned int class, unsigned int parameter);

extern return_type system_call_timer_read (time_type *timer);
extern return_type system_call_process_name_set (char *name);
extern return_type system_call_process_parent_unblock (void);

extern return_type system_call_memory_reserve
  (address_type start, unsigned int size, void **virtual_address);

extern return_type system_call_port_range_register
  (unsigned int start, unsigned int ports, char *description);

extern return_type system_call_port_range_unregister (unsigned int start);

extern return_type system_call_memory_allocate 
  (void **address, u32 pages, bool cacheable);

extern return_type system_call_memory_deallocate (void **address);

extern return_type system_call_irq_wait (unsigned int irq_number);
extern return_type system_call_irq_acknowledge (unsigned int irq_number);

extern return_type system_call_irq_register
  (unsigned int irq_number, char *description);

extern return_type system_call_process_create
  (process_create_type *process_data);

extern return_type system_call_init (void);

extern return_type system_call_memory_get_physical_address 
  (void *virtual_address, void **physical_address);

extern return_type system_call_dma_register
  (unsigned int channel, void **dma_buffer);

extern return_type system_call_dma_unregister (unsigned int channel);

extern return_type system_call_dma_transfer 
  (unsigned int channel, unsigned int buffer_size, unsigned int operation,
   unsigned int transfer_mode, unsigned int autoinit);

extern return_type system_call_dma_transfer_cancel (unsigned int channel);

extern return_type system_call_kernelfs_entry_read
  (kernelfs_generic_type *kernelfs_generic);

return_type system_call_mailbox_create
  (mailbox_id_type *mailbox_id, unsigned int size,
   process_id_type user_process_id, cluster_id_type user_cluster_id,
   thread_id_type user_thread_id);

extern return_type system_call_mailbox_destroy
  (mailbox_id_type mailbox_id);

extern return_type system_call_mailbox_flush (mailbox_id_type mailbox_id);

extern return_type system_call_mailbox_send
  (mailbox_id_type mailbox_id, message_parameter_type *message_parameter);

extern return_type system_call_mailbox_receive
  (mailbox_id_type mailbox_id, message_parameter_type *message_parameter);

extern return_type system_call_service_create
  (const char *protocol_name,  mailbox_id_type *mailbox_id,
   tag_type *identification);

extern return_type system_call_service_destroy (mailbox_id_type mailbox_id);

extern return_type system_call_service_get 
  (const char *protocol_name, service_parameter_type *service_parameter,
   tag_type *identification_mask);

extern return_type system_call_service_protocol_get_amount 
  (unsigned int *number_of_protocols);

extern return_type system_call_service_protocol_get
  (unsigned int *maximum_protocols, service_protocol_type *protocol_info);

extern return_type system_call_irq_unregister (unsigned int irq_number);

#endif /* !__STORM_GENERIC_SYSTEM_CALL_H__ */
