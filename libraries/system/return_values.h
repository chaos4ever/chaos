// Abstract: Return values used by the system library.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2000 chaos development
// © Copyright 2015 chaos development

#pragma once

enum
{
  /* The call completed successfully. */

  SYSTEM_RETURN_SUCCESS,

  /* Those are returned by system_thread_create (), so that the caller
     knows how to distinguish between the old and the new thread. */

  SYSTEM_RETURN_THREAD_NEW,
  SYSTEM_RETURN_THREAD_OLD,

  /* Something went wrong in system_call_thread_create (), and we do
     not yet handle it properly. Please fix. */

  SYSTEM_RETURN_THREAD_CREATE_FAILED,
};
