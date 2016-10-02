// Abstract: Definition of the data area which is used for communication between the startup code and the kernel.
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 1998-1999, 2013 chaos development.

#pragma once

#include <storm/generic/types.h>

typedef struct
{
  uint8_t display_type;
  uint16_t x_size;
  uint16_t y_size;
  uint8_t x_position;
  uint8_t y_position;
} __attribute__ ((packed)) dataarea_type;

extern dataarea_type dataarea;
