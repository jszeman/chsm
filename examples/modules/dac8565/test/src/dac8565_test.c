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

TEST_GROUP(dac8565);

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

TEST_SETUP(dac8565)
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

TEST_TEAR_DOWN(dac8565)
{
}

/*
 * Just call setup
 */
TEST(dac8565, init)
{
}

/* set_outputs:
		Set all outputs at once.
 */
TEST(dac8565, set_outputs)
{
    spi_mock_slave_device_tst dev_st = {0};

    drv_mock_st.slave_pst = &dev_st;
	drv_mock_st.slave_count_u16 = 1;

	
}




TEST_GROUP_RUNNER(dac8565)
{
	RUN_TEST_CASE(dac8565, init);
	RUN_TEST_CASE(dac8565, set_outputs);
	//RUN_TEST_CASE(dac8565, init);
	//RUN_TEST_CASE(dac8565, init);

}
