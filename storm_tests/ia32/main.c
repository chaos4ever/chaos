// Abstract: Entry point for the unit test program.
// Authors: Per Lundberg <per@halleluja.nu>
//
// © Copyright 2015 chaos development

#include "test_helper.h"

extern void test_memory_global_deallocate(void **state);

int main(void)
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_memory_global_deallocate),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}

