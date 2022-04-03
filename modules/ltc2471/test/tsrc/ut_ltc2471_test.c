/*
 * ut_ltc2471_test.c
 *
 *  Created on: 2020. dec. 30.
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
#include "ut_i2c_driver_mock.h"
#include "i2c_master.h"
#include "ltc2471.h"
#include "cevent.h"

TEST_GROUP(ltc2471);

ut_i2c_driver_mock_tst  drv_mock_st;
i2c_driver_if_tst*      drv_pst = (i2c_driver_if_tst *)&drv_mock_st;

const cevent_tst*		i2c_master_events_apst[8];
i2c_master_tst			i2c_master_st;

const cevent_tst*		ltc2471_events_apst[12];
ltc2471_tst				ltc2471_st;


ltc2471_tst *self = 	&ltc2471_st; // This is necessary for macros like ltc2471_READ_PERIOD to work here

chsm_tst*				hsm_apst[] = {
	(chsm_tst*)(&i2c_master_st),
	(chsm_tst*)(&ltc2471_st),
	NULL};

uint8_t 				pool_buff_au8[1024];
cpool_tst				pool_ast[1];

crf_tst					crf;

const cevent_tst*       events_apst[4];
cqueue_tst              q_st;


void ltc2471_send(chsm_tst *self, const cevent_tst *e_pst)
{
    //printf("\n%s\n", __FUNCTION__);
	switch(e_pst->sig)
	{
		case SIG_LTC2471_SAMPLE:
			CRF_POST(e_pst, &q_st);
			break;

		default:
			CRF_POST(e_pst, &i2c_master_st);
	}
}

static void tick_ms(uint32_t tick_cnt_u32)
{
	while(tick_cnt_u32--)
	{
		drv_mock_st.tick(&drv_mock_st);

		CRF_POST(&sys_event_tick_1ms_st, &ltc2471_st);

		while(CRF_STEP())
		{
			printf("|");
		}
	}
}

TEST_SETUP(ltc2471)
{
    memset(events_apst, 0, sizeof events_apst);
    memset(&q_st, 0, sizeof q_st);
    memset(&drv_mock_st, 0, sizeof drv_mock_st);
    memset(&i2c_master_st, 0, sizeof i2c_master_st);
    memset(&crf, 0, sizeof crf);
    memset(&i2c_master_events_apst, 0, sizeof i2c_master_events_apst);
    memset(&pool_buff_au8, 0, sizeof pool_buff_au8);
    memset(&pool_ast, 0, sizeof pool_ast);
    memset(&ltc2471_st, 0, sizeof ltc2471_st);

	cpool_init(pool_ast+0, pool_buff_au8, 24, 16);

    cqueue_init(&q_st, events_apst, 4);

    ut_i2c_driver_mock_init(&drv_mock_st);

	i2c_master_st.config_st.driver_pst = drv_pst;
	chsm_ctor(&i2c_master_st.super, i2c_master_top, i2c_master_events_apst, 4, 4);
	chsm_ctor(&ltc2471_st.super, ltc2471_top, ltc2471_events_apst, 8, 4);
	
	ltc2471_st.config_st = (ltc2471_cfg_tst){
		.address_u8 = 			0x12,
		.read_timeout_ms_u16 =	10,
		};
	ltc2471_st.super.send = ltc2471_send;

	crf_init(&crf, hsm_apst, pool_ast, 1);
	chsm_init((chsm_tst *)&i2c_master_st);
	chsm_init((chsm_tst *)&ltc2471_st);
}

TEST_TEAR_DOWN(ltc2471)
{
}

/*
 * Just call setup
 */
TEST(ltc2471, init)
{
	//tick_ms(10);
}

/*
 * Read one sample
 */
TEST(ltc2471, read_one)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0x55, 0x66, 0x77, 0x88, 0x99, 0xaa}
    };

	
	const ltc2471_sample_tst* s_pst;

    drv_mock_st.slave_pst = &dev_st;

	tick_ms(14);

	s_pst = (ltc2471_sample_tst*)q_st.get(&q_st);
	TEST_ASSERT(s_pst);
	TEST_ASSERT_EQUAL(SIG_LTC2471_SAMPLE, s_pst->super.sig);
	TEST_ASSERT_EQUAL_HEX(0x5566, s_pst->sample_u16);
}

/*
 * Read two samples
 */
TEST(ltc2471, read_two)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0x55, 0x66, 0x77, 0x88, 0x99, 0xaa}
    };

	
	const ltc2471_sample_tst* s_pst;

    drv_mock_st.slave_pst = &dev_st;

	tick_ms(14);

	s_pst = (ltc2471_sample_tst*)q_st.get(&q_st);
	TEST_ASSERT(s_pst);
	TEST_ASSERT_EQUAL(SIG_LTC2471_SAMPLE, s_pst->super.sig);
	TEST_ASSERT_EQUAL_HEX(0x5566, s_pst->sample_u16);

	s_pst = (ltc2471_sample_tst*)q_st.get(&q_st);
	TEST_ASSERT_NULL(s_pst);

	tick_ms(14);

	s_pst = (ltc2471_sample_tst*)q_st.get(&q_st);
	TEST_ASSERT(s_pst);
	TEST_ASSERT_EQUAL(SIG_LTC2471_SAMPLE, s_pst->super.sig);
	TEST_ASSERT_EQUAL_HEX(0x7788, s_pst->sample_u16);
}



TEST_GROUP_RUNNER(ltc2471)
{
	RUN_TEST_CASE(ltc2471, init);

	RUN_TEST_CASE(ltc2471, read_one);
	RUN_TEST_CASE(ltc2471, read_two);
}
