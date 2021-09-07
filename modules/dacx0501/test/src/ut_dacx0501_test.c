/*
 * ut_dacx0501_test.c
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
#include "dacx0501.h"
#include "cevent.h"

TEST_GROUP(dacx0501);

ut_i2c_driver_mock_tst  drv_mock_st;
i2c_driver_if_tst*      drv_pst = (i2c_driver_if_tst *)&drv_mock_st;

const cevent_tst*		i2c_master_events_apst[8];
i2c_master_tst			i2c_master_st;

const cevent_tst*		dacx0501_events_apst[12];
dacx0501_tst			dacx0501_st;


dacx0501_tst *self = 	&dacx0501_st; // This is necessary for macros like dacx0501_READ_PERIOD to work here

chsm_tst* hsm_apst[] =  {
	(chsm_tst*)(&i2c_master_st),
	(chsm_tst*)(&dacx0501_st),
	NULL};

uint8_t 				pool_buff_au8[1024];
cpool_tst				pool_ast[1];

crf_tst					crf;

const cevent_tst*       events_apst[4];
cqueue_tst              q_st;


void dacx0501_send(chsm_tst *self, const cevent_tst *e_pst)
{
    //printf("\n%s\n", __FUNCTION__);
	switch(e_pst->sig)
	{
		case SIG_DACX0501_ERROR:
			CRF_POST(e_pst, &q_st);
			break;

		default:
			printf("*");
			CRF_POST(e_pst, &i2c_master_st);
	}
}

static void tick_ms(uint32_t tick_cnt_u32)
{
	while(tick_cnt_u32--)
	{
		drv_mock_st.tick(&drv_mock_st);

		CRF_POST(&sys_event_tick_1ms_st, &dacx0501_st);

		while(CRF_STEP())
		{
			printf("|");
		}
	}
}

static void tick_ms_crf_only(uint32_t tick_cnt_u32)
{
	while(tick_cnt_u32--)
	{
		CRF_POST(&sys_event_tick_1ms_st, &dacx0501_st);

		while(CRF_STEP())
		{
			printf("|");
		}
	}
}

TEST_SETUP(dacx0501)
{
    memset(events_apst, 0, sizeof events_apst);
    memset(&q_st, 0, sizeof q_st);
    memset(&drv_mock_st, 0, sizeof drv_mock_st);
    memset(&i2c_master_st, 0, sizeof i2c_master_st);
    memset(&crf, 0, sizeof crf);
    memset(&i2c_master_events_apst, 0, sizeof i2c_master_events_apst);
    memset(&pool_buff_au8, 0, sizeof pool_buff_au8);
    memset(&pool_ast, 0, sizeof pool_ast);
    memset(&dacx0501_st, 0, sizeof dacx0501_st);

	cpool_init(pool_ast+0, pool_buff_au8, 24, 16);

    cqueue_init(&q_st, events_apst, 4);

    ut_i2c_driver_mock_init(&drv_mock_st);

	i2c_master_st.config_st.driver_pst = drv_pst;
	chsm_ctor(&i2c_master_st.super, i2c_master_top, i2c_master_events_apst, 4, 4);
	chsm_ctor(&dacx0501_st.super, dacx0501_top, dacx0501_events_apst, 8, 4);
	
	dacx0501_st.config_st = (dacx0501_cfg_tst){
		.address_u8 = 			0x12,
		.read_timeout_ms_u16 =	10,
		};
	dacx0501_st.super.send = dacx0501_send;

	crf_init(&crf, hsm_apst, pool_ast, 1);
	chsm_init((chsm_tst *)&i2c_master_st);
	chsm_init((chsm_tst *)&dacx0501_st);
}

TEST_TEAR_DOWN(dacx0501)
{
}

/*
 * Just call setup
 */
TEST(dacx0501, init)
{
	//tick_ms(10);
}

/* write_one_sample:
 * Send one sample to the mock device.
 */
TEST(dacx0501, write_one_sample)
{
	    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20
    };

	
	dacx0501_sample_tst* s_pst;

    drv_mock_st.slave_pst = &dev_st;

	tick_ms(5);

	s_pst = CRF_NEW(SIG_DACX0501_SAMPLE);
	TEST_ASSERT_NOT_NULL(s_pst);
	TEST_ASSERT_EQUAL(SIG_DACX0501_SAMPLE, s_pst->super.sig);

	s_pst->sample_u16 = 0xabcd;

	CRF_POST(s_pst, &dacx0501_st);
	
	tick_ms(4);

	uint8_t expected_rx_data_au8[4] = {0x08, 0xab, 0xcd, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 3);
}

/* write_two_samples:
 * Send two samples to the mock device.
 */
TEST(dacx0501, write_two_samples)
{
	    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20
    };

	
	dacx0501_sample_tst* s_pst;

    drv_mock_st.slave_pst = &dev_st;

	tick_ms(5);

	/* First sample */

	s_pst = CRF_NEW(SIG_DACX0501_SAMPLE);
	TEST_ASSERT_NOT_NULL(s_pst);
	TEST_ASSERT_EQUAL(SIG_DACX0501_SAMPLE, s_pst->super.sig);
	s_pst->sample_u16 = 0xabcd;
	CRF_POST(s_pst, &dacx0501_st);
	
	tick_ms(20);

	uint8_t expected_rx_data_au8[4] = {0x08, 0xab, 0xcd, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 3);

	/* Second sample */

	s_pst = CRF_NEW(SIG_DACX0501_SAMPLE);
	TEST_ASSERT_NOT_NULL(s_pst);
	TEST_ASSERT_EQUAL(SIG_DACX0501_SAMPLE, s_pst->super.sig);
	s_pst->sample_u16 = 0x4567;
	CRF_POST(s_pst, &dacx0501_st);
	
	tick_ms(20);

	uint8_t expected_rx_data2_au8[4] = {0x08, 0x45, 0x67, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data2_au8, dev_st.rx_data_au8+3, 3);
}

/* write_one_sample_address_nack:
 * Send one sample to the mock device but with wrong address. Check that an
 * error event is produced.
 */
TEST(dacx0501, write_one_sample_address_nack)
{
	    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x13,
        .nack_idx_u16 = 20
    };

	dacx0501_sample_tst* s_pst;

    drv_mock_st.slave_pst = &dev_st;

	tick_ms(5);

	s_pst = CRF_NEW(SIG_DACX0501_SAMPLE);
	TEST_ASSERT_NOT_NULL(s_pst);
	TEST_ASSERT_EQUAL(SIG_DACX0501_SAMPLE, s_pst->super.sig);

	s_pst->sample_u16 = 0xabcd;

	CRF_POST(s_pst, &dacx0501_st);
	
	tick_ms(4);

	const cevent_tst* e_pst;

	e_pst = q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(e_pst);
	TEST_ASSERT_EQUAL(SIG_DACX0501_ERROR, e_pst->sig);
}

/* write_one_sample_data_nack:
 * Send one sample to the mock device but generate a data nack. Check that an
 * error event is produced.
 */
TEST(dacx0501, write_one_sample_data_nack)
{
	    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 1
    };

	dacx0501_sample_tst* s_pst;

    drv_mock_st.slave_pst = &dev_st;

	tick_ms(5);

	s_pst = CRF_NEW(SIG_DACX0501_SAMPLE);
	TEST_ASSERT_NOT_NULL(s_pst);
	TEST_ASSERT_EQUAL(SIG_DACX0501_SAMPLE, s_pst->super.sig);

	s_pst->sample_u16 = 0xabcd;

	CRF_POST(s_pst, &dacx0501_st);
	
	tick_ms(4);

	const cevent_tst* e_pst;

	e_pst = q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(e_pst);
	TEST_ASSERT_EQUAL(SIG_DACX0501_ERROR, e_pst->sig);
}

/* write_one_sample_timeout:
 * Send one sample to the mock device but don't let the mock driver process the data. 
 * Check that an error event is produced.
 */
TEST(dacx0501, write_one_sample_timeout)
{
	    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20
    };

	dacx0501_sample_tst* s_pst;

    drv_mock_st.slave_pst = &dev_st;

	tick_ms(5);

	s_pst = CRF_NEW(SIG_DACX0501_SAMPLE);
	TEST_ASSERT_NOT_NULL(s_pst);
	TEST_ASSERT_EQUAL(SIG_DACX0501_SAMPLE, s_pst->super.sig);

	s_pst->sample_u16 = 0xabcd;

	CRF_POST(s_pst, &dacx0501_st);

	const cevent_tst* e_pst;

	tick_ms_crf_only(10);

	e_pst = q_st.get(&q_st);
	TEST_ASSERT_NULL(e_pst);

	tick_ms_crf_only(1);
	
	e_pst = q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(e_pst);
	TEST_ASSERT_EQUAL(SIG_DACX0501_ERROR, e_pst->sig);
}

TEST_GROUP_RUNNER(dacx0501)
{
	RUN_TEST_CASE(dacx0501, init);

	RUN_TEST_CASE(dacx0501, write_one_sample);
	RUN_TEST_CASE(dacx0501, write_two_samples);
	RUN_TEST_CASE(dacx0501, write_one_sample_address_nack);
	RUN_TEST_CASE(dacx0501, write_one_sample_data_nack);
	RUN_TEST_CASE(dacx0501, write_one_sample_timeout);
	//RUN_TEST_CASE(dacx0501, init);
	//RUN_TEST_CASE(dacx0501, init);
	//RUN_TEST_CASE(dacx0501, init);
	//RUN_TEST_CASE(dacx0501, init);
	//RUN_TEST_CASE(dacx0501, init);

}
