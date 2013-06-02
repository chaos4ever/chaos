// Abstract: Time management.
// Author: Per Lundberg <per@halleluja.nu>

// Copyright 2000, 2013 chaos development.

#pragma once

#include <storm/current-arch/types.h>
#include <storm/current-arch/timer.h>

// External variables.
extern time_type time;
extern volatile time_type uptime;
extern u32 hz;

// Function prototypes.
extern void time_init (void);
