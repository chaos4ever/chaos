// Abstract: Definitions of task states.
// Authors: Henrik Hallin <hal@chaosdev.org>
//          Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999-2000, 2013 chaos development.

#pragma once

// FIXME: Define exactly which states are needed.
enum
{
    // The thread is ready to be dispatched.
    STATE_DISPATCH = 0,

    // The thread is waiting to have something sent.
    STATE_MAILBOX_SEND,

    // The thread is waiting for something to be received.
    STATE_MAILBOX_RECEIVE,

    // The thread is blocked on a kernel mutex.
    STATE_MUTEX_KERNEL,

    // The thread is blocked on a user mutex.
    STATE_MUTEX_USER,

    // The thread is an idle thread, and is thus idle. :-)
    STATE_IDLE,

    // Thread is in a zombie state and is waiting to be removed by the idle thread.
    STATE_ZOMBIE,

    // The thread is waiting for an interrupt.
    STATE_INTERRUPT_WAIT,

    // The thread is under creation.
    STATE_UNREADY,

    // The thread is sleeping unconditionally.
    STATE_SLEEP,

    // The thread is blocked, waiting for a kernel log event.
    STATE_LOG_READ,

    // The thread is blocked on its parent.
    STATE_BLOCKED_PARENT,

    // The thread is blocked during bootup.
    STATE_BLOCKED_BOOT,
};
