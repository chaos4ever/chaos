/* Generated automatically by system_calls.pl. Don't change. */

#include <storm/ia32/defines.h>
#include <storm/ia32/wrapper.h>

void wrapper_init (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */


                "call	system_call_init\n"

                "addl	$4 * 0, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 0\n");
}

void wrapper_kernelfs_entry_read (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_kernelfs_entry_read\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_mailbox_create (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 5 * 4(%esp)\n"
                "pushl  32 + 4 + 5 * 4(%esp)\n"
                "pushl  32 + 4 + 5 * 4(%esp)\n"
                "pushl  32 + 4 + 5 * 4(%esp)\n"
                "pushl  32 + 4 + 5 * 4(%esp)\n"

                "call	system_call_mailbox_create\n"

                "addl	$4 * 5, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 5\n");
}

void wrapper_mailbox_destroy (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_mailbox_destroy\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_mailbox_flush (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_mailbox_flush\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_mailbox_send (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 2 * 4(%esp)\n"
                "pushl  32 + 4 + 2 * 4(%esp)\n"

                "call	system_call_mailbox_send\n"

                "addl	$4 * 2, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 2\n");
}

void wrapper_mailbox_receive (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 2 * 4(%esp)\n"
                "pushl  32 + 4 + 2 * 4(%esp)\n"

                "call	system_call_mailbox_receive\n"

                "addl	$4 * 2, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 2\n");
}

void wrapper_service_create (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 3 * 4(%esp)\n"
                "pushl  32 + 4 + 3 * 4(%esp)\n"
                "pushl  32 + 4 + 3 * 4(%esp)\n"

                "call	system_call_service_create\n"

                "addl	$4 * 3, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 3\n");
}

void wrapper_service_destroy (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_service_destroy\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_service_get (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 3 * 4(%esp)\n"
                "pushl  32 + 4 + 3 * 4(%esp)\n"
                "pushl  32 + 4 + 3 * 4(%esp)\n"

                "call	system_call_service_get\n"

                "addl	$4 * 3, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 3\n");
}

void wrapper_service_protocol_get (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 2 * 4(%esp)\n"
                "pushl  32 + 4 + 2 * 4(%esp)\n"

                "call	system_call_service_protocol_get\n"

                "addl	$4 * 2, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 2\n");
}

void wrapper_service_protocol_get_amount (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_service_protocol_get_amount\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_dma_transfer (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 5 * 4(%esp)\n"
                "pushl  32 + 4 + 5 * 4(%esp)\n"
                "pushl  32 + 4 + 5 * 4(%esp)\n"
                "pushl  32 + 4 + 5 * 4(%esp)\n"
                "pushl  32 + 4 + 5 * 4(%esp)\n"

                "call	system_call_dma_transfer\n"

                "addl	$4 * 5, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 5\n");
}

void wrapper_dma_transfer_cancel (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_dma_transfer_cancel\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_dma_register (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 2 * 4(%esp)\n"
                "pushl  32 + 4 + 2 * 4(%esp)\n"

                "call	system_call_dma_register\n"

                "addl	$4 * 2, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 2\n");
}

void wrapper_dma_unregister (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_dma_unregister\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_irq_register (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 2 * 4(%esp)\n"
                "pushl  32 + 4 + 2 * 4(%esp)\n"

                "call	system_call_irq_register\n"

                "addl	$4 * 2, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 2\n");
}

void wrapper_irq_unregister (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_irq_unregister\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_irq_wait (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_irq_wait\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_irq_acknowledge (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_irq_acknowledge\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_memory_allocate (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 3 * 4(%esp)\n"
                "pushl  32 + 4 + 3 * 4(%esp)\n"
                "pushl  32 + 4 + 3 * 4(%esp)\n"

                "call	system_call_memory_allocate\n"

                "addl	$4 * 3, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 3\n");
}

void wrapper_memory_deallocate (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_memory_deallocate\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_memory_reserve (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 3 * 4(%esp)\n"
                "pushl  32 + 4 + 3 * 4(%esp)\n"
                "pushl  32 + 4 + 3 * 4(%esp)\n"

                "call	system_call_memory_reserve\n"

                "addl	$4 * 3, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 3\n");
}

void wrapper_memory_get_physical_address (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 2 * 4(%esp)\n"
                "pushl  32 + 4 + 2 * 4(%esp)\n"

                "call	system_call_memory_get_physical_address\n"

                "addl	$4 * 2, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 2\n");
}

void wrapper_port_range_register (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 3 * 4(%esp)\n"
                "pushl  32 + 4 + 3 * 4(%esp)\n"
                "pushl  32 + 4 + 3 * 4(%esp)\n"

                "call	system_call_port_range_register\n"

                "addl	$4 * 3, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 3\n");
}

void wrapper_port_range_unregister (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_port_range_unregister\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_process_create (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_process_create\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_process_name_set (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_process_name_set\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_process_parent_unblock (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */


                "call	system_call_process_parent_unblock\n"

                "addl	$4 * 0, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 0\n");
}

void wrapper_thread_create (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */


                "call	system_call_thread_create\n"

                "addl	$4 * 0, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 0\n");
}

void wrapper_thread_control (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 3 * 4(%esp)\n"
                "pushl  32 + 4 + 3 * 4(%esp)\n"
                "pushl  32 + 4 + 3 * 4(%esp)\n"

                "call	system_call_thread_control\n"

                "addl	$4 * 3, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 3\n");
}

void wrapper_thread_name_set (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_thread_name_set\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_timer_read (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */

                "pushl  32 + 4 + 1 * 4(%esp)\n"

                "call	system_call_timer_read\n"

                "addl	$4 * 1, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 1\n");
}

void wrapper_dispatch_next (void)
{
  asm ("pushal\n"
                /* Push all arguments. This is pretty smart... */


                "call	system_call_dispatch_next\n"

                "addl	$4 * 0, %esp\n"

		/* Simulate a popa, without overwriting EAX. */

		"popl	%edi\n"
		"popl	%esi\n"
		"popl	%ebp\n"

		/* ESP can't be popped for obvious reasons. */

		"addl	$4, %esp\n"
		"popl	%ebx\n"
		"popl	%edx\n"
		"popl	%ecx\n"

		/* EAX shall not be changed, since it is our return
          	  value. */

		"addl	$4, %esp\n"
		"lret	$4 * 0\n");
}
