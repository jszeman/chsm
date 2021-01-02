/*
 * ut_fram_test.c
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
#include "fram.h"
#include "cevent.h"

TEST_GROUP(fram);

ut_i2c_driver_mock_tst  drv_mock_st;
i2c_driver_if_tst*      drv_pst = (i2c_driver_if_tst *)&drv_mock_st;

const cevent_tst*		i2c_master_events_apst[8];
i2c_master_tst			i2c_master_st;

const cevent_tst*		fram_events_apst[8];
fram_tst				fram_st;


fram_tst *self = 		&fram_st; // This is necessary for macros like fram_READ_PERIOD to work here

chsm_tst*				hsm_apst[] = {
	(chsm_tst*)(&i2c_master_st),
	(chsm_tst*)(&fram_st),
	NULL};

uint8_t 				pool_buff_au8[1024];
cpool_tst				pool_ast[1];

crf_tst					crf;

const cevent_tst*       events_apst[4];
cqueue_tst              q_st;

void fram_send(chsm_tst *self, const cevent_tst *e_pst)
{
    //printf("\n%s\n", __FUNCTION__);
	switch(e_pst->sig)
	{
		default:
			CRF_POST(e_pst, &i2c_master_st);
	}
}

static void crf_run()
{
	while(CRF_STEP())
	{
		printf("|");
	}
}


TEST_SETUP(fram)
{
    memset(events_apst, 0, sizeof events_apst);
    memset(&q_st, 0, sizeof q_st);
    memset(&drv_mock_st, 0, sizeof drv_mock_st);
    memset(&i2c_master_st, 0, sizeof i2c_master_st);
    memset(&crf, 0, sizeof crf);
    memset(&i2c_master_events_apst, 0, sizeof i2c_master_events_apst);
    memset(&pool_buff_au8, 0, sizeof pool_buff_au8);
    memset(&pool_ast, 0, sizeof pool_ast);
    memset(&fram_st, 0, sizeof fram_st);

	cpool_init(pool_ast+0, pool_buff_au8, 24, 16);

    cqueue_init(&q_st, events_apst, 4);

    ut_i2c_driver_mock_init(&drv_mock_st);

	i2c_master_st.config_st.driver_pst = drv_pst;
	chsm_ctor(&i2c_master_st.super, i2c_master_top, i2c_master_events_apst, 4, 4);
	chsm_ctor(&fram_st.super, fram_top, fram_events_apst, 8, 0);
	
	fram_st.config_st = (fram_cfg_tst){
		.address_u8 = 			0x12,
		.chunk_size_u8 =		16,
		.size_u32 =				131072
		};
	fram_st.super.send = fram_send;

	crf_init(&crf, hsm_apst, pool_ast, 1);
	chsm_init((chsm_tst *)&i2c_master_st);
	chsm_init((chsm_tst *)&fram_st);
}

TEST_TEAR_DOWN(fram)
{
}

/*
 * Just call setup
 */
TEST(fram, init)
{
}

/* read_1b_from_page_0:
 * Trigger a 1 byte read from page 0 and check the result.
 */
TEST(fram, read_1b_from_page_0)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0xa5, 0x90, 0xf3, 0x80, 0x0c, 0x80},
		.rx_data_au8 = {0},
    };

	uint8_t buff_au8[4] = {0};

	mem_op_tst mem_op_st = {
		.super.sig = SIG_MEM_READ,
		.address_u32 = 0xf002,
		.buff_pu8 = buff_au8,
		.len_u32 = 1,
		.q_pst = &q_st,
	};

    drv_mock_st.slave_pst = &dev_st;

	CRF_POST(&mem_op_st, &fram_st);

	crf_run();

	uint8_t expected_buff_au8[4] = {0xa5, 0, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buff_au8, buff_au8, 4);

	uint8_t expected_rx_data_au8[4] = {0xf0, 0x02, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 4);

	TEST_ASSERT_EQUAL(2, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(1, dev_st.tx_idx_u16);

	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_READ_SUCCESS, ms_pst->super.sig);
}



TEST_GROUP_RUNNER(fram)
{
	RUN_TEST_CASE(fram, init);
	RUN_TEST_CASE(fram, read_1b_from_page_0);
	//RUN_TEST_CASE(fram, init);
	//RUN_TEST_CASE(fram, init);
	//RUN_TEST_CASE(fram, init);
	//RUN_TEST_CASE(fram, init);
	//RUN_TEST_CASE(fram, init);
	//RUN_TEST_CASE(fram, init);
	//RUN_TEST_CASE(fram, init);
	//RUN_TEST_CASE(fram, init);
	//RUN_TEST_CASE(fram, init);

}
