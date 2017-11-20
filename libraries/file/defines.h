// Abstract: Defines used by the file library.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#pragma once

enum
{
    // Open the file for reading.
    FILE_MODE_READ,

    // Open the file for writing (overwriting any previous contents).
    FILE_MODE_WRITE,
};

// Directory entry types.
enum
{
    FILE_ENTRY_TYPE_DIRECTORY,
    FILE_ENTRY_TYPE_FILE,
    FILE_ENTRY_TYPE_SOFT_LINK,
    FILE_ENTRY_TYPE_HARD_LINK,
};

// The maxium number of ACL:s for one entity.
#define MAX_ACL                 16

// The maximum number of elements in a path.
#define MAX_PATH_ELEMENTS       16

#define PATH_NAME_SEPARATOR     '/'
