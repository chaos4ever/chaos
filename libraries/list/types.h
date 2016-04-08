// Abstract: List type definitions.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

// Type definitions.
typedef struct
{
  struct list_type *next;
  struct list_type *previous;

  // Arbitrary data may follow. We don't care about it.
} list_type;
