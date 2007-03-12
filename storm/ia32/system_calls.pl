#!/usr/bin/perl

# $Id: system_calls.pl,v 1.8 2000/10/22 21:38:16 plundis Exp $

# Abstract: Generate files with system call stuff. Since they are a
# bunch, updating them all manually was a little
# uncomfortable. Ideally, the architecture independant parts of this
# file should be in a generic directory.
# Author: Per Lundberg <plundis@chaosdev.org>

# C-emulation. :)

use strict;

# The lowest entry in the GDT we may use.

my $gdt_start = 48;

my $count;

# Always add things at the END of this list! Otherwise, you'll have to
# recompile all programs, libraries and everything... so please, don't.

my @system_call =  
(
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
);

my $system_calls = scalar @system_call / 2;

# Subroutine for converting a string to all uppercase.

sub toupper
{
  my $string = shift;
 $string =~ tr/a-z/A-Z/;
  return $string;
};

# First of all, switch to the directory where $0 is located.

$0 =~ s/system_calls.pl//;
chdir ($0) or die "Couldn't change directory: $!";

# Start with wrapper.c

my $FILE;

open (FILE, ">wrapper.c") or die ("Couldn't create wrapper.c");

print (FILE 
"/* Generated automatically by system_calls.pl. Don't change. */

#include <storm/ia32/defines.h>
#include <storm/ia32/wrapper.h>
");

for ($count = 0; $count < $system_calls; $count++)
{
  print (FILE "\
void wrapper_$system_call[$count * 2] (void)
{
  asm (\"
                pushal
");

  print (FILE "\
                /* Push all arguments. This is pretty smart... */

");

  for (my $variables = 0; $variables < $system_call[$count * 2 + 1];
       $variables++)
  {
    print (FILE "                pushl  32 + 4 + $system_call[$count * 2 + 1] * 4(%esp)\n");
  }
  print (FILE "\
                call	system_call_$system_call[$count * 2]

                addl	\$4 * $system_call[$count * 2 + 1], %esp

		/* Simulate a popa, without overwriting EAX. */

		popl	%edi
		popl	%esi
		popl	%ebp

		/* ESP can't be popped for obvious reasons. */

		addl	\$4, %esp
		popl	%ebx
		popl	%edx
		popl	%ecx

		/* EAX shall not be changed, since it is our return
          	  value. */

		addl	\$4, %esp
		lret	\$4 * $system_call[$count * 2 + 1]
  \");
}
");
}

close (FILE);

# ...and continue to include/storm/system_calls.h

open (FILE, ">../include/storm/system_calls.h") or die ("Couldn't create storm/system_calls.h");

print (FILE "/* Generated automatically by system_calls.pl */\n\n");

printf (FILE "#ifndef __STORM_SYSTEM_CALLS_H__\
#define __STORM_SYSTEM_CALLS_H__\n\
#define SYSTEM_CALLS %u\n\n", $system_calls);
  
printf (FILE "enum\n{\n  SYSTEM_CALL_%s = $gdt_start,\n", toupper ($system_call[0]));

for ($count = 1; $count < $system_calls; $count++)
{
  printf (FILE "  SYSTEM_CALL_%s,\n", toupper ($system_call[$count * 2]))
};

print (FILE "};\n\
#endif /* !__STORM_SYSTEM_CALL_H__ */");
  
close (FILE);

# Now, let's create system_calls-auto.c.

open (FILE, ">system_calls-auto.c") or die ("Couldn't create system_call-auto.c");

print (FILE "/* Generated automatically by system_calls.pl */\n\
#include <storm/ia32/system_calls.h>\
#include <storm/ia32/wrapper.h>\n\
const system_call_type system_call[] =\n{\n");

for ($count = 0; $count < $system_calls; $count++)
{
  printf (FILE "  { SYSTEM_CALL_%s, wrapper_%s, $system_call[$count * 2 + 1] },\n",
	  toupper ($system_call[$count * 2]), $system_call[$count * 2]);
}

print (FILE "};");
close (FILE);

# ...and include/storm/wrapper.h

open (FILE, ">../include/storm/ia32/wrapper.h") or die ("Couldn't create wrapper.h");

print (FILE "/* Generated automatically by system_calls.pl */\n\
#ifndef __STORM_IA32_WRAPPER_H__\
#define __STORM_IA32_WRAPPER_H__\n\n");

for ($count = 0; $count < $system_calls; $count++)
{
  print (FILE "void wrapper_$system_call[$count * 2] (void);\n")
}

print (FILE "\n#endif /* !__STORM_IA32_WRAPPER_H__ */");
close (FILE);

# Over and out!
