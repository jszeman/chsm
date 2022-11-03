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
#include <stdio.h>
#include "unity_fixture.h"
#include "crf.h"
#include "cbits.h"

TEST_GROUP(cb);

cevent_tst active_events_ast[32];
cevent_tst inactive_events_ast[32];

uint32_t active_bits_u32;
uint32_t inactive_bits_u32;
static void *last_user_param_pv;

cbits_tst cb_st;

#define TEST_POINTER (void *)0x12345678

void send(void *user_param_pv, const cevent_tst *e_pst)
{
    last_user_param_pv = user_param_pv;

    if (e_pst->sig & 0x100)
    {
        active_bits_u32 |= (1UL << e_pst->sig);
    }
    else if (e_pst->sig & 0x200)
    {
        inactive_bits_u32 |= (1UL << (e_pst->sig & 0xff));
    }
}

TEST_SETUP(cb)
{
    bool res_b;

    active_bits_u32 = 0;
    inactive_bits_u32 = 0;
    last_user_param_pv = NULL;

    for (int i=0; i<32; i++)
    {
        active_events_ast[i] = (cevent_tst){.sig = i | 0x100};
        inactive_events_ast[i] = (cevent_tst){.sig = i | 0x200};
    }

    cb_st = (cbits_tst){
        .config = {
            .active_events_pst = active_events_ast,
            .inactive_events_pst = inactive_events_ast,
            .user_param_pv = TEST_POINTER,
            .send_pft = send
        }
    };

    res_b = cbits_init(&cb_st, 0);
    TEST_ASSERT(res_b);
    TEST_ASSERT(cb_st.set_data);
}

TEST_TEAR_DOWN(cb)
{
}

/**
 * Make a rising edge on bit 0 and check that the send function was called with
 * the proper event.
 */
TEST(cb, init_without_send)
{
    bool res_b;

    cb_st.config.send_pft = NULL;
    res_b = cbits_init(&cb_st, 0);

    TEST_ASSERT_FALSE(res_b);
}

/**
 * Make a rising edge on bit 0 and check that the send function was called with
 * the proper event.
 */
TEST(cb, set_bit_0)
{
    cb_st.set_data(&cb_st, 1);

    TEST_ASSERT_EQUAL_HEX32(1, active_bits_u32);
    TEST_ASSERT_EQUAL_HEX32(TEST_POINTER, last_user_param_pv);
}

/**
 * Make a rising and falling edge on bit 0 and check that the send function was
 * called with the proper events.
 */
TEST(cb, set_clr_bit_0)
{
    cb_st.set_data(&cb_st, 1);
    cb_st.set_data(&cb_st, 0);

    TEST_ASSERT_EQUAL_HEX32(1, active_bits_u32);
    TEST_ASSERT_EQUAL_HEX32(1, inactive_bits_u32);
}

/**
 * Make a rising edge on bit 0 and 1 then check that the send function was called with
 * the proper events.
 */
TEST(cb, set_bit_01)
{
    cb_st.set_data(&cb_st, 3);

    TEST_ASSERT_EQUAL_HEX32(3, active_bits_u32);
}

/**
 * Make a rising edge on bit 0 and 1 then check that the send function was called with
 * the proper events.
 */
TEST(cb, set_bit_0_15)
{
    cb_st.set_data(&cb_st, 1 | (1 << 15));

    TEST_ASSERT_EQUAL_HEX32(1 | (1 << 15), active_bits_u32);
}

/**
 * Check effect of setting two bits, then clearning only one
 */
TEST(cb, set_2_clr_1)
{
    cb_st.set_data(&cb_st, 1);
    cb_st.set_data(&cb_st, 3);
    cb_st.set_data(&cb_st, 2);

    TEST_ASSERT_EQUAL_HEX32(3, active_bits_u32);
    TEST_ASSERT_EQUAL_HEX32(1, inactive_bits_u32);
}

/**
 * Check that events with sig == 0 are not sent
 */
TEST(cb, check_0_signal_not_sent)
{
    active_events_ast[0] = (cevent_tst){.sig = 0};

    cb_st.set_data(&cb_st, 1);

    TEST_ASSERT_EQUAL_HEX32(0, active_bits_u32);
}

/**
 * Check that the module works without active_sig pointer.
 */
TEST(cb, no_active_events)
{
    cb_st.config.active_events_pst = NULL;

    cb_st.set_data(&cb_st, 1);
    cb_st.set_data(&cb_st, 0);

    TEST_ASSERT_EQUAL_HEX32(0, active_bits_u32);
    TEST_ASSERT_EQUAL_HEX32(1, inactive_bits_u32);
}

/**
 * Check that the module works without inactive_sig pointer.
 */
TEST(cb, no_inactive_events)
{
    cb_st.config.inactive_events_pst = NULL;

    cb_st.set_data(&cb_st, 1);
    cb_st.set_data(&cb_st, 0);

    TEST_ASSERT_EQUAL_HEX32(1, active_bits_u32);
    TEST_ASSERT_EQUAL_HEX32(0, inactive_bits_u32);
}

TEST_GROUP_RUNNER(cb)
{
	RUN_TEST_CASE(cb, set_bit_0);
	RUN_TEST_CASE(cb, init_without_send);
	RUN_TEST_CASE(cb, set_clr_bit_0);
	RUN_TEST_CASE(cb, set_bit_01);
	RUN_TEST_CASE(cb, set_bit_0_15);
	RUN_TEST_CASE(cb, set_2_clr_1);
	RUN_TEST_CASE(cb, check_0_signal_not_sent);
	RUN_TEST_CASE(cb, no_active_events);
	RUN_TEST_CASE(cb, no_inactive_events);
	//RUN_TEST_CASE(cb, init);
	//RUN_TEST_CASE(cb, init);
}
