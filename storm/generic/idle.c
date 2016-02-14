// Abstract: Idle thread.
// Author: Per Lundberg <per@halleluja.nu>

// © Copyright 2000, 2013, 2016 chaos development.

#include <storm/generic/debug.h>
#include <storm/generic/idle.h>
#include <storm/generic/process.h>
#include <storm/generic/string.h>
#include <storm/state.h>

void idle (void)
{
  string_copy (current_tss->thread_name, "Idle thread");

  while (TRUE)
  {
    // FIXME: If we get here, the system is totaly asleep, and we can safely check for tasks to do. For now, only try to find a
    // thread to run.
    dispatch_next ();
  }
}
