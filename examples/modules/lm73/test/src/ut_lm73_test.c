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

TEST_GROUP(lm73);

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

	cpool_init(pool_ast+0, pool_buff_au8, 24, 16);

    cqueue_init(&q_st, events_apst, 4);

    ut_i2c_driver_mock_init(&drv_mock_st);

	i2c_master_st.config_st.driver_pst = drv_pst;
	chsm_ctor(&i2c_master_st.super, i2c_master_top, i2c_master_events_apst, 4, 4);

	chsm_init((chsm_tst *)&i2c_master_st);
	crf_init(&crf, hsm_apst, pool_ast, 1);
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

TEST_GROUP_RUNNER(lm73)
{
	RUN_TEST_CASE(lm73, init);
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
	//RUN_TEST_CASE(lm73, init);
	//RUN_TEST_CASE(lm73, init);
	//RUN_TEST_CASE(lm73, init);
	//RUN_TEST_CASE(lm73, init);
	//RUN_TEST_CASE(lm73, init);
	//RUN_TEST_CASE(lm73, init);

}
