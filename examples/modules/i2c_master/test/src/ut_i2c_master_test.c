/*
 * ut_i2c_driver_mock_test.c
 *
 *  Created on: 2020. dec. 19.
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

TEST_GROUP(i2c_master);

ut_i2c_driver_mock_tst  drv_mock_st;
i2c_driver_if_tst*      drv_pst = (i2c_driver_if_tst *)&drv_mock_st;

const cevent_tst*		i2c_master_events_apst[8];
i2c_master_tst			i2c_master_st;

chsm_tst*				hsm_apst[] = {(chsm_tst*)(&i2c_master_st), NULL};

uint8_t 				pool_buff_au8[1024];
cpool_tst				pool_ast[1];

crf_tst					crf;

const cevent_tst*       events_apst[4];
cqueue_tst              q_st;




TEST_SETUP(i2c_master)
{
    memset(events_apst, 0, sizeof events_apst);
    memset(&q_st, 0, sizeof q_st);
    memset(&drv_mock_st, 0, sizeof drv_mock_st);
    memset(&i2c_master_st, 0, sizeof i2c_master_st);
    memset(&crf, 0, sizeof crf);
    memset(&i2c_master_events_apst, 0, sizeof i2c_master_events_apst);
    memset(&pool_buff_au8, 0, sizeof pool_buff_au8);
    memset(&pool_ast, 0, sizeof pool_ast);

	cpool_init(pool_ast+0, pool_buff_au8, 24, 16);

    cqueue_init(&q_st, events_apst, 4);

    ut_i2c_driver_mock_init(&drv_mock_st);

	i2c_master_st.config_st.driver_pst = drv_pst;
	chsm_ctor(&i2c_master_st.super, i2c_master_top, i2c_master_events_apst, 4, 4);


	chsm_init((chsm_tst *)&i2c_master_st);
	crf_init(&crf, hsm_apst, pool_ast, 1);
}

TEST_TEAR_DOWN(i2c_master)
{
}

/*
 * Just call setup
 */
TEST(i2c_master, init)
{
}

TEST(i2c_master, send_1b)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 1
    };

    drv_mock_st.slave_pst = &dev_st;

	i2c_transaction_tst*	t_pst = crf.new_event(&crf, sizeof(i2c_transaction_tst) + 2);

	TEST_ASSERT(t_pst);

	*t_pst = (i2c_transaction_tst){
		.super.sig = SIG_I2C_W_TRANSACTION,
		.target_q_pst = &q_st,
		.read_cnt_u16 = 0,
		.write_cnt_u16 = 1,
		.slave_addr_u16 = 0x12,
	};
	t_pst->write_data_au8[0] = 0x5a;
	t_pst->write_data_au8[1] = 0x6b;

	CRF_POST(t_pst, &i2c_master_st);

	while(CRF_STEP());

	uint8_t expected_au8[4] = {0x5a, 0, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_au8, dev_st.rx_data_au8, 2);
}



TEST_GROUP_RUNNER(i2c_master)
{
	RUN_TEST_CASE(i2c_master, init);
	RUN_TEST_CASE(i2c_master, send_1b);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);

}
