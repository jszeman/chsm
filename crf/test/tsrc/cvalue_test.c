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

static void send(cvalue_tst *self, const cevent_tst *e_pst)
{
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
            }
        };
}

TEST_TEAR_DOWN(cv)
{
}

TEST(cv, init)
{
	TEST_ASSERT(0);
}

TEST_GROUP_RUNNER(cv)
{
	RUN_TEST_CASE(cv, init);

}
