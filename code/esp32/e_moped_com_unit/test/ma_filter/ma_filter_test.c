#include <unity.h>
#include "ma_filter.h"

void test_ma_init()
{
    ma_filter_int_t *test_filter_i = ma_init_int(5);
    ma_filter_float_t *test_filter_f = ma_init_float(10);

    TEST_ASSERT_NOT_NULL(test_filter_i);
    TEST_ASSERT_EQUAL(5, test_filter_i->num_readings);
    TEST_ASSERT_NOT_NULL(test_filter_f);
    TEST_ASSERT_EQUAL(10, test_filter_f->num_readings);
    ma_free_filter_int(test_filter_i);
    ma_free_filter_float(test_filter_f);
}

void test_ma_add_value()
{
    ma_filter_int_t *test_filter_i = ma_init_int(5);
    ma_filter_float_t *test_filter_f = ma_init_float(10);

    float value_f = 2.5f;
    int value_i = 2;

    ma_add_value_int(test_filter_i, value_i);
    ma_add_value_float(test_filter_f, value_f);
    TEST_ASSERT_EQUAL(1, test_filter_i->val_counter);
    TEST_ASSERT_EQUAL(1, test_filter_f->val_counter);

    ma_add_value_int(test_filter_i, 2);
    ma_add_value_float(test_filter_f, 1.5f);
    ma_add_value_int(test_filter_i, 1);
    ma_add_value_float(test_filter_f, 4.5f);
    ma_add_value_int(test_filter_i, 3);
    ma_add_value_float(test_filter_f, 5.25f);
    ma_add_value_int(test_filter_i, 2);
    ma_add_value_float(test_filter_f, 1.5f);

    TEST_ASSERT_EQUAL(2, ma_get_avg_int(test_filter_i));
    TEST_ASSERT_EQUAL(3.05f, ma_get_avg_float(test_filter_f));
}

int app_main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_ma_init);
    RUN_TEST(test_ma_add_value);

    UNITY_END();

    return 0;
}