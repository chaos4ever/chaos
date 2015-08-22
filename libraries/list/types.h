// Abstract: List type definitions.
// Author: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2000 chaos development
// © Copyright 2015 chaos development

#pragma once

// Type definitions.
typedef struct
{
  struct list_type *next;
  struct list_type *previous;

  // Arbitrary data may follow. We don't care about it.
} list_type;
