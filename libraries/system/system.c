/* $Id$ */
/* Abstract: Library for system related stuff. */
/* Author: Per Lundberg <plundis@chaosdev.org> */

/* Copyright 2000 chaos development. */

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

#include <system/system.h>

/* Shut down the system. */

return_type system_shutdown (void)
{
  /* Make sure we are root. If not, we will only end up killing our
     own processes, which is probably not what we want... */

  /* Kill all processes. */

  while (TRUE);
}

/* Sleep for a given amount of time. (In milliseconds) */

return_type system_sleep (unsigned int time)
{
#if FALSE
  time_type start_time, current_time;

  system_call_timer_read (&start_time);
  current_time = start_time;
  while (current_time < start_time + time + 1)
  {
    system_call_timer_read (&current_time);
    system_call_dispatch_next ();
  }
#endif
  
  kernelfs_self_type kernelfs_self;
  kernelfs_self.kernelfs_class = KERNELFS_CLASS_SELF_INFO;
  system_call_kernelfs_entry_read (&kernelfs_self);

  system_call_thread_control (kernelfs_self.thread_id, THREAD_SLEEP,
                              time);
  return SYSTEM_RETURN_SUCCESS;
}

/* Sleep for a given amount of time. (In microseconds) */

return_type system_sleep_microseconds (unsigned int time)
{
  kernelfs_self_type kernelfs_self;
  kernelfs_self.kernelfs_class = KERNELFS_CLASS_SELF_INFO;
  system_call_kernelfs_entry_read (&kernelfs_self);

  system_call_thread_control (kernelfs_self.thread_id, THREAD_SLEEP,
                              (time / 1000) + 1);
  return SYSTEM_RETURN_SUCCESS;
}

/* Set the name of the current process. */

return_type system_process_name_set (char *name)
{
  system_call_process_name_set (name);

  return SYSTEM_RETURN_SUCCESS;
}

/* Set the name of the current cluster. */

return_type system_cluster_name_set (char *name __attribute__ ((unused)))
{
  /* FIXME: Do something here. I guess we need a conforming system
     call first... */

  return SYSTEM_RETURN_SUCCESS;
}

/* Set the name of the current thread. */

return_type system_thread_name_set (char *name)
{
  system_call_thread_name_set (name);

  return SYSTEM_RETURN_SUCCESS;
}

/* Create a new thread. */

return_type system_thread_create (void)
{
  switch (system_call_thread_create ())
  {
    case STORM_RETURN_THREAD_NEW:
    {
      return SYSTEM_RETURN_THREAD_NEW;
    }

    case STORM_RETURN_THREAD_OLD:
    {
      return SYSTEM_RETURN_THREAD_OLD;
    }

    /* Someone has added a return code in thread.c in the kernel
       without handling it in the system library. Please fix this in
       the system library and send us a patch. */

    default:
    {
      return SYSTEM_RETURN_THREAD_CREATE_FAILED;
    }
  }
}

/* Execute the program specified in the foreground (that is, this
   function doesn't return until the child process has been
   terminated) */
/* FIXME: Maybe have a flag that says whether we should wait or not? */
/* FIXME: Should we open a connection to the VFS on startup, or...? */

return_type system_execute (char *program __attribute__ ((unused)))
{
  return SYSTEM_RETURN_SUCCESS;
}

/* Abort the current thread. */

return_type system_exit (void)
{
  kernelfs_self_type kernelfs_self;
  
  kernelfs_self.kernelfs_class = KERNELFS_CLASS_SELF_INFO;
  system_call_kernelfs_entry_read (&kernelfs_self);
  system_call_thread_control (kernelfs_self.thread_id, THREAD_TERMINATE, 0);
  while (TRUE);
}
