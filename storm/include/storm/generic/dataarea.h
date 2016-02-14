// Abstract: Definition of the data area which is used for communication between the startup code and the kernel.
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 1998-1999, 2013 chaos development.

#pragma once

#include <storm/generic/types.h>

typedef struct
{
  u8 display_type;
  u16 x_size;
  u16 y_size;
  u8 x_position;
  u8 y_position;
} __attribute__ ((packed)) dataarea_type;

extern dataarea_type dataarea;
