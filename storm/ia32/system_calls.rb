#!/usr/bin/env ruby

# Abstract: Generate files with system call stuff. Since they are a bunch, updating them all manually was a little
# uncomfortable. Ideally, the architecture independant parts of this file should be in a generic directory.
#
# Author: Per Lundberg <per@halleluja.nu>

# The lowest entry in the GDT we may use.
$gdt_start = 48

# Always add things at the END of this list! Otherwise, you'll have to recompile all programs, libraries and
# everything... so please, don't.
system_calls = Hash[
  'init',                         0,
  'kernelfs_entry_read',          1,

  'mailbox_create',               5,
  'mailbox_destroy',              1,
  'mailbox_flush',                1,
  'mailbox_send',                 2,
  'mailbox_receive',              2,

  'service_create',               3,
  'service_destroy',              1,
  'service_get',                  3,
  'service_protocol_get',         2,
  'service_protocol_get_amount',  1,

  'dma_transfer',                 5,
  'dma_transfer_cancel',          1,
  'dma_register',                 2,
  'dma_unregister',               1,

  'irq_register',                 2,
  'irq_unregister',               1,
  'irq_wait',                     1,
  'irq_acknowledge',              1,

  'memory_allocate',              3,
  'memory_deallocate',            1,
  'memory_reserve',               3,
  'memory_get_physical_address',  2,

  'port_range_register',          3,
  'port_range_unregister',        1,

  'process_create',               1,
  'process_name_set',             1,
  'process_parent_unblock',       0,

  'thread_create',                2,
  'thread_control',               3,
  'thread_name_set',              1,

  'timer_read',                   1,

  # This system call releases all the rest of this timeslice and continues the task switching.
  'dispatch_next',                0,
]

def create_wrapper_c(system_calls)
  File.open('wrapper.c', 'wb') do |file|
    file.puts(
"// Generated automatically by system_calls.rb. Do not modify!

#include <storm/ia32/defines.h>
#include <storm/ia32/wrapper.h>
")

    system_calls.each do |system_call, num_parameters|
      file.puts %Q[
void wrapper_#{system_call}(void)
{
  asm("pushal\\n\"]

      if num_parameters > 0
        file.puts '
      // Push all arguments. This approach pretty smart; it utilizes the fact that the stack grows downwards
      // so the "next parameter to push" is always in the same memory location. :)'
      end

      for parameter in 0..num_parameters - 1 do
        file.puts("\
      \"pushl 32 + 4 + #{num_parameters} * 4(%esp)\\n\"\n"
        )
      end

      file.puts %Q[\

      "call   system_call_#{system_call}\\n"\
      ]

      if num_parameters > 0
        file.puts %Q[
      // Restore the stack after the function call
      "addl   \$4 * #{num_parameters}, %esp\\n"\
      ]
      end

      file.puts %Q[
      // Simulate a popa, without overwriting EAX (since it contains the return value from the system call).
      "popl   %edi\\n"
      "popl   %esi\\n"
      "popl   %ebp\\n"

      // ESP can't be popped for obvious reasons.
      "addl   \$4, %esp\\n"
      "popl   %ebx\\n"
      "popl   %edx\\n"
      "popl   %ecx\\n"

      // Adjust the stack for the fact that EAX isn't being popped.
      "addl   \$4, %esp\\n"
      "lret   \$4 * #{num_parameters}\\n");
}
]
    end
  end
end

def create_include_storm_system_calls_h(system_calls)
  File.open('../include/storm/system_calls.h', 'wb') do |file|
    file.puts(
"// Generated automatically by system_calls.rb. Do not modify!

#pragma once

#define SYSTEM_CALLS #{system_calls.keys.count}

")

    file.puts "enum\n{\n  SYSTEM_CALL_#{system_calls.keys.first.upcase} = #{$gdt_start},\n"

    system_calls.keys[1...system_calls.keys.count].each do |system_call|
      file.puts "  SYSTEM_CALL_#{system_call.upcase},\n"
    end

    file.puts "};\n"
  end
end

def create_system_calls_auto_c(system_calls)
  File.open('system_calls-auto.c', 'wb') do |file|
    file.puts "// Generated automatically by system_calls.rb. Do not modify!

#include <storm/ia32/system_calls.h>
#include <storm/ia32/wrapper.h>

const system_call_type system_call[] =
{
"

    system_calls.each do |system_call, num_parameters|
      file.puts "  { SYSTEM_CALL_#{system_call.upcase}, wrapper_#{system_call}, #{num_parameters} },\n"
    end

    file.puts "};\n"
  end
end

def create_include_storm_ia32_wrapper_h(system_calls)
  File.open('../include/storm/ia32/wrapper.h', 'wb') do |file|
    file.puts "// Generated automatically by system_calls.rb. Do not modify!

#pragma once

"

    system_calls.keys.each { |system_call| file.puts "void wrapper_#{system_call}(void);\n" }
  end
end

script_directory = $0.sub 'system_calls.rb', ''
Dir.chdir(script_directory) or fail "Couldn't change directory: $!"

create_wrapper_c system_calls
create_include_storm_system_calls_h system_calls
create_system_calls_auto_c system_calls
create_include_storm_ia32_wrapper_h system_calls
