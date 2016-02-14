// Abstract: Tag item management.
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 2000, 2013 chaos development.

#pragma once

#include <storm/generic/defines.h>
#include <storm/generic/types.h>
#include <storm/tag.h>

extern unsigned int tag_get_length (tag_type *tag);
extern bool tag_masked_compare (tag_type *tag, tag_type *tag_mask);
