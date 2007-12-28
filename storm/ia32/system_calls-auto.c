/* Generated automatically by system_calls.pl */

#include <storm/ia32/system_calls.h>
#include <storm/ia32/wrapper.h>

const system_call_type system_call[] =
{
  { SYSTEM_CALL_INIT, wrapper_init, 0 },
  { SYSTEM_CALL_KERNELFS_ENTRY_READ, wrapper_kernelfs_entry_read, 1 },
  { SYSTEM_CALL_MAILBOX_CREATE, wrapper_mailbox_create, 5 },
  { SYSTEM_CALL_MAILBOX_DESTROY, wrapper_mailbox_destroy, 1 },
  { SYSTEM_CALL_MAILBOX_FLUSH, wrapper_mailbox_flush, 1 },
  { SYSTEM_CALL_MAILBOX_SEND, wrapper_mailbox_send, 2 },
  { SYSTEM_CALL_MAILBOX_RECEIVE, wrapper_mailbox_receive, 2 },
  { SYSTEM_CALL_SERVICE_CREATE, wrapper_service_create, 3 },
  { SYSTEM_CALL_SERVICE_DESTROY, wrapper_service_destroy, 1 },
  { SYSTEM_CALL_SERVICE_GET, wrapper_service_get, 3 },
  { SYSTEM_CALL_SERVICE_PROTOCOL_GET, wrapper_service_protocol_get, 2 },
  { SYSTEM_CALL_SERVICE_PROTOCOL_GET_AMOUNT, wrapper_service_protocol_get_amount, 1 },
  { SYSTEM_CALL_DMA_TRANSFER, wrapper_dma_transfer, 5 },
  { SYSTEM_CALL_DMA_TRANSFER_CANCEL, wrapper_dma_transfer_cancel, 1 },
  { SYSTEM_CALL_DMA_REGISTER, wrapper_dma_register, 2 },
  { SYSTEM_CALL_DMA_UNREGISTER, wrapper_dma_unregister, 1 },
  { SYSTEM_CALL_IRQ_REGISTER, wrapper_irq_register, 2 },
  { SYSTEM_CALL_IRQ_UNREGISTER, wrapper_irq_unregister, 1 },
  { SYSTEM_CALL_IRQ_WAIT, wrapper_irq_wait, 1 },
  { SYSTEM_CALL_IRQ_ACKNOWLEDGE, wrapper_irq_acknowledge, 1 },
  { SYSTEM_CALL_MEMORY_ALLOCATE, wrapper_memory_allocate, 3 },
  { SYSTEM_CALL_MEMORY_DEALLOCATE, wrapper_memory_deallocate, 1 },
  { SYSTEM_CALL_MEMORY_RESERVE, wrapper_memory_reserve, 3 },
  { SYSTEM_CALL_MEMORY_GET_PHYSICAL_ADDRESS, wrapper_memory_get_physical_address, 2 },
  { SYSTEM_CALL_PORT_RANGE_REGISTER, wrapper_port_range_register, 3 },
  { SYSTEM_CALL_PORT_RANGE_UNREGISTER, wrapper_port_range_unregister, 1 },
  { SYSTEM_CALL_PROCESS_CREATE, wrapper_process_create, 1 },
  { SYSTEM_CALL_PROCESS_NAME_SET, wrapper_process_name_set, 1 },
  { SYSTEM_CALL_PROCESS_PARENT_UNBLOCK, wrapper_process_parent_unblock, 0 },
  { SYSTEM_CALL_THREAD_CREATE, wrapper_thread_create, 0 },
  { SYSTEM_CALL_THREAD_CONTROL, wrapper_thread_control, 3 },
  { SYSTEM_CALL_THREAD_NAME_SET, wrapper_thread_name_set, 1 },
  { SYSTEM_CALL_TIMER_READ, wrapper_timer_read, 1 },
  { SYSTEM_CALL_DISPATCH_NEXT, wrapper_dispatch_next, 0 },
};
