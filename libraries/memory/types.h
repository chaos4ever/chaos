// Abstract: Memory library types.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development.

#pragma once

#include <memory/slab.h>

typedef struct
{
  volatile bool locked;
  slab_heap_type *slab_heap;
} memory_structure_type;
