// Abstract: Library for calculating random numbers.
// Authors: Per Lundberg <per@chaosdev.io>
//          Henrik Hallin <hal@chaosdev.org>
//
// © Copyright 2000 chaos development
// © Copyright 2015 chaos development

#include <random/random.h>
#include <system/system.h>

// The algorithm is pretty pathetic right now, but it's better than nothing...
static volatile int random_seed = 0;

// Initialise the random seed.
return_type random_init(int seed)
{
    random_seed = seed;

    return RANDOM_RETURN_SUCCESS;
}

// Get a random number with max size max in *output.
int random(int max)
{
    random_seed = (random_seed * 12345 + 67892);
    return random_seed & max;
}
