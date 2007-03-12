/* Generated automatically by system_calls.pl */

#ifndef __STORM_IA32_WRAPPER_H__
#define __STORM_IA32_WRAPPER_H__

void wrapper_init (void);
void wrapper_kernelfs_entry_read (void);
void wrapper_mailbox_create (void);
void wrapper_mailbox_destroy (void);
void wrapper_mailbox_flush (void);
void wrapper_mailbox_send (void);
void wrapper_mailbox_receive (void);
void wrapper_service_create (void);
void wrapper_service_destroy (void);
void wrapper_service_get (void);
void wrapper_service_protocol_get (void);
void wrapper_service_protocol_get_amount (void);
void wrapper_dma_transfer (void);
void wrapper_dma_transfer_cancel (void);
void wrapper_dma_register (void);
void wrapper_dma_unregister (void);
void wrapper_irq_register (void);
void wrapper_irq_unregister (void);
void wrapper_irq_wait (void);
void wrapper_irq_acknowledge (void);
void wrapper_memory_allocate (void);
void wrapper_memory_deallocate (void);
void wrapper_memory_reserve (void);
void wrapper_memory_get_physical_address (void);
void wrapper_port_range_register (void);
void wrapper_port_range_unregister (void);
void wrapper_process_create (void);
void wrapper_process_name_set (void);
void wrapper_process_parent_unblock (void);
void wrapper_thread_create (void);
void wrapper_thread_control (void);
void wrapper_thread_name_set (void);
void wrapper_timer_read (void);
void wrapper_dispatch_next (void);

#endif /* !__STORM_IA32_WRAPPER_H__ */