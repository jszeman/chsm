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
#include "sys_if.h"
#include "sdo_common.h"

TEST_GROUP(co);

static const cevent_tst*		node_events_apst[12];
static co_node_tst				node_st;


static co_node_tst *self = 	&node_st;

static chsm_tst*				hsm_apst[] = {
	(chsm_tst*)(&node_st),
	NULL};

static uint8_t 				pool_buff_au8[1024];
static cpool_tst				pool_ast[1];

static const cevent_tst*       events_apst[4];
static cqueue_tst              q_st;

static const cevent_tst*       can_drv_events_apst[4];
static cqueue_tst              can_drv_st;

static uint8_t					obj_u8;
static uint8_t					obj_ro_u8;
static uint16_t				obj_u16;
static uint32_t				obj_u32;
static char					str_ac[32] = "String object";

static inline void send_sdo_request(uint8_t cmd_u8, uint32_t mlx_u32, uint32_t data_u32)
{
	can_frame_tst* f_pst;

	f_pst = CRF_NEW(SIG_CAN_FRAME);
	TEST_ASSERT_NOT_NULL(f_pst);

	f_pst->header_un = CAN_HDR(CO_SDO_RX + node_st.config_st.node_id_u8, 0, 8);
	f_pst->mdl_un.bit_st.d0_u8 = cmd_u8;
	f_pst->mdl_un.bit_st.d1_u8 = (mlx_u32 >> 8) & 0xff;
	f_pst->mdl_un.bit_st.d2_u8 = (mlx_u32 >> 16) & 0xff;;
	f_pst->mdl_un.bit_st.d3_u8 = mlx_u32 & 0xff;
	f_pst->mdh_un.all_u32 = data_u32;

	CRF_POST(f_pst, &node_st);
}

static inline void test_sdo_response(uint8_t cmd_u8, uint32_t mlx_u32, uint32_t data_u32)
{
	const can_frame_tst *e_pst;

	e_pst = (const can_frame_tst *)can_drv_st.get(&can_drv_st);

	TEST_ASSERT_NOT_NULL(e_pst);
	TEST_ASSERT_EQUAL(SIG_CAN_FRAME, e_pst->super.sig);
	
	TEST_ASSERT_EQUAL_HEX(node_st.config_st.node_id_u8 + CO_SDO_TX, e_pst->header_un.bit_st.id_u12);
	TEST_ASSERT_EQUAL(8, e_pst->header_un.bit_st.dlc_u4);
	TEST_ASSERT_EQUAL(0, e_pst->header_un.bit_st.rtr_u1);
	TEST_ASSERT_EQUAL(cmd_u8,	e_pst->mdl_un.bit_st.d0_u8);
	TEST_ASSERT_EQUAL((mlx_u32 >> 8) & 0xff, 	e_pst->mdl_un.bit_st.d1_u8);
	TEST_ASSERT_EQUAL((mlx_u32 >> 16) & 0xff, 	e_pst->mdl_un.bit_st.d2_u8);
	TEST_ASSERT_EQUAL(mlx_u32 & 0xff, 			e_pst->mdl_un.bit_st.d3_u8);
	TEST_ASSERT_EQUAL_HEX32(data_u32, e_pst->mdh_un.all_u32);
	CRF_GC(e_pst);
}

#define MLX_U8_RW 		0x123400
#define MLX_U16_RW 		0x123500
#define MLX_U32_RW 		0x123600
#define MLX_STR_RW 		0x123700
#define MLX_U8_RO 		0x123800
#define MLX_NOT_EXISTS	0x183400

static od_entry_tst			od_entries_ast[] = {
	OD_ENTRY_DEF(MLX_U8_RW, 	obj_u8, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_ENTRY_DEF(MLX_U16_RW, 	obj_u16, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_ENTRY_DEF(MLX_U32_RW, 	obj_u32, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_ENTRY_DEF(MLX_STR_RW, 	str_ac, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_ENTRY_DEF(MLX_U8_RO, 	obj_ro_u8, OD_ATTR_READ),
	OD_ENTRY_TERMINATOR,
};

static object_dictionary_tst	od_st = {
	.mem_drv_pst = 		NULL,
	.mlx_mask_u32 = 	0xffffffff,
	.objects_ast = 		od_entries_ast
};

static void co_send(chsm_tst *self, const cevent_tst *e_pst)
{
    //printf("\n%s\n", __FUNCTION__);
	switch(e_pst->sig)
	{
		case SIG_CAN_FRAME:
			CRF_POST(e_pst, &can_drv_st);
			break;

		default:
			CRF_POST(e_pst, &q_st);
	}
}

static const cevent_tst		tick_1ms_st = {.sig = SIG_SYS_TICK_1ms};

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

static void node_init(void)
{
	const can_frame_tst *e_pst;

	chsm_init((chsm_tst *)&node_st);
	e_pst = (can_frame_tst *)can_drv_st.get(&can_drv_st);
	CRF_GC(e_pst);
}

TEST_SETUP(co)
{
	obj_u8 = 0;
	obj_u16 = 0;
	obj_u32 = 0;
	obj_ro_u8 = 0x3c;
	strcpy(str_ac, "String object");

    memset(&node_events_apst, 0, sizeof node_events_apst);
    memset(&node_st, 0, sizeof node_st);
	
	chsm_ctor(&node_st.super, co_node_top, node_events_apst, 8, 4);
	
	node_st.config_st = (co_node_cfg_tst){
		.node_id_u8 = 0x11,
		.od_pst = &od_st,
		.guard_time_ms_u16 = 10,
		.life_time_factor_u16 = 2,
		};

	node_st.super.send = co_send;
}

TEST_TEAR_DOWN(co)
{
}

/* init
 * Init the CRF. It is only done once, but that should not be a problem,
 * as long as all resources are properly garbage collected. So in a way,
 * this is also a test for the framework.
 */
TEST(co, init)
{
    memset(&pool_buff_au8, 0, sizeof pool_buff_au8);
    memset(&pool_ast, 0, sizeof pool_ast);

    memset(events_apst, 0, sizeof events_apst);
    memset(&q_st, 0, sizeof q_st);
    cqueue_init(&q_st, events_apst, 4);

    memset(can_drv_events_apst, 0, sizeof can_drv_events_apst);
    memset(&can_drv_st, 0, sizeof can_drv_st);
    cqueue_init(&can_drv_st, can_drv_events_apst, 4);

    memset(&crf, 0, sizeof crf);
	cpool_init(pool_ast+0, pool_buff_au8, 24, 16);
	crf_init(&crf, hsm_apst, pool_ast, 1);
}

/* bootup
 * Check that the bootup message is sent after init.
 */
TEST(co, bootup)
{
	const can_frame_tst *e_pst;

	chsm_init((chsm_tst *)&node_st);

	e_pst = (const can_frame_tst *)can_drv_st.get(&can_drv_st);

	TEST_ASSERT_NOT_NULL(e_pst);
	TEST_ASSERT_EQUAL(SIG_CAN_FRAME, e_pst->super.sig);
	
	TEST_ASSERT_EQUAL_HEX(node_st.config_st.node_id_u8 + CO_NMT, e_pst->header_un.bit_st.id_u12);
	TEST_ASSERT_EQUAL(1, e_pst->header_un.bit_st.dlc_u4);
	TEST_ASSERT_EQUAL(0, e_pst->header_un.bit_st.rtr_u1);
	TEST_ASSERT_EQUAL_HEX(0, e_pst->mdl_un.bit_st.d0_u8);

	CRF_GC(e_pst);
}

/* nodeguard_req
 * Check that the node responds to a nodeguard request
 */
TEST(co, nodeguard_req)
{
	const can_frame_tst *e_pst;
	can_frame_tst *ng_pst;

	node_init();

	ng_pst = CRF_NEW(SIG_CAN_FRAME);
	TEST_ASSERT_NOT_NULL(ng_pst);

	ng_pst->header_un = CAN_HDR(CO_NMT + self->config_st.node_id_u8, 1, 0);
	CRF_POST(ng_pst, &node_st);

	tick_ms(1);

	e_pst = (const can_frame_tst *)can_drv_st.get(&can_drv_st);
	TEST_ASSERT_NOT_NULL(e_pst);
	TEST_ASSERT_EQUAL(SIG_CAN_FRAME, e_pst->super.sig);
	TEST_ASSERT_EQUAL_HEX(node_st.config_st.node_id_u8 + CO_NMT, e_pst->header_un.bit_st.id_u12);
	TEST_ASSERT_EQUAL(1, e_pst->header_un.bit_st.dlc_u4);
	TEST_ASSERT_EQUAL(0, e_pst->header_un.bit_st.rtr_u1);
	TEST_ASSERT_EQUAL_HEX(0x7f, e_pst->mdl_un.bit_st.d0_u8);
	CRF_GC(e_pst);

	e_pst = (const can_frame_tst *)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(e_pst);
	TEST_ASSERT_EQUAL(SIG_CANOPEN_NG_ACTIVE, e_pst->super.sig);
	CRF_GC(e_pst);
}

/* nodeguard_toggle
 * Check that the node responds to a nodeguard request and toggles the
 * MSB in consecutive responses
 */
TEST(co, nodeguard_toggle)
{
	const can_frame_tst *e_pst;
	can_frame_tst *ng_pst;

	node_init();

	for (int i=0; i<10; i++)
	{
		ng_pst = CRF_NEW(SIG_CAN_FRAME);
		TEST_ASSERT_NOT_NULL(ng_pst);

		ng_pst->header_un = CAN_HDR(CO_NMT + self->config_st.node_id_u8, 1, 0);
		CRF_POST(ng_pst, &node_st);

		tick_ms(1);

		e_pst = (const can_frame_tst *)can_drv_st.get(&can_drv_st);

		TEST_ASSERT_NOT_NULL(e_pst);
		TEST_ASSERT_EQUAL(SIG_CAN_FRAME, e_pst->super.sig);
		
		TEST_ASSERT_EQUAL_HEX(node_st.config_st.node_id_u8 + CO_NMT, e_pst->header_un.bit_st.id_u12);
		TEST_ASSERT_EQUAL(1, e_pst->header_un.bit_st.dlc_u4);
		TEST_ASSERT_EQUAL(0, e_pst->header_un.bit_st.rtr_u1);
		TEST_ASSERT_EQUAL_HEX(0x7f | (i & 1 ? 0x80 : 0), e_pst->mdl_un.bit_st.d0_u8);
		CRF_GC(e_pst);
	}

	e_pst = (const can_frame_tst *)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(e_pst);
	TEST_ASSERT_EQUAL(SIG_CANOPEN_NG_ACTIVE, e_pst->super.sig);
	CRF_GC(e_pst);
}

/* nodeguard_timeout
 * Check that the node sends an NG_INACTIVE notification if no NG request arrives within the lifetime
 */
TEST(co, nodeguard_timeout)
{
	const can_frame_tst *e_pst;
	can_frame_tst *ng_pst;

	node_init();

	for (int i=0; i<10; i++)
	{
		ng_pst = CRF_NEW(SIG_CAN_FRAME);
		TEST_ASSERT_NOT_NULL(ng_pst);

		ng_pst->header_un = CAN_HDR(CO_NMT + self->config_st.node_id_u8, 1, 0);
		CRF_POST(ng_pst, &node_st);

		tick_ms(1);

		e_pst = (const can_frame_tst *)can_drv_st.get(&can_drv_st);

		TEST_ASSERT_NOT_NULL(e_pst);
		TEST_ASSERT_EQUAL(SIG_CAN_FRAME, e_pst->super.sig);
		
		TEST_ASSERT_EQUAL_HEX(node_st.config_st.node_id_u8 + CO_NMT, e_pst->header_un.bit_st.id_u12);
		TEST_ASSERT_EQUAL(1, e_pst->header_un.bit_st.dlc_u4);
		TEST_ASSERT_EQUAL(0, e_pst->header_un.bit_st.rtr_u1);
		TEST_ASSERT_EQUAL_HEX(0x7f | (i & 1 ? 0x80 : 0), e_pst->mdl_un.bit_st.d0_u8);
		CRF_GC(e_pst);
	}

	e_pst = (const can_frame_tst *)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(e_pst);
	TEST_ASSERT_EQUAL(SIG_CANOPEN_NG_ACTIVE, e_pst->super.sig);
	CRF_GC(e_pst);

	tick_ms(node_st.config_st.guard_time_ms_u16 * node_st.config_st.life_time_factor_u16 - 1);

	e_pst = (const can_frame_tst *)q_st.get(&q_st);
	TEST_ASSERT_NULL(e_pst);
	
	tick_ms(1);

	e_pst = (const can_frame_tst *)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(e_pst);
	TEST_ASSERT_EQUAL(SIG_CANOPEN_NG_INACTIVE, e_pst->super.sig);
	CRF_GC(e_pst);
}

/* nmt_reset
 * Check that on_nmt_reset function is called when an NMT reset frame is received
 */
TEST(co, nmt_reset)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;

	node_init();

	f_pst = CRF_NEW(SIG_CAN_FRAME);
	TEST_ASSERT_NOT_NULL(f_pst);

	f_pst->header_un = CAN_HDR(0, 0, 2);
	f_pst->mdl_un.bit_st.d0_u8 = CO_NMT_CMD_RESET;
	f_pst->mdl_un.bit_st.d1_u8 = node_st.config_st.node_id_u8;
	CRF_POST(f_pst, &node_st);

	tick_ms(1);

	e_pst = (const can_frame_tst *)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(e_pst);
	TEST_ASSERT_EQUAL(SIG_SYS_RESET, e_pst->super.sig);
	CRF_GC(e_pst);
}



/* nmt_start
 * Check that a START command changes the code in NG responses
 */
TEST(co, nmt_start)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;
	can_frame_tst *ng_pst;

	node_init();

	f_pst = CRF_NEW(SIG_CAN_FRAME);
	TEST_ASSERT_NOT_NULL(f_pst);

	f_pst->header_un = CAN_HDR(0, 0, 2);
	f_pst->mdl_un.bit_st.d0_u8 = CO_NMT_CMD_START;
	f_pst->mdl_un.bit_st.d1_u8 = node_st.config_st.node_id_u8;
	CRF_POST(f_pst, &node_st);

	tick_ms(1);

	for (int i=0; i<10; i++)
	{
		ng_pst = CRF_NEW(SIG_CAN_FRAME);
		TEST_ASSERT_NOT_NULL(ng_pst);

		ng_pst->header_un = CAN_HDR(CO_NMT + self->config_st.node_id_u8, 1, 0);
		CRF_POST(ng_pst, &node_st);

		tick_ms(1);

		e_pst = (const can_frame_tst *)can_drv_st.get(&can_drv_st);

		TEST_ASSERT_NOT_NULL(e_pst);
		TEST_ASSERT_EQUAL(SIG_CAN_FRAME, e_pst->super.sig);
		
		TEST_ASSERT_EQUAL_HEX(node_st.config_st.node_id_u8 + CO_NMT, e_pst->header_un.bit_st.id_u12);
		TEST_ASSERT_EQUAL(1, e_pst->header_un.bit_st.dlc_u4);
		TEST_ASSERT_EQUAL(0, e_pst->header_un.bit_st.rtr_u1);
		TEST_ASSERT_EQUAL_HEX(0x05 | (i & 1 ? 0x80 : 0), e_pst->mdl_un.bit_st.d0_u8);
		CRF_GC(e_pst);
	}

	e_pst = (const can_frame_tst *)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(e_pst);
	TEST_ASSERT_EQUAL(SIG_CANOPEN_NG_ACTIVE, e_pst->super.sig);
	CRF_GC(e_pst);
}



/* nmt_preop
 * Check that a PREOP command changes the code in NG responses
 */
TEST(co, nmt_preop)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;
	can_frame_tst *ng_pst;

	node_init();

	/* Go to STARTED state */
	f_pst = CRF_NEW(SIG_CAN_FRAME);
	TEST_ASSERT_NOT_NULL(f_pst);

	f_pst->header_un = CAN_HDR(0, 0, 2);
	f_pst->mdl_un.bit_st.d0_u8 = CO_NMT_CMD_START;
	f_pst->mdl_un.bit_st.d1_u8 = node_st.config_st.node_id_u8;
	CRF_POST(f_pst, &node_st);

	tick_ms(1);

	/* Go to PREOP state */
	f_pst = CRF_NEW(SIG_CAN_FRAME);
	TEST_ASSERT_NOT_NULL(f_pst);

	f_pst->header_un = CAN_HDR(0, 0, 2);
	f_pst->mdl_un.bit_st.d0_u8 = CO_NMT_CMD_PREOP;
	f_pst->mdl_un.bit_st.d1_u8 = node_st.config_st.node_id_u8;
	CRF_POST(f_pst, &node_st);

	tick_ms(1);

	for (int i=0; i<10; i++)
	{
		ng_pst = CRF_NEW(SIG_CAN_FRAME);
		TEST_ASSERT_NOT_NULL(ng_pst);

		ng_pst->header_un = CAN_HDR(CO_NMT + self->config_st.node_id_u8, 1, 0);
		CRF_POST(ng_pst, &node_st);

		tick_ms(1);

		e_pst = (const can_frame_tst *)can_drv_st.get(&can_drv_st);

		TEST_ASSERT_NOT_NULL(e_pst);
		TEST_ASSERT_EQUAL(SIG_CAN_FRAME, e_pst->super.sig);
		
		TEST_ASSERT_EQUAL_HEX(node_st.config_st.node_id_u8 + CO_NMT, e_pst->header_un.bit_st.id_u12);
		TEST_ASSERT_EQUAL(1, e_pst->header_un.bit_st.dlc_u4);
		TEST_ASSERT_EQUAL(0, e_pst->header_un.bit_st.rtr_u1);
		TEST_ASSERT_EQUAL_HEX(CO_NMT_STATE_PREOP | (i & 1 ? 0x80 : 0), e_pst->mdl_un.bit_st.d0_u8);
		CRF_GC(e_pst);
	}

	e_pst = (const can_frame_tst *)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(e_pst);
	TEST_ASSERT_EQUAL(SIG_CANOPEN_NG_ACTIVE, e_pst->super.sig);
	CRF_GC(e_pst);
}

/* sdo_dl_exp_1b
 * Initiate an expedited 1 byte download and check that the value was correctly written.
 */
TEST(co, sdo_dl_exp_1b)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;

	node_init();

	send_sdo_request(CO_SDO_DL_REQ_EXP_1B, MLX_U8_RW, 0xa5);

	tick_ms(1);

	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_U8_RW, 0);

	TEST_ASSERT_EQUAL_HEX32(0xa5, obj_u8);
}

/* sdo_abort_missing_od
 * Initiate an expedited 1 byte without a valid object dictionaty pointer and
 * check that the correct abort code was sent.
 */
TEST(co, sdo_abort_missing_od)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;

	node_st.config_st.od_pst = NULL;

	node_init();

	send_sdo_request(CO_SDO_DL_REQ_EXP_1B, MLX_U8_RW, 0xa5);

	tick_ms(1);

	test_sdo_response(CO_SDO_ABORT, MLX_U8_RW, CO_SDO_ABORT_NO_OBJ_DICT);
	
	TEST_ASSERT_EQUAL_HEX32(0, obj_u8);
}

/* sdo_abort_obj_not_found
 * Initiate an expedited 1 byte download with a nonexistent mlx and check that
 * the correct abort code was sent.
 */
TEST(co, sdo_abort_obj_not_found)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;

	node_init();

	
	send_sdo_request(CO_SDO_DL_REQ_EXP_1B, MLX_NOT_EXISTS, 0xa5);

	tick_ms(1);

	test_sdo_response(CO_SDO_ABORT, MLX_NOT_EXISTS, CO_SDO_ABORT_OBJECT_NOT_FOUND);
	
	TEST_ASSERT_EQUAL_HEX32(0, obj_u8);
}

/* sdo_abort_obj_read_only
 * Initiate an expedited 1 byte download with read only mlx and check that the
 * correct abort code was sent.
 */
TEST(co, sdo_abort_obj_read_only)
{
	const can_frame_tst *e_pst;

	node_init();

	send_sdo_request(CO_SDO_DL_REQ_EXP_1B, MLX_U8_RO, 0xa5);

	tick_ms(1);

	test_sdo_response(CO_SDO_ABORT, MLX_U8_RO, CO_SDO_ABORT_READ_ONLY_OBJECT);
	
	TEST_ASSERT_EQUAL_HEX32(0x3c, obj_ro_u8);
}

/* sdo_abort_obj_len_mismatch
 * Initiate an expedited 1 byte download to a longer than 1 mlx and check that
 * correct abort code was sent.
 */
TEST(co, sdo_abort_obj_len_mismatch)
{
	const can_frame_tst *e_pst;

	node_init();

	send_sdo_request(CO_SDO_DL_REQ_EXP_1B, MLX_U16_RW, 0xa5);

	tick_ms(1);

	test_sdo_response(CO_SDO_ABORT, MLX_U16_RW, CO_SDO_ABORT_LENGTH_MISMATCH);
	
	TEST_ASSERT_EQUAL_HEX32(0, obj_u16);
}

/* sdo_dl_exp_2b
 * Initiate an expedited 2 byte download and check that the value was correctly written.
 */
TEST(co, sdo_dl_exp_2b)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;

	node_init();

	send_sdo_request(CO_SDO_DL_REQ_EXP_2B, MLX_U16_RW, 0xa5b6);

	tick_ms(1);

	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_U16_RW, 0);
	
	TEST_ASSERT_EQUAL_HEX32(0xa5b6, obj_u16);
}

/* sdo_dl_exp_4b
 * Initiate an expedited 4 byte download and check that the value was correctly written.
 */
TEST(co, sdo_dl_exp_4b)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;

	node_init();

	send_sdo_request(CO_SDO_DL_REQ_EXP_4B, MLX_U32_RW, 0xa5b6c7d8);

	tick_ms(1);

	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_U32_RW, 0);
	
	TEST_ASSERT_EQUAL_HEX32(0xa5b6c7d8, obj_u32);
}

/* sdo_ul_exp_1b
 * Initiate an expedited 1 byte upload and check that the value was correctly read.
 */
TEST(co, sdo_ul_exp_1b)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;

	node_init();

	obj_u8 = 0xa5;

	send_sdo_request(CO_SDO_UL_REQ, MLX_U8_RW, 0);

	tick_ms(1);

	test_sdo_response(CO_SDO_UL_RESP_EXP_1B, MLX_U8_RW, 0xa5);
}

/* sdo_ul_exp_2b
 * Initiate an expedited 2 byte upload and check that the value was correctly read.
 */
TEST(co, sdo_ul_exp_2b)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;

	node_init();

	obj_u16 = 0xa5b6;

	send_sdo_request(CO_SDO_UL_REQ, MLX_U16_RW, 0);

	tick_ms(1);

	test_sdo_response(CO_SDO_UL_RESP_EXP_2B, MLX_U16_RW, 0xa5b6);
}

/* sdo_ul_exp_4b
 * Initiate an expedited 4 byte upload and check that the value was correctly read.
 */
TEST(co, sdo_ul_exp_4b)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;

	node_init();

	obj_u32 = 0xa5b6c7d8;

	send_sdo_request(CO_SDO_UL_REQ, MLX_U32_RW, 0);

	tick_ms(1);

	test_sdo_response(CO_SDO_UL_RESP_EXP_4B, MLX_U32_RW, 0xa5b6c7d8);
}


TEST_GROUP_RUNNER(co)
{
	RUN_TEST_CASE(co, init);
	RUN_TEST_CASE(co, bootup);
	RUN_TEST_CASE(co, nodeguard_req);
	RUN_TEST_CASE(co, nodeguard_toggle);
	RUN_TEST_CASE(co, nodeguard_timeout);
	RUN_TEST_CASE(co, nmt_reset);
	RUN_TEST_CASE(co, nmt_start);
	RUN_TEST_CASE(co, nmt_preop);
	RUN_TEST_CASE(co, sdo_dl_exp_1b);
	RUN_TEST_CASE(co, sdo_abort_missing_od);
	RUN_TEST_CASE(co, sdo_abort_obj_not_found);
	RUN_TEST_CASE(co, sdo_abort_obj_read_only);
	RUN_TEST_CASE(co, sdo_abort_obj_len_mismatch);
	RUN_TEST_CASE(co, sdo_dl_exp_2b);
	RUN_TEST_CASE(co, sdo_dl_exp_4b);
	RUN_TEST_CASE(co, sdo_ul_exp_1b);
	RUN_TEST_CASE(co, sdo_ul_exp_2b);
	RUN_TEST_CASE(co, sdo_ul_exp_4b);
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
