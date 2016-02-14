// Abstract: List library functions.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development
// © Copyright 2015 chaos development

#pragma once

#include <system/system.h>

extern return_type list_node_insert (list_type **list, list_type *node);
extern return_type list_node_delete (list_type **list, list_type *node);
