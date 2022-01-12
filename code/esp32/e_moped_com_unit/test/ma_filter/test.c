#include <unity.h>
#include "ma_filter.h"

void test_init()
{
    ma_filter_int_t *test_filter_i = ma_init_int(false);
    ma_filter_float_t *test_filter_f = ma_init_float(false);

    TEST_ASSERT_NOT_NULL(test_filter_i);
    TEST_ASSERT_NOT_NULL(test_filter_f);
    ma_free_filter_int(test_filter_i);
    ma_free_filter_float(test_filter_f);
}

void test_add_value_int()
{
    ma_filter_int_t *test_filter_i = ma_init_int(false);

    ma_add_value_int(test_filter_i, 25);
    TEST_ASSERT_EQUAL(1, test_filter_i->val_counter);
    TEST_ASSERT_EQUAL(25, ma_get_avg_int(test_filter_i));
    ma_add_value_int(test_filter_i, 27);
    TEST_ASSERT_EQUAL(26, ma_get_avg_int(test_filter_i));
    ma_add_value_int(test_filter_i, 26);
    TEST_ASSERT_EQUAL(26, ma_get_avg_int(test_filter_i));
    ma_add_value_int(test_filter_i, 29);
    TEST_ASSERT_EQUAL(27, ma_get_avg_int(test_filter_i));
    ma_add_value_int(test_filter_i, 27);

    TEST_ASSERT_EQUAL(27, ma_get_avg_int(test_filter_i));
    ma_free_filter_int(test_filter_i);
}

void test_add_value_float()
{
    ma_filter_float_t *test_filter_f = ma_init_float(false);
    float value_f = 2.5f;

    ma_add_value_float(test_filter_f, value_f);
    TEST_ASSERT_EQUAL(1, test_filter_f->val_counter);

    ma_add_value_float(test_filter_f, 4.5f);
    ma_add_value_float(test_filter_f, 5.25f);
    ma_add_value_float(test_filter_f, 1.5f);
    ma_add_value_float(test_filter_f, 1.5f);
    TEST_ASSERT_EQUAL(3.05f, ma_get_avg_float(test_filter_f));
    ma_free_filter_float(test_filter_f);
}

void test_add_value_float_has_mean()
{
    ma_filter_float_t *test_filter_f = ma_init_float(true);

    ma_add_value_float(test_filter_f, 5.25f);
    ma_add_value_float(test_filter_f, 1.58f);
    ma_add_value_float(test_filter_f, 1.45f);
    ma_add_value_float(test_filter_f, 4.5f);
    ma_add_value_float(test_filter_f, 5.25f);
    ma_add_value_float(test_filter_f, 1.5f);
    ma_add_value_float(test_filter_f, 9.7f);
    ma_add_value_float(test_filter_f, 10.5f);
    /*
    Total: 39,73 //all numbers added up
    1,45+10,5= 11,95 //biggest and smallest value
    39,73-11,95=27,78 //deduct that amount from total
    27,78/6=4,63 //sum after deviding with numbers of values.
    */
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 4.63f, ma_get_avg_float(test_filter_f));

    ma_add_value_float(test_filter_f, 6.2f);
    ma_add_value_float(test_filter_f, 3.7f);
    /*
    Total: 42,8 //all numbers added up
    1,45+10,5= 11,95 //biggest and smallest value
    42,8-11,95 =30,85 //deduct that amount from total
    30,85/6=5,14 //sum after deviding with numbers of values.
    */
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 5.14f, ma_get_avg_float(test_filter_f));

    ma_free_filter_float(test_filter_f);
}

void test_add_value_int_has_mean()
{
    ma_filter_int_t *test_filter_i = ma_init_int(true);

    ma_add_value_int(test_filter_i, 5);
    ma_add_value_int(test_filter_i, 1);
    ma_add_value_int(test_filter_i, 2);
    ma_add_value_int(test_filter_i, 4);
    ma_add_value_int(test_filter_i, 5);
    ma_add_value_int(test_filter_i, 3);
    ma_add_value_int(test_filter_i, 9);
    ma_add_value_int(test_filter_i, 10);

    TEST_ASSERT_EQUAL(5, ma_get_avg_int(test_filter_i));

    ma_add_value_int(test_filter_i, 6);
    ma_add_value_int(test_filter_i, 3);

    TEST_ASSERT_EQUAL(5, ma_get_avg_int(test_filter_i));

    ma_free_filter_int(test_filter_i);
}

void test_add_value_int_rolling()
{
    ma_filter_int_t *test_filter_i = ma_init_int(false);

    ma_add_value_int(test_filter_i, 5);
    ma_add_value_int(test_filter_i, 1);
    ma_add_value_int(test_filter_i, 2);
    ma_add_value_int(test_filter_i, 4);
    ma_add_value_int(test_filter_i, 5);
    ma_add_value_int(test_filter_i, 3);
    ma_add_value_int(test_filter_i, 9);
    ma_add_value_int(test_filter_i, 10); //tot 4,875 =5

    TEST_ASSERT_EQUAL(5, ma_get_avg_int(test_filter_i));

    ma_add_value_int(test_filter_i, 7);
    ma_add_value_int(test_filter_i, 6);
    ma_add_value_int(test_filter_i, 6); //tot 6,25 =5

    TEST_ASSERT_EQUAL(6, ma_get_avg_int(test_filter_i));

    ma_free_filter_int(test_filter_i);
}

void test_add_value_float_rolling()
{
    ma_filter_float_t *test_filter_f = ma_init_float(false);

    ma_add_value_float(test_filter_f, 5.25f);
    ma_add_value_float(test_filter_f, 1.58f);
    ma_add_value_float(test_filter_f, 1.45f);
    ma_add_value_float(test_filter_f, 4.5f);
    ma_add_value_float(test_filter_f, 5.25f);
    ma_add_value_float(test_filter_f, 1.5f);
    ma_add_value_float(test_filter_f, 9.7f);
    ma_add_value_float(test_filter_f, 10.5f);

    TEST_ASSERT_FLOAT_WITHIN(0.01f, 4.96f, ma_get_avg_float(test_filter_f));

    ma_add_value_float(test_filter_f, 6.2f);
    ma_add_value_float(test_filter_f, 8.7f);

    TEST_ASSERT_FLOAT_WITHIN(0.01f, 5.97f, ma_get_avg_float(test_filter_f));

    ma_free_filter_float(test_filter_f);
}

#ifdef DEVENV
int main(void)
{
#else
int app_main(void)
{
#endif
    UNITY_BEGIN();

    RUN_TEST(test_init);
    RUN_TEST(test_add_value_int);
    RUN_TEST(test_add_value_float);
    RUN_TEST(test_add_value_float_has_mean);
    RUN_TEST(test_add_value_int_has_mean);
    RUN_TEST(test_add_value_int_rolling);
    RUN_TEST(test_add_value_float_rolling);

    UNITY_END();

    return 0;
}