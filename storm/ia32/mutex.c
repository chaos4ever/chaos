/* $Id$ */
/* Abstract: Mutexes for the kernel. */

/* Copyright 1999-2000 chaos development. */
/* Copyright 2007 chaos development. */

/* Defined this as true if you want lots of debug information .*/

#define DEBUG FALSE

#include <storm/generic/debug.h>
#include <storm/generic/dispatch.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/mutex.h>
#include <storm/generic/return_values.h>
#include <storm/generic/thread.h>
#include <storm/generic/types.h>
#include <storm/generic/defines.h>
#include <storm/state.h>

/* FIXME: Optimize this. The kernel doesn't really need a mutex tree,
   but it might be neat for regular processes. */

/* The internal format of a mutex. */

typedef struct
{
  /* Who may use the mutex. */

  process_id_type process_id;
  cluster_id_type cluster_id;
  thread_id_type thread_id;

  /* Mailbox id. */

  mutex_id_type id;

  /* The number of users who may access the mutex. */

  int queue_length;

  /* The rest of the mutex tree. */

  struct mutex_type *less;
  struct mutex_type *more;
} __attribute__ ((packed)) mutex_user_type;

/* Mutex lock variable. When zero (locked), no-one may access the
   mutex system. */

spinlock_type mutex_spinlock = MUTEX_SPIN_UNLOCKED;

/* The ID mutex locks the next_free_id variable. */

static mutex_kernel_type id_mutex = MUTEX_UNLOCKED;

#if FALSE

static int next_free_id = 0;

/* Get the first free mutex ID. */

static mutex_id_type mutex_get_free_id (void)
{
  int id;

  mutex_kernel_wait (&id_mutex);
  id = next_free_id;
  next_free_id++;
  mutex_kernel_signal (&id_mutex);

  return id;
}
#endif

/* Wait if this mutex is busy. */
/* FIXME: Support access checks. */

return_type mutex_kernel_wait (mutex_kernel_type *mutex)
{
#if FALSE
  /* This is used to find places where we used dual-mutexing -- which
     is very bad. */

  u32 esp = cpu_get_esp ();
  
  if (mutex == &tss_tree_mutex)
  {
    if (tss_tree_mutex == MUTEX_UNLOCKED)
    {
      DEBUG_MESSAGE (TRUE, "Called from %p", ((u32 *) esp)[7]);
    }
    else
    {
      DEBUG_MESSAGE (TRUE, "Called from %p (dispatcher locked)", ((u32 *) esp)[7]);
    }
  }
#endif

  if (tss_tree_mutex == MUTEX_LOCKED)
  {
    DEBUG_HALT ("Kernel tried to lock a mutex in a situation where tss_tree_mutex was already locked.");
  }

  mutex_spin_lock (mutex_spinlock);

  /* Can we access the mutex? */

  if (*mutex == MUTEX_LOCKED)
  {
    if (mutex == &tss_tree_mutex)
    {
      DEBUG_HALT ("Tried to lock tss_tree_mutex, which was already locked. This should really never happen.");
    }
    DEBUG_MESSAGE (DEBUG, "%p is LOCKED!", mutex);
    mutex_spin_unlock (mutex_spinlock);

    thread_block_kernel_mutex (current_tss, mutex);
  }
  
  /* Oh, yes. Let's HSLINK it. */

  else
  {
    DEBUG_MESSAGE (DEBUG, "Locking %p", mutex);
    *mutex = MUTEX_LOCKED;
    mutex_spin_unlock (mutex_spinlock);
  }

  return RETURN_SUCCESS;
}

/* Signal that a mutex can be released. */

return_type mutex_kernel_signal (mutex_kernel_type *mutex)
{
#if FALSE
  /* This is used to find places where we used dual-mutexing -- which
     is very bad. */

  u32 esp = cpu_get_esp ();
  
  if (mutex == &tss_tree_mutex)
  {
    DEBUG_MESSAGE (TRUE, "Called from %p", ((u32 *) esp)[7]);
  }
#endif

  mutex_spin_lock (mutex_spinlock);

  /* If no threads were unblocked because of this, no thread were
     waiting for the mutex. Set it free. */

  if (thread_unblock_kernel_mutex (mutex) == RETURN_NO_THREAD_UNBLOCKED)
  {
    DEBUG_MESSAGE (DEBUG, "Unlocking %p", mutex);
    *mutex = MUTEX_UNLOCKED;
  }

  mutex_spin_unlock (mutex_spinlock);

  return RETURN_SUCCESS;
}
