// Generated automatically by system_calls.rb. Do not modify!

#include <storm/x86/defines.h>
#include <storm/x86/wrapper.h>

void wrapper_init(void)
{
  asm("pushal\n"

      "call   system_call_init\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 0\n");
}

void wrapper_kernelfs_entry_read(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_kernelfs_entry_read\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_mailbox_create(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 5 * 4(%esp)\n"
      "pushl 32 + 4 + 5 * 4(%esp)\n"
      "pushl 32 + 4 + 5 * 4(%esp)\n"
      "pushl 32 + 4 + 5 * 4(%esp)\n"
      "pushl 32 + 4 + 5 * 4(%esp)\n"

      "call   system_call_mailbox_create\n"      

      // Restore the stack after the function call
      "addl   $4 * 5, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 5\n");
}

void wrapper_mailbox_destroy(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_mailbox_destroy\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_mailbox_flush(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_mailbox_flush\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_mailbox_send(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 2 * 4(%esp)\n"
      "pushl 32 + 4 + 2 * 4(%esp)\n"

      "call   system_call_mailbox_send\n"      

      // Restore the stack after the function call
      "addl   $4 * 2, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 2\n");
}

void wrapper_mailbox_receive(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 2 * 4(%esp)\n"
      "pushl 32 + 4 + 2 * 4(%esp)\n"

      "call   system_call_mailbox_receive\n"      

      // Restore the stack after the function call
      "addl   $4 * 2, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 2\n");
}

void wrapper_service_create(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 3 * 4(%esp)\n"
      "pushl 32 + 4 + 3 * 4(%esp)\n"
      "pushl 32 + 4 + 3 * 4(%esp)\n"

      "call   system_call_service_create\n"      

      // Restore the stack after the function call
      "addl   $4 * 3, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 3\n");
}

void wrapper_service_destroy(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_service_destroy\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_service_get(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 3 * 4(%esp)\n"
      "pushl 32 + 4 + 3 * 4(%esp)\n"
      "pushl 32 + 4 + 3 * 4(%esp)\n"

      "call   system_call_service_get\n"      

      // Restore the stack after the function call
      "addl   $4 * 3, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 3\n");
}

void wrapper_service_protocol_get(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 2 * 4(%esp)\n"
      "pushl 32 + 4 + 2 * 4(%esp)\n"

      "call   system_call_service_protocol_get\n"      

      // Restore the stack after the function call
      "addl   $4 * 2, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 2\n");
}

void wrapper_service_protocol_get_amount(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_service_protocol_get_amount\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_dma_transfer(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 5 * 4(%esp)\n"
      "pushl 32 + 4 + 5 * 4(%esp)\n"
      "pushl 32 + 4 + 5 * 4(%esp)\n"
      "pushl 32 + 4 + 5 * 4(%esp)\n"
      "pushl 32 + 4 + 5 * 4(%esp)\n"

      "call   system_call_dma_transfer\n"      

      // Restore the stack after the function call
      "addl   $4 * 5, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 5\n");
}

void wrapper_dma_transfer_cancel(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_dma_transfer_cancel\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_dma_register(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 2 * 4(%esp)\n"
      "pushl 32 + 4 + 2 * 4(%esp)\n"

      "call   system_call_dma_register\n"      

      // Restore the stack after the function call
      "addl   $4 * 2, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 2\n");
}

void wrapper_dma_unregister(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_dma_unregister\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_irq_register(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 2 * 4(%esp)\n"
      "pushl 32 + 4 + 2 * 4(%esp)\n"

      "call   system_call_irq_register\n"      

      // Restore the stack after the function call
      "addl   $4 * 2, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 2\n");
}

void wrapper_irq_unregister(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_irq_unregister\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_irq_wait(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_irq_wait\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_irq_acknowledge(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_irq_acknowledge\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_memory_allocate(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 3 * 4(%esp)\n"
      "pushl 32 + 4 + 3 * 4(%esp)\n"
      "pushl 32 + 4 + 3 * 4(%esp)\n"

      "call   system_call_memory_allocate\n"      

      // Restore the stack after the function call
      "addl   $4 * 3, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 3\n");
}

void wrapper_memory_deallocate(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_memory_deallocate\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_memory_reserve(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 3 * 4(%esp)\n"
      "pushl 32 + 4 + 3 * 4(%esp)\n"
      "pushl 32 + 4 + 3 * 4(%esp)\n"

      "call   system_call_memory_reserve\n"      

      // Restore the stack after the function call
      "addl   $4 * 3, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 3\n");
}

void wrapper_memory_get_physical_address(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 2 * 4(%esp)\n"
      "pushl 32 + 4 + 2 * 4(%esp)\n"

      "call   system_call_memory_get_physical_address\n"      

      // Restore the stack after the function call
      "addl   $4 * 2, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 2\n");
}

void wrapper_port_range_register(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 3 * 4(%esp)\n"
      "pushl 32 + 4 + 3 * 4(%esp)\n"
      "pushl 32 + 4 + 3 * 4(%esp)\n"

      "call   system_call_port_range_register\n"      

      // Restore the stack after the function call
      "addl   $4 * 3, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 3\n");
}

void wrapper_port_range_unregister(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_port_range_unregister\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_process_create(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_process_create\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_process_name_set(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_process_name_set\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_process_parent_unblock(void)
{
  asm("pushal\n"

      "call   system_call_process_parent_unblock\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 0\n");
}

void wrapper_thread_create(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 2 * 4(%esp)\n"
      "pushl 32 + 4 + 2 * 4(%esp)\n"

      "call   system_call_thread_create\n"      

      // Restore the stack after the function call
      "addl   $4 * 2, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 2\n");
}

void wrapper_thread_control(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 3 * 4(%esp)\n"
      "pushl 32 + 4 + 3 * 4(%esp)\n"
      "pushl 32 + 4 + 3 * 4(%esp)\n"

      "call   system_call_thread_control\n"      

      // Restore the stack after the function call
      "addl   $4 * 3, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 3\n");
}

void wrapper_thread_name_set(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_thread_name_set\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_timer_read(void)
{
  asm("pushal\n"

      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)
      "pushl 32 + 4 + 1 * 4(%esp)\n"

      "call   system_call_timer_read\n"      

      // Restore the stack after the function call
      "addl   $4 * 1, %esp\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 1\n");
}

void wrapper_dispatch_next(void)
{
  asm("pushal\n"

      "call   system_call_dispatch_next\n"      

      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\n"
      "popl   %esi\n"
      "popl   %ebp\n"

      // ESP can't be popped for obvious reasons.
      "addl   $4, %esp\n"
      "popl   %ebx\n"
      "popl   %edx\n"
      "popl   %ecx\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   $4, %esp\n"
      "lret   $4 * 0\n");
}
