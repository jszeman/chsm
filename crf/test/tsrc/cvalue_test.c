/*
 * canopen_test.c
 *
 *  Created on: 2019. jan. 25.
 *      Author: jszeman
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "unity_fixture.h"
#include "crf.h"
#include "cvalue.h"


TEST_GROUP(cv);

const cevent_tst low_st = {.sig = 0x100};
const cevent_tst in_st = {.sig = 0x101};
const cevent_tst high_st = {.sig = 0x102};

uint32_t low_cnt_u32;
uint32_t in_cnt_u32;
uint32_t high_cnt_u32;


static void *last_user_param_pv;

#define TEST_POINTER ((void *)0x12345678)

static void send(void *user_param_pv, const cevent_tst *e_pst)
{
    last_user_param_pv = user_param_pv;
    
    switch(e_pst->sig)
    {
        case 0x100: low_cnt_u32++; break;
        case 0x101: in_cnt_u32++; break;
        case 0x102: high_cnt_u32++; break;
    }
}

cvalue_tst  cv_st;

TEST_SETUP(cv)
{
        cv_st = (cvalue_tst){
            .config = {
                .low_limit_event_pst = &low_st,
                .in_range_event_pst = &in_st,
                .high_limit_event_pst = &high_st,
                .low_limit_i32 = 10,
                .high_limit_i32 = 20,
                .filter_count_u32 = 10,
                .send = send,
                .user_param_pv = TEST_POINTER,
            }
        };

        last_user_param_pv = NULL;

        bool res = cvalue_init(&cv_st);

        TEST_ASSERT(res);
        TEST_ASSERT(cv_st.set_value);

        low_cnt_u32 = 0;
        in_cnt_u32 = 0;
        high_cnt_u32 = 0;
}

TEST_TEAR_DOWN(cv)
{
}

/* Check that setting a value in range less times than the filter count
 * does not trigger any events.
 */
TEST(cv, set_less_than_filter_count_values_in_range)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32 - 1; i++)
    {
        cv_st.set_value(&cv_st, 15);
    }

    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check that setting a low value less times than the filter count
 * does not trigger any events.
 */
TEST(cv, set_less_than_filter_count_values_low)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32 - 1; i++)
    {
        cv_st.set_value(&cv_st, 5);
    }

    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check that setting a high value less times than the filter count
 * does not trigger any events.
 */
TEST(cv, set_less_than_filter_count_values_high)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32 - 1; i++)
    {
        cv_st.set_value(&cv_st, 25);
    }

    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check that calling set_value with a value in range will trigger
 * the in_range event once.
 */
TEST(cv, set_filter_count_values_in_range)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32; i++)
    {
        cv_st.set_value(&cv_st, 15);
    }

    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(1, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);

    TEST_ASSERT_EQUAL_HEX64((uint64_t)TEST_POINTER, (uint64_t)last_user_param_pv);
}

/* Check that calling set_value with a low value in range will trigger
 * the low event once.
 */
TEST(cv, set_filter_count_values_low)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32; i++)
    {
        cv_st.set_value(&cv_st, 5);
    }

    TEST_ASSERT_EQUAL(1, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check that calling set_value with a high value in range will trigger
 * the low event once.
 */
TEST(cv, set_filter_count_values_high)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32; i++)
    {
        cv_st.set_value(&cv_st, 25);
    }

    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(1, high_cnt_u32);
}

/* Check that calling set_value with an in range value more than the filter_count
 * will trigger the low event only once.
 */
TEST(cv, set_more_than_filter_count_values_in_range)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32*5; i++)
    {
        cv_st.set_value(&cv_st, 15);
    }

    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(1, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check that calling set_value with a low value more than the filter_count
 * will trigger the low event only once.
 */
TEST(cv, set_more_than_filter_count_values_low)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32*5; i++)
    {
        cv_st.set_value(&cv_st, 5);
    }

    TEST_ASSERT_EQUAL(1, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check that calling set_value with a high value more than the filter_count
 * will trigger the low event only once.
 */
TEST(cv, set_more_than_filter_count_values_high)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32*5; i++)
    {
        cv_st.set_value(&cv_st, 25);
    }

    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(1, high_cnt_u32);
}

/* Check that in_range event is only triggered if filter_count consecutive
 * in range values were set.
 */
TEST(cv, filter_count_in_range_values_with_low_noise)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32-1; i++)
    {
        cv_st.set_value(&cv_st, 15);
    }

    cv_st.set_value(&cv_st, 5);
    cv_st.set_value(&cv_st, 15);

    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check that in_range event is only triggered if filter_count consecutive
 * in range values were set.
 */
TEST(cv, filter_count_in_range_values_with_high_noise)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32-1; i++)
    {
        cv_st.set_value(&cv_st, 15);
    }

    cv_st.set_value(&cv_st, 25);
    cv_st.set_value(&cv_st, 15);

    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check that low_limit event is only triggered if filter_count consecutive
 * low values were set.
 */
TEST(cv, filter_count_low_values_with_high_noise)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32-1; i++)
    {
        cv_st.set_value(&cv_st, 5);
    }

    cv_st.set_value(&cv_st, 15);
    cv_st.set_value(&cv_st, 5);

    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check that hi_limit event is only triggered if filter_count consecutive
 * high values were set.
 */
TEST(cv, filter_count_hig_values_with_high_noise)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32-1; i++)
    {
        cv_st.set_value(&cv_st, 25);
    }

    cv_st.set_value(&cv_st, 15);
    cv_st.set_value(&cv_st, 25);

    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check that in_range event is not triggered again, if there
 * are a few values out of range
 */
TEST(cv, no_multiple_in_range_event_after_few_low_values)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 15);
    }
    
    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(1, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);

    for (uint32_t i=0; i<cv_st.config.filter_count_u32-1; i++)
    {
        cv_st.set_value(&cv_st, 5);
    }

    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(1, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);

    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 15);
    }

    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(1, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check that in_range event is not triggered again, if there
 * are a few values out of range
 */
TEST(cv, no_multiple_in_range_event_after_few_high_values)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 15);
    }
    
    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(1, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);

    for (uint32_t i=0; i<cv_st.config.filter_count_u32-1; i++)
    {
        cv_st.set_value(&cv_st, 25);
    }

    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(1, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);

    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 15);
    }

    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(1, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check that low_limit event is not triggered again, if there
 * are a few values out of range
 */
TEST(cv, no_multiple_low_limit_event_after_few_high_values)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 5);
    }
    
    TEST_ASSERT_EQUAL(1, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);


    for (uint32_t i=0; i<cv_st.config.filter_count_u32-1; i++)
    {
        cv_st.set_value(&cv_st, 25);
    }

    TEST_ASSERT_EQUAL(1, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);

    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 5);
    }

    TEST_ASSERT_EQUAL(1, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check that low_limit event is not triggered again, if there
 * are a few values out of range
 */
TEST(cv, no_multiple_low_limit_event_after_few_in_range_values)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 5);
    }
    
    TEST_ASSERT_EQUAL(1, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);


    for (uint32_t i=0; i<cv_st.config.filter_count_u32-1; i++)
    {
        cv_st.set_value(&cv_st, 15);
    }

    TEST_ASSERT_EQUAL(1, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);

    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 5);
    }

    TEST_ASSERT_EQUAL(1, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check that high_limit event is not triggered again, if there
 * are a few values out of range
 */
TEST(cv, no_multiple_high_limit_event_after_few_in_range_values)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 25);
    }
    
    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(1, high_cnt_u32);


    for (uint32_t i=0; i<cv_st.config.filter_count_u32-1; i++)
    {
        cv_st.set_value(&cv_st, 15);
    }
    
    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(1, high_cnt_u32);

    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 25);
    }
    
    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(1, high_cnt_u32);
}

/* Check that high_limit event is not triggered again, if there
 * are a few values out of range
 */
TEST(cv, no_multiple_high_limit_event_after_few_low_values)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 25);
    }
    
    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(1, high_cnt_u32);


    for (uint32_t i=0; i<cv_st.config.filter_count_u32-1; i++)
    {
        cv_st.set_value(&cv_st, 5);
    }
    
    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(1, high_cnt_u32);

    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 25);
    }
    
    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(1, high_cnt_u32);
}

/* Check low to in_range transition
 */
TEST(cv, low_to_in_range)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 5);
    }
    
    TEST_ASSERT_EQUAL(1, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);


    for (uint32_t i=0; i<cv_st.config.filter_count_u32; i++)
    {
        cv_st.set_value(&cv_st, 15);
    }
    
    TEST_ASSERT_EQUAL(1, low_cnt_u32);
    TEST_ASSERT_EQUAL(1, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check low to in_range transition
 */
TEST(cv, low_to_high)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 5);
    }
    
    TEST_ASSERT_EQUAL(1, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);


    for (uint32_t i=0; i<cv_st.config.filter_count_u32; i++)
    {
        cv_st.set_value(&cv_st, 25);
    }
    
    TEST_ASSERT_EQUAL(1, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(1, high_cnt_u32);
}

/* Check in_range to low transition
 */
TEST(cv, in_to_low)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 15);
    }
    
    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(1, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);


    for (uint32_t i=0; i<cv_st.config.filter_count_u32; i++)
    {
        cv_st.set_value(&cv_st, 5);
    }
    
    TEST_ASSERT_EQUAL(1, low_cnt_u32);
    TEST_ASSERT_EQUAL(1, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);
}

/* Check in_range to high transition
 */
TEST(cv, in_to_high)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 15);
    }
    
    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(1, in_cnt_u32);
    TEST_ASSERT_EQUAL(0, high_cnt_u32);


    for (uint32_t i=0; i<cv_st.config.filter_count_u32; i++)
    {
        cv_st.set_value(&cv_st, 25);
    }
    
    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(1, in_cnt_u32);
    TEST_ASSERT_EQUAL(1, high_cnt_u32);
}

/* Check high to in_range transition
 */
TEST(cv, high_to_in_range)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 25);
    }
    
    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(1, high_cnt_u32);


    for (uint32_t i=0; i<cv_st.config.filter_count_u32; i++)
    {
        cv_st.set_value(&cv_st, 15);
    }
    
    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(1, in_cnt_u32);
    TEST_ASSERT_EQUAL(1, high_cnt_u32);
}

/* Check high to low transition
 */
TEST(cv, high_to_low)
{
    for (uint32_t i=0; i<cv_st.config.filter_count_u32*2; i++)
    {
        cv_st.set_value(&cv_st, 25);
    }
    
    TEST_ASSERT_EQUAL(0, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(1, high_cnt_u32);


    for (uint32_t i=0; i<cv_st.config.filter_count_u32; i++)
    {
        cv_st.set_value(&cv_st, 5);
    }
    
    TEST_ASSERT_EQUAL(1, low_cnt_u32);
    TEST_ASSERT_EQUAL(0, in_cnt_u32);
    TEST_ASSERT_EQUAL(1, high_cnt_u32);
}

TEST_GROUP_RUNNER(cv)
{
	RUN_TEST_CASE(cv, set_less_than_filter_count_values_in_range);
	RUN_TEST_CASE(cv, set_less_than_filter_count_values_low);
	RUN_TEST_CASE(cv, set_less_than_filter_count_values_high);
	RUN_TEST_CASE(cv, set_filter_count_values_in_range);
	RUN_TEST_CASE(cv, set_filter_count_values_low);
	RUN_TEST_CASE(cv, set_filter_count_values_high);
	RUN_TEST_CASE(cv, set_more_than_filter_count_values_in_range);
	RUN_TEST_CASE(cv, set_more_than_filter_count_values_low);
	RUN_TEST_CASE(cv, set_more_than_filter_count_values_high);
	RUN_TEST_CASE(cv, filter_count_in_range_values_with_low_noise);
	RUN_TEST_CASE(cv, filter_count_in_range_values_with_high_noise);
	RUN_TEST_CASE(cv, filter_count_low_values_with_high_noise);
	RUN_TEST_CASE(cv, filter_count_hig_values_with_high_noise);
	RUN_TEST_CASE(cv, no_multiple_in_range_event_after_few_low_values);
	RUN_TEST_CASE(cv, no_multiple_in_range_event_after_few_high_values);
	RUN_TEST_CASE(cv, no_multiple_low_limit_event_after_few_high_values);
	RUN_TEST_CASE(cv, no_multiple_low_limit_event_after_few_in_range_values);
	RUN_TEST_CASE(cv, no_multiple_high_limit_event_after_few_in_range_values);
	RUN_TEST_CASE(cv, no_multiple_high_limit_event_after_few_low_values);
	RUN_TEST_CASE(cv, low_to_in_range);
	RUN_TEST_CASE(cv, low_to_high);
	RUN_TEST_CASE(cv, in_to_low);
	RUN_TEST_CASE(cv, in_to_high);
	RUN_TEST_CASE(cv, high_to_in_range);
	RUN_TEST_CASE(cv, high_to_low);
	//RUN_TEST_CASE(cv, init);
	//RUN_TEST_CASE(cv, init);
	//RUN_TEST_CASE(cv, init);
	//RUN_TEST_CASE(cv, init);
	//RUN_TEST_CASE(cv, init);
}
