/*
 * ut_spi_driver_mock_test.c
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
#include "ut_spi_driver_mock.h"
#include "spi_master.h"

TEST_GROUP(spi_master);

ut_spi_driver_mock_tst  drv_mock_st;
spi_driver_if_tst*      drv_pst = (spi_driver_if_tst *)&drv_mock_st;

const cevent_tst*		spi_master_events_apst[8];
spi_master_tst			spi_master_st;

chsm_tst*				hsm_apst[] = {(chsm_tst*)(&spi_master_st), NULL};

uint8_t 				pool_buff_au8[1024];
cpool_tst				pool_ast[1];

crf_tst					crf;

const cevent_tst*       events_apst[4];
cqueue_tst              q_st;

static void drv_tick(uint32_t tick_cnt_u32)
{
    for (uint32_t i=0; i<tick_cnt_u32; i++)
    {
        drv_mock_st.tick(&drv_mock_st);

		while(CRF_STEP())
		{
			printf("|");
		}
    }
}

TEST_SETUP(spi_master)
{
    memset(events_apst, 0, sizeof events_apst);
    memset(&q_st, 0, sizeof q_st);
    memset(&drv_mock_st, 0, sizeof drv_mock_st);
    memset(&spi_master_st, 0, sizeof spi_master_st);
    memset(&crf, 0, sizeof crf);
    memset(&spi_master_events_apst, 0, sizeof spi_master_events_apst);
    memset(&pool_buff_au8, 0, sizeof pool_buff_au8);
    memset(&pool_ast, 0, sizeof pool_ast);

	cpool_init(pool_ast+0, pool_buff_au8, 24, 16);

    cqueue_init(&q_st, events_apst, 4);

    ut_spi_driver_mock_init(&drv_mock_st);

	spi_master_st.config_st.driver_pst = drv_pst;
	chsm_ctor(&spi_master_st.super, spi_master_top, spi_master_events_apst, 4, 4);

	chsm_init((chsm_tst *)&spi_master_st);
	crf_init(&crf, hsm_apst, pool_ast, 1);
}

TEST_TEAR_DOWN(spi_master)
{
}

/*
 * Just call setup
 */
TEST(spi_master, init)
{
}

/* send_1b:
 * Send one byte to the correct slave address and assert the following:
 * 	- The data was written to the device (via the mock driver)
 *  - The master sent back a success message (put it in the queue designated by the transaction)
 */
TEST(spi_master, send_1b)
{
    spi_mock_slave_device_tst dev_st = {0};

    drv_mock_st.slave_pst = &dev_st;
	drv_mock_st.slave_count_u16 = 1;

	uint8_t data_au8[4] = {0x5a, 0x6b, 0, 0};

	spi_transaction_tst*	t_pst = CRF_NEW(SIG_SPI_MASTER_TRANSACTION);

	TEST_ASSERT_NOT_NULL(t_pst);

	t_pst->target_q_pst = &q_st;
	t_pst->len_u16 = 1;
	t_pst->slave_idx_u8 = 0;
	t_pst->tx_data_pu8 = data_au8;

	TEST_ASSERT_EQUAL(SIG_SPI_MASTER_TRANSACTION, t_pst->super.sig);

	CRF_POST(t_pst, &spi_master_st);

	drv_tick(5);

	uint8_t expected_au8[4] = {0x5a, 0, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_au8, dev_st.rx_data_au8, 2);

	const cevent_tst* e_pst = q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_SPI_MASTER_RESULT_SUCCESS, e_pst->sig);
}

/* send_2b:
 * Send two bytes to the correct slave address and assert the following:
 * 	- The data was written to the device (via the mock driver)
 *  - The master sent back a success message (put it in the queue designated by the transaction)
 */
TEST(spi_master, send_2b)
{
    spi_mock_slave_device_tst dev_st = {0};

    drv_mock_st.slave_pst = &dev_st;
	drv_mock_st.slave_count_u16 = 1;

	uint8_t data_au8[4] = {0x5a, 0x6b, 0x7c, 0};

	spi_transaction_tst*	t_pst = CRF_NEW(SIG_SPI_MASTER_TRANSACTION);

	TEST_ASSERT_NOT_NULL(t_pst);

	t_pst->target_q_pst = &q_st;
	t_pst->len_u16 = 2;
	t_pst->slave_idx_u8 = 0;
	t_pst->tx_data_pu8 = data_au8;

	TEST_ASSERT_EQUAL(SIG_SPI_MASTER_TRANSACTION, t_pst->super.sig);

	CRF_POST(t_pst, &spi_master_st);

	drv_tick(5);

	uint8_t expected_au8[4] = {0x5a, 0x6b, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_au8, dev_st.rx_data_au8, 4);

	const cevent_tst* e_pst = q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_SPI_MASTER_RESULT_SUCCESS, e_pst->sig);
}



TEST_GROUP_RUNNER(spi_master)
{
	RUN_TEST_CASE(spi_master, init);
	RUN_TEST_CASE(spi_master, send_1b);
	RUN_TEST_CASE(spi_master, send_2b);
	//RUN_TEST_CASE(spi_master, init);
	//RUN_TEST_CASE(spi_master, init);
	//RUN_TEST_CASE(spi_master, init);
	//RUN_TEST_CASE(spi_master, init);
	//RUN_TEST_CASE(spi_master, init);

}
