// Abstract: String routines (unit tests)
// Authors: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2015-2016 chaos development

#include "test_helper.h"

#include <storm/generic/string.h>

void string_to_number_decimal(void **state)
{
    int i;
    string_to_number("10", &i);

    assert_int_equal(i, 10);
}

void string_to_number_negative_decimal(void **state)
{
    int i;
    string_to_number("-42", &i);

    assert_int_equal(i, -42);
}

void string_to_number_invalid_value(void **state)
{
    int i;
    return_type result = string_to_number("foo", &i);

    assert_int_equal(result, RETURN_INVALID_ARGUMENT);
}

void string_to_number_hexadecimal(void **state)
{
    int i;
    string_to_number("0xBADBEEF", &i);

    assert_int_equal(i, 0xBADBEEF);
}

void string_to_number_binary(void **state)
{
    int i;
    string_to_number("0b00101010", &i);

    assert_int_equal(i, 42);
}
