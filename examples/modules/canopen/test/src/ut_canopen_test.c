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
#include "cevent.h"
#include "canopen.h"
#include "can_if.h"

TEST_GROUP(co);

const cevent_tst*		node_events_apst[12];
co_node_tst				node_st;


co_node_tst *self = 	&node_st;

chsm_tst*				hsm_apst[] = {
	(chsm_tst*)(&node_st),
	NULL};

uint8_t 				pool_buff_au8[1024];
cpool_tst				pool_ast[1];

crf_tst					crf;

const cevent_tst*       events_apst[4];
cqueue_tst              q_st;

uint8_t					obj_u8;
uint16_t				obj_u16;
uint32_t				obj_u32;
char					str_ac[32] = "String object";

od_entry_tst			od_entries_ast[] = {
	OD_ENTRY_DEF(0x123400, obj_u8, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_ENTRY_DEF(0x123500, obj_u16, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_ENTRY_DEF(0x123600, obj_u32, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_ENTRY_DEF(0x123700, str_ac, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_ENTRY_TERMINATOR,
};

object_dictionary_tst	od_st = {
	.mem_drv_pst = 		NULL,
	.mlx_mask_u32 = 	0xffffffff,
	.objects_ast = 		od_entries_ast
};


void co_send(chsm_tst *self, const cevent_tst *e_pst)
{
    //printf("\n%s\n", __FUNCTION__);
	switch(e_pst->sig)
	{
		default:
			CRF_POST(e_pst, &q_st);
	}
}

const cevent_tst		tick_1ms_st = {.sig = SIG_SYS_TICK_1ms};

static void tick_ms(uint32_t tick_cnt_u32)
{
	while(tick_cnt_u32--)
	{
		CRF_POST(&tick_1ms_st, &node_st);

		while(CRF_STEP())
		{
			printf("|");
		}
	}
}

TEST_SETUP(co)
{
    memset(events_apst, 0, sizeof events_apst);
    memset(&q_st, 0, sizeof q_st);

    memset(&pool_buff_au8, 0, sizeof pool_buff_au8);
    memset(&pool_ast, 0, sizeof pool_ast);

    memset(&node_events_apst, 0, sizeof node_events_apst);
    memset(&node_st, 0, sizeof node_st);
	
    memset(&crf, 0, sizeof crf);

	cpool_init(pool_ast+0, pool_buff_au8, 24, 16);

    cqueue_init(&q_st, events_apst, 4);

	chsm_ctor(&node_st.super, co_node_top, node_events_apst, 8, 4);
	
	node_st.config_st = (co_node_cfg_tst){
		.node_id_u8 = 0x11,
		};

	node_st.super.send = co_send;

	crf_init(&crf, hsm_apst, pool_ast, 1);
	chsm_init((chsm_tst *)&node_st);
}

TEST_TEAR_DOWN(co)
{
}

/*
 * Just call setup
 */
TEST(co, init)
{
	const can_frame_tst *e_pst;

	e_pst = (const can_frame_tst *)q_st.get(&q_st);

	TEST_ASSERT_NOT_NULL(e_pst)
	TEST_ASSERT_EQUAL(SIG_CAN_FRAME, e_pst->super.sig);
	
	TEST_ASSERT_EQUAL_HEX(node_st.config_st.node_id_u8 + CO_NMT, e_pst->header_un.bit_st.id_u12);
	TEST_ASSERT_EQUAL(1, e_pst->header_un.bit_st.dlc_u4);
	TEST_ASSERT_EQUAL(0, e_pst->header_un.bit_st.rtr_u1);
	TEST_ASSERT_EQUAL_HEX(0, e_pst->mdl_un.bit_st.d0_u8);
}

/* bootup
 * Check that the bootup message is sent after init.
 */
TEST(co, bootup)
{
}

TEST_GROUP_RUNNER(co)
{
	RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);
	//RUN_TEST_CASE(co, init);

}
