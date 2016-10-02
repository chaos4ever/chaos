// Abstract: Memory allocation functions for the global memory area (unit tests)
// Authors: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2015-2016 chaos development

#include "test_helper.h"

#include <stdlib.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/return_values.h>

return_type memory_physical_allocate(uint32_t *page, unsigned int length, char *description)
{
    void *p;
    int return_value = posix_memalign(&p, SIZE_PAGE, SIZE_PAGE * length);
    assert_int_equal(return_value, 0);
    
    return RETURN_SUCCESS;
}

return_type memory_virtual_map(uint32_t virtual_page, uint32_t physical_page, uint32_t pages, uint32_t flags)
{
    return RETURN_SUCCESS;
}

void test_memory_global_allocate(void **state)
{
    return;

    // TODO: Make the test work. At the moment, memory_global_init() and friends very much rely on stuff being
    // available in specific memory locations.
    memory_global_init();

    void *p = memory_global_allocate(1024);
    assert_non_null(p);
}

void test_memory_global_deallocate(void **state)
{
    return;

    // TODO: Make the test work. At the moment, memory_global_init() and friends very much rely on stuff being
    // available in specific memory locations.
    memory_global_init();

    void *p = memory_global_allocate(1024);
    assert_true(memory_global_deallocate(&p) == RETURN_SUCCESS);
    assert_null(p);
}
