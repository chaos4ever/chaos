// Abstract: Time management.
// Author: Per Lundberg <per@chaosdev.io>

// Copyright 2000‚ 2013 chaos development.

#include <storm/current-arch/types.h>

// FIXME: We could just store the start time when the system boots,
// and then always add the uptime when returning this. That way, we
// will only have to update one counter on each timer interrupt.
time_type time;
