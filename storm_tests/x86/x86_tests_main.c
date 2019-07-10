// Abstract: Entry point for the unit test program.
// Authors: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2015 chaos development

#include "../test_helper.h"

extern void test_memory_global_allocate(void **state);
extern void test_memory_global_deallocate(void **state);
extern void string_to_number_decimal(void **state);
extern void string_to_number_negative_decimal(void **state);
extern void string_to_number_invalid_value(void **state);
extern void string_to_number_hexadecimal(void **state);
extern void string_to_number_binary(void **state);

int main(void)
{
    const struct CMUnitTest memory_global_tests[] =
    {
        cmocka_unit_test(test_memory_global_allocate),
        cmocka_unit_test(test_memory_global_deallocate),
    };

    const struct CMUnitTest string_tests[] =
    {
        cmocka_unit_test(string_to_number_decimal),
        cmocka_unit_test(string_to_number_negative_decimal),
        cmocka_unit_test(string_to_number_invalid_value),
        cmocka_unit_test(string_to_number_hexadecimal),
        cmocka_unit_test(string_to_number_binary)
    };

    cmocka_run_group_tests_name("memory_global", memory_global_tests, NULL, NULL);
    cmocka_run_group_tests_name("string", string_tests, NULL, NULL);
}
