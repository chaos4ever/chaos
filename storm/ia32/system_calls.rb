#!/usr/bin/env ruby

# Abstract: Generate files with system call stuff. Since they are a bunch, updating them all manually was a little
# uncomfortable. Ideally, the architecture independant parts of this file should be in a generic directory.
#
# Author: Per Lundberg <per@halleluja.nu>

# The lowest entry in the GDT we may use.
gdt_start = 48

# Always add things at the END of this list! Otherwise, you'll have to recompile all programs, libraries and
# everything... so please, don't.
system_calls = Hash[
 'init',                          0,
 'kernelfs_entry_read',           1,
 
 'mailbox_create',                5,
 'mailbox_destroy',               1,
 'mailbox_flush',                 1,
 'mailbox_send',                  2,
 'mailbox_receive',               2,

 'service_create',                3,
 'service_destroy',		  1,
 'service_get',                   3,
 'service_protocol_get',          2,
 'service_protocol_get_amount',   1,
 
 'dma_transfer',                  5,
 'dma_transfer_cancel',           1,
 'dma_register',                  2,
 'dma_unregister',                1,

 'irq_register',                  2,
 'irq_unregister',                1,
 'irq_wait',                      1,
 'irq_acknowledge',               1,

 'memory_allocate',		  3,
 'memory_deallocate',             1,
 'memory_reserve',                3,
 'memory_get_physical_address',   2,
 
 'port_range_register',           3,
 'port_range_unregister',         1,

 'process_create',                1,
 'process_name_set',              1,
 'process_parent_unblock',        0,

 # 'cluster_create',		  0

 'thread_create',                 0,
 'thread_control',                3,
 'thread_name_set',               1,

 'timer_read',                    1,

 # This system call releases all the rest of this timeslice and
 # continues the task switching.

 'dispatch_next',                 0,
]

$0.sub! 'system_calls.rb', ''
Dir.chdir($0) or fail "Couldn't change directory: $!"

file = File.open('wrapper.c', 'wb') or fail "Couldn't create wrapper.c"

file.puts
"/* Generated automatically by system_calls.rb. Don't change. */

#include <storm/ia32/defines.h>
#include <storm/ia32/wrapper.h>
"

system_calls.each do |system_call, num_parameters|
  file.puts "\
void wrapper_#{system_call}(void)
{
  asm (\"pushal\\n\""

  file.puts "\
                /* Push all arguments. This is pretty smart... */

"

  for parameter in 0..num_parameters do
    file.puts "                \"pushl  32 + 4 + #{num_parameters} * 4(%esp)\\n\"\n"
  end

  file.puts "\
                ""call  system_call_#{system_call}\\n""

                ""addl  \$4 * #{num_parameters}, %esp\\n""

    /* Simulate a popa, without overwriting EAX. */

    ""popl  %edi\\n""
    ""popl  %esi\\n""
    ""popl  %ebp\\n""

    /* ESP can't be popped for obvious reasons. */

    ""addl  \$4, %esp\\n""
    ""popl  %ebx\\n""
    ""popl  %edx\\n""
    ""popl  %ecx\\n""

    /* EAX shall not be changed, since it is our return value. */

    ""addl  \$4, %esp\\n""
    ""lret  \$4 * #{num_parameters}\\n"");
}
"
end

file.close

file = File.open('../include/storm/system_calls.h', 'wb') or fail "Couldn't create storm/system_calls.h"

file.puts(
  "// Generated automatically by system_calls.pl. Do not modify!

#pragma once

#define SYSTEM_CALLS #{system_calls.keys.count}

")
  
file.puts "enum\n{\n  SYSTEM_CALL_#{system_calls.keys.first.upcase} = #{gdt_start},\n"

system_calls.keys[1...system_calls.keys.count].each do |system_call|
  file.puts "  SYSTEM_CALL_#{system_call.upcase},\n"
end

file.puts "};\n"
  
file.close

file = File.open('system_calls-auto.c', 'wb') or fail "Couldn't create system_call-auto.c"

file.puts "/* Generated automatically by system_calls.pl */

#include <storm/ia32/system_calls.h>
#include <storm/ia32/wrapper.h>

const system_call_type system_call[] =
{
"

system_calls.each { |system_call, num_parameters|
  file.puts "  { SYSTEM_CALL_#{system_call.upcase}, wrapper_#{system_call}, #{num_parameters} },\n"
}

file.puts "};\n"
file.close

file = File.open('../include/storm/ia32/wrapper.h', 'wb') or fail "Couldn't create wrapper.h"

file.puts "/* Generated automatically by system_calls.pl */

#pragma once

"

system_calls.each { |system_call|
  file.puts "void wrapper_system_call(void);\n"
}

file.close
