// Abstract: PCI library return values.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

enum
{
    // The function completed successfully.
    PCI_RETURN_SUCCESS,

    // The service we tried to resolve was unavailable.
    PCI_RETURN_SERVICE_UNAVAILABLE,

    // We failed to allocate memory.
    PCI_RETURN_OUT_OF_MEMORY,
};
