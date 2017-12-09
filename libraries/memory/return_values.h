// Abstract: Return values for the memory library.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development.

#pragma once

enum
{
  // The function call succeeded.
  MEMORY_RETURN_SUCCESS,

  // Out of memory.
  MEMORY_RETURN_OUT_OF_MEMORY,

  // The SLAB system we tried to access was broken.
  MEMORY_RETURN_SLAB_BROKEN,
};
