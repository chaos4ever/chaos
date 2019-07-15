// Abstract: Entry point for the unit test program.
// Authors: Per Lundberg <per@chaosdev.io>
//
// © Copyright 1999 chaos development

#include "../test_helper.h"

extern int circular_queue_test_setup(void **state);

extern void test_circular_queue_enqueue_one(void **state);
extern void test_circular_queue_enqueue_until_full(void **state);
extern void test_circular_queue_enqueue_before_tail(void **state);
extern void test_circular_queue_enqueue_between_head_and_tail(void **state);

int main(void)
{
    const struct CMUnitTest mailbox_tests[] =
    {
        cmocka_unit_test_setup(test_circular_queue_enqueue_one, circular_queue_test_setup),
        cmocka_unit_test_setup(test_circular_queue_enqueue_until_full, circular_queue_test_setup),
        cmocka_unit_test_setup(test_circular_queue_enqueue_before_tail, circular_queue_test_setup),
        cmocka_unit_test_setup(test_circular_queue_enqueue_between_head_and_tail, circular_queue_test_setup)
    };

    cmocka_run_group_tests_name("circular_queue", mailbox_tests, NULL, NULL);
}
