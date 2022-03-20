/*
 * ut_lm73_test.c
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
#include "lm73.h"
#include "cevent.h"
#include "sys_if.h"

TEST_GROUP(lm73);

ut_i2c_driver_mock_tst  drv_mock_st;
i2c_driver_if_tst*      drv_pst = (i2c_driver_if_tst *)&drv_mock_st;

const cevent_tst*		i2c_master_events_apst[8];
i2c_master_tst			i2c_master_st;

const cevent_tst*		lm73_events_apst[8];
lm73_tst				lm73_st;


lm73_tst *self = 		&lm73_st; // This is necessary for macros like LM73_READ_PERIOD to work here

chsm_tst*				hsm_apst[] = {
	(chsm_tst*)(&i2c_master_st),
	(chsm_tst*)(&lm73_st),
	NULL};

uint8_t 				pool_buff_au8[1024];
cpool_tst				pool_ast[1];

crf_tst					crf;

const cevent_tst*       events_apst[4];
cqueue_tst              q_st;

const cevent_tst		tick_1ms_st = {.sig = SIG_SYS_TICK_1ms};

static void tick_ms(uint32_t tick_cnt_u32)
{
	while(tick_cnt_u32--)
	{
		drv_mock_st.tick(&drv_mock_st);

		CRF_POST(&tick_1ms_st, &lm73_st);

		while(CRF_STEP())
		{
			printf("|");
		}
	}
}

void lm73_send(chsm_tst *self, const cevent_tst *e_pst)
{
    //printf("\n%s\n", __FUNCTION__);
	switch(e_pst->sig)
	{
		case SIG_LM73_TEMP:
		case SIG_LM73_ONLINE:
		case SIG_LM73_OFFLINE:
			CRF_POST(e_pst, &q_st);
			break;

		default:
			CRF_POST(e_pst, &i2c_master_st);
	}
}


TEST_SETUP(lm73)
{
    memset(events_apst, 0, sizeof events_apst);
    memset(&q_st, 0, sizeof q_st);
    memset(&drv_mock_st, 0, sizeof drv_mock_st);
    memset(&i2c_master_st, 0, sizeof i2c_master_st);
    memset(&crf, 0, sizeof crf);
    memset(&i2c_master_events_apst, 0, sizeof i2c_master_events_apst);
    memset(&pool_buff_au8, 0, sizeof pool_buff_au8);
    memset(&pool_ast, 0, sizeof pool_ast);
    memset(&lm73_st, 0, sizeof lm73_st);

	cpool_init(pool_ast+0, pool_buff_au8, 24, 16);

    cqueue_init(&q_st, events_apst, 4);

    ut_i2c_driver_mock_init(&drv_mock_st);

	i2c_master_st.config_st.driver_pst = drv_pst;
	chsm_ctor(&i2c_master_st.super, i2c_master_top, i2c_master_events_apst, 4, 4);
	chsm_ctor(&lm73_st.super, lm73_top, lm73_events_apst, 8, 0);
	
	lm73_st.config_st = (lm73_cfg_tst){
		.address_u8 = 			0x12,
		.id_u16 = 				0xabcd,
		.period_ms_u16 = 		100,
		.max_error_cnt_u16 = 	5
		};

	CRF_SEND_FUNC(&lm73_st) = lm73_send;

	crf_init(&crf, hsm_apst, pool_ast, 1);
	chsm_init((chsm_tst *)&i2c_master_st);
	chsm_init((chsm_tst *)&lm73_st);
}

TEST_TEAR_DOWN(lm73)
{
}

/*
 * Just call setup
 */
TEST(lm73, init)
{
}

/* read_temp_twice:
 * Setup the mock device to send the id bytes and two temperature reading (-25 and 25) to the master.
 * Check, that two temperature events were sent
 */
TEST(lm73, read_temp_twice)
{
	    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0x01, 0x90, 0xf3, 0x80, 0x0c, 0x80}
    };

	
	const lm73_temp_tst* e_pst;
	const lm73_status_tst* s_pst;

    drv_mock_st.slave_pst = &dev_st;

	tick_ms(100);

	s_pst = (lm73_status_tst*)q_st.get(&q_st);
	TEST_ASSERT(s_pst);
	TEST_ASSERT_EQUAL(SIG_LM73_ONLINE, s_pst->super.sig);

	e_pst = (lm73_temp_tst*)q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_LM73_TEMP, e_pst->super.sig);
	TEST_ASSERT_EQUAL(-25, e_pst->temp_C_i32);

	tick_ms(LM73_READ_PERIOD+1);

	uint8_t expected_au8[4] = {7, 0, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_au8, dev_st.rx_data_au8, 4);
	TEST_ASSERT_EQUAL(2, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(6, dev_st.tx_idx_u16);

	e_pst = (lm73_temp_tst*)q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_LM73_TEMP, e_pst->super.sig);
	TEST_ASSERT_EQUAL(25, e_pst->temp_C_i32);
}

/* read_id_retry:
 * Check that a failed ID read will be repeated after the configured time.
 */
TEST(lm73, read_id_retry)
{
	    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x13,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0x01, 0x90, 0xf3, 0x80, 0x0c, 0x80}
    };

	const lm73_status_tst* s_pst;

    drv_mock_st.slave_pst = &dev_st;

	tick_ms(10);

	s_pst = (lm73_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NULL(s_pst);

	dev_st.address_u8 = 0x12;
	
	tick_ms(LM73_RETRY_TIMEOUT-10);
	
	s_pst = (lm73_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NULL(s_pst);
	
	tick_ms(10);
	
	s_pst = (lm73_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(s_pst);
	TEST_ASSERT_EQUAL(SIG_LM73_ONLINE, s_pst->super.sig);
}

/* read_id_retry_bad_id:
 * Check that a failed ID read will be repeated after the configured time if the data sent back
 * by the slave is not 0x190 for the first try.
 */
TEST(lm73, read_id_retry_bad_id)
{
	    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0x01, 0x91, 0x01, 0x90, 0x0c, 0x80}
    };

	const lm73_status_tst* s_pst;

    drv_mock_st.slave_pst = &dev_st;

	tick_ms(10);

	s_pst = (lm73_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NULL(s_pst);
	
	tick_ms(LM73_RETRY_TIMEOUT-10);
	
	s_pst = (lm73_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NULL(s_pst);
	
	tick_ms(10);
	
	s_pst = (lm73_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(s_pst);
	TEST_ASSERT_EQUAL(SIG_LM73_ONLINE, s_pst->super.sig);
}

/* go_offline:
 * Check that after reading temperature data fails for more than LM73_MAX_ERROR_COUNT times
 * the state machine goes offline and invalidates the temperature data.
 */
TEST(lm73, go_offline)
{
	    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0x01, 0x90, 0xf3, 0x80, 0x0c, 0x80}
    };

	const lm73_temp_tst* e_pst;
	const lm73_status_tst* s_pst;

    drv_mock_st.slave_pst = &dev_st;

	tick_ms(10);

	s_pst = (lm73_status_tst*)q_st.get(&q_st);
	TEST_ASSERT(s_pst);
	TEST_ASSERT_EQUAL(SIG_LM73_ONLINE, s_pst->super.sig);

	e_pst = (lm73_temp_tst*)q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_LM73_TEMP, e_pst->super.sig);
	TEST_ASSERT_EQUAL(-25, e_pst->temp_C_i32);

	tick_ms(LM73_READ_PERIOD + 1);

	e_pst = (lm73_temp_tst*)q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_LM73_TEMP, e_pst->super.sig);
	TEST_ASSERT_EQUAL(25, e_pst->temp_C_i32);
	
	TEST_ASSERT_TRUE(lm73_st.valid_b);
	
	dev_st.address_u8 = 0x13;
	
	tick_ms((LM73_READ_PERIOD + 1) * LM73_MAX_ERROR_COUNT + 1);

	s_pst = (lm73_status_tst*)q_st.get(&q_st);
	TEST_ASSERT(s_pst);
	TEST_ASSERT_EQUAL(SIG_LM73_OFFLINE, s_pst->super.sig);

	TEST_ASSERT_FALSE(lm73_st.valid_b);
}

/* triggered_read:
 * Check that it is possible to trigger a read before the read period timeout elapses.
 */
TEST(lm73, triggered_read)
{
	    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0x01, 0x90, 0xf3, 0x80, 0x0c, 0x80}
    };

	const cevent_tst read_event = {.sig = SIG_LM73_READ};

	
	const lm73_temp_tst* e_pst;
	const lm73_status_tst* s_pst;

    drv_mock_st.slave_pst = &dev_st;

	tick_ms(10);

	s_pst = (lm73_status_tst*)q_st.get(&q_st);
	TEST_ASSERT(s_pst);
	TEST_ASSERT_EQUAL(SIG_LM73_ONLINE, s_pst->super.sig);

	e_pst = (lm73_temp_tst*)q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_LM73_TEMP, e_pst->super.sig);
	TEST_ASSERT_EQUAL(-25, e_pst->temp_C_i32);

	tick_ms(1);

	CRF_POST(&read_event, &lm73_st);

	tick_ms(10);

	uint8_t expected_au8[4] = {7, 0, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_au8, dev_st.rx_data_au8, 4);
	TEST_ASSERT_EQUAL(2, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(6, dev_st.tx_idx_u16);

	e_pst = (lm73_temp_tst*)q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_LM73_TEMP, e_pst->super.sig);
	TEST_ASSERT_EQUAL(25, e_pst->temp_C_i32);
}

TEST_GROUP_RUNNER(lm73)
{
	RUN_TEST_CASE(lm73, init);
	RUN_TEST_CASE(lm73, read_temp_twice);
	RUN_TEST_CASE(lm73, read_id_retry);
	RUN_TEST_CASE(lm73, read_id_retry_bad_id);
	RUN_TEST_CASE(lm73, go_offline);
	RUN_TEST_CASE(lm73, triggered_read);
	//RUN_TEST_CASE(lm73, init);
	//RUN_TEST_CASE(lm73, init);
	//RUN_TEST_CASE(lm73, init);
	//RUN_TEST_CASE(lm73, init);
	//RUN_TEST_CASE(lm73, init);
	//RUN_TEST_CASE(lm73, init);
	//RUN_TEST_CASE(lm73, init);
	//RUN_TEST_CASE(lm73, init);
	//RUN_TEST_CASE(lm73, init);
	//RUN_TEST_CASE(lm73, init);

}
