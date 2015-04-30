// Abstract: Memory allocation functions for the global memory area (unit tests)
// Authors: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2015 chaos development

#include "test_helper.h"

#include <stdlib.h>
#include <storm/generic/memory_global.h>
#include <storm/generic/return_values.h>

return_type memory_physical_allocate(u32 *page, unsigned int length, char *description)
{
    void *p;
    int return_value = posix_memalign(&p, SIZE_PAGE, SIZE_PAGE * length);
    assert_int_equal(return_value, 0);
    
    return RETURN_SUCCESS;
}

return_type memory_virtual_map(u32 virtual_page, u32 physical_page, u32 pages, u32 flags)
{
    return RETURN_SUCCESS;
}

void test_memory_global_allocate(void **state)
{
    memory_global_init();

    void *p = memory_global_allocate(1024);
    assert_non_null(p);
}

void test_memory_global_deallocate(void **state)
{
    memory_global_init();

    void *p = memory_global_allocate(1024);
    assert_true(memory_global_deallocate(&p) == RETURN_SUCCESS);
    assert_null(p);
}
