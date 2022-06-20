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

TEST_GROUP(od);

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

typedef struct data_tst
{
 	uint8_t				obj_u8;
 	uint8_t				obj_ro_u8;
 	uint16_t			obj_u16;
 	uint32_t			obj_u32;
 	char				str_ac[32];
} data_tst;

data_tst d1;
data_tst d2;

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
	TEST_ASSERT_EQUAL_MESSAGE(8, e_pst->header_un.bit_st.dlc_u4, "DLC");
	TEST_ASSERT_EQUAL_MESSAGE(0, e_pst->header_un.bit_st.rtr_u1, "RTR");
	TEST_ASSERT_EQUAL_HEX_MESSAGE(cmd_u8,	e_pst->mdl_un.bit_st.d0_u8, "CMD");
	TEST_ASSERT_EQUAL_HEX_MESSAGE((mlx_u32 >> 8) & 0xff, 	e_pst->mdl_un.bit_st.d1_u8, "INDEX_LSB");
	TEST_ASSERT_EQUAL_HEX_MESSAGE((mlx_u32 >> 16) & 0xff, 	e_pst->mdl_un.bit_st.d2_u8, "INDEX_MSB");
	TEST_ASSERT_EQUAL_HEX_MESSAGE(mlx_u32 & 0xff, 			e_pst->mdl_un.bit_st.d3_u8, "SUBINDEX");
	TEST_ASSERT_EQUAL_HEX32_MESSAGE(data_u32, e_pst->mdh_un.all_u32, "DATA");
	CRF_GC(e_pst);
}

static inline void send_sdo_segment(uint8_t t_bit_u8, uint8_t size_u8, uint8_t last_u8, uint8_t *data_pu8)
{
	can_frame_tst* f_pst;
	uint8_t *dst_pu8;

	f_pst = CRF_NEW(SIG_CAN_FRAME);
	TEST_ASSERT_NOT_NULL(f_pst);

	f_pst->header_un = CAN_HDR(CO_SDO_RX + node_st.config_st.node_id_u8, 0, 8);
	f_pst->mdl_un.bit_st.d0_u8 = CO_SDO_DL_SEG_REQ(t_bit_u8, size_u8, last_u8);

	dst_pu8 = (uint8_t *)(&f_pst->mdl_un.all_u32);
	dst_pu8++;

	while (size_u8--)
	{
		*dst_pu8++ = *data_pu8++;
	}

	CRF_POST(f_pst, &node_st);
}

static inline void test_sdo_seg_response(uint8_t t_bit_u8)
{
	const can_frame_tst *e_pst;

	e_pst = (const can_frame_tst *)can_drv_st.get(&can_drv_st);

	TEST_ASSERT_NOT_NULL(e_pst);
	TEST_ASSERT_EQUAL(SIG_CAN_FRAME, e_pst->super.sig);
	
	TEST_ASSERT_EQUAL_HEX(node_st.config_st.node_id_u8 + CO_SDO_TX, e_pst->header_un.bit_st.id_u12);
	TEST_ASSERT_EQUAL(8, e_pst->header_un.bit_st.dlc_u4);
	TEST_ASSERT_EQUAL(0, e_pst->header_un.bit_st.rtr_u1);
	TEST_ASSERT_EQUAL_HEX32(CO_SDO_DL_SEG_RESP(t_bit_u8), e_pst->mdl_un.all_u32);
	TEST_ASSERT_EQUAL_HEX32(0, e_pst->mdh_un.all_u32);
	CRF_GC(e_pst);
}

static inline void assert_sdo_ul_seg_response(uint8_t t_bit_u8, uint8_t size_u8, uint8_t last_u8, uint8_t* expected_pu8)
{
	const can_frame_tst *e_pst;
	uint8_t* dst_pu8;

	e_pst = (const can_frame_tst *)can_drv_st.get(&can_drv_st);

	TEST_ASSERT_NOT_NULL(e_pst);
	TEST_ASSERT_EQUAL(SIG_CAN_FRAME, e_pst->super.sig);
	
	TEST_ASSERT_EQUAL_HEX_MESSAGE(node_st.config_st.node_id_u8 + CO_SDO_TX, e_pst->header_un.bit_st.id_u12, "UL_SEG_CAN_ID");
	TEST_ASSERT_EQUAL_MESSAGE(8, e_pst->header_un.bit_st.dlc_u4, "UL_SEG_DLC");
	TEST_ASSERT_EQUAL_MESSAGE(0, e_pst->header_un.bit_st.rtr_u1, "UL_SEG_RTR");
	TEST_ASSERT_EQUAL_HEX32_MESSAGE(CO_SDO_UL_RESP_SEG(t_bit_u8, size_u8, last_u8), e_pst->mdl_un.bit_st.d0_u8, "UL_SEG_CMD");

	dst_pu8 = (uint8_t *)(&e_pst->mdl_un.all_u32);
	dst_pu8++;

	TEST_ASSERT_EQUAL_STRING_LEN(expected_pu8, dst_pu8, size_u8);

	CRF_GC(e_pst);
}

static inline void test_assert_no_response(void)
{
	const can_frame_tst *e_pst;

	e_pst = (const can_frame_tst *)can_drv_st.get(&can_drv_st);

	TEST_ASSERT_NULL(e_pst);
}

#define MLX_U8_RW 		0x123400
#define MLX_U16_RW 		0x123500
#define MLX_U32_RW 		0x123600
#define MLX_STR_RW 		0x123700
#define MLX_U8_RO 		0x123800
#define MLX_NOT_EXISTS	0x183400


#define MLX2_U8_RW 		0xa23400
#define MLX2_U16_RW 	0xa23500
#define MLX2_U32_RW 	0xa23600
#define MLX2_STR_RW 	0xa23700
#define MLX2_U8_RO 		0xa23800

static od_entry_tst			od_entries1_ast[] = {
	OD_ENTRY_DEF(MLX_U8_RW, 	d1.obj_u8, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_ENTRY_DEF(MLX_U16_RW, 	d1.obj_u16, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_ENTRY_DEF(MLX_U32_RW, 	d1.obj_u32, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_ENTRY_DEF(MLX_STR_RW, 	d1.str_ac, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_ENTRY_DEF(MLX_U8_RO, 	d1.obj_ro_u8, OD_ATTR_READ),
	OD_ENTRY_TERMINATOR,
};

static object_dictionary_tst	od1_st = {
	.mem_drv_pst = 		NULL,
	.mlx_mask_u32 = 	0xffffffff,
	.objects_ast = 		od_entries1_ast
};

static od_entry_tst			od_entries2_ast[] = {
	OD_ENTRY_DEF(MLX2_U8_RW, 	d2.obj_u8, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_ENTRY_DEF(MLX2_U16_RW, 	d2.obj_u16, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_ENTRY_DEF(MLX2_U32_RW, 	d2.obj_u32, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_EXTENSION(od1_st),
	OD_ENTRY_DEF(MLX2_STR_RW, 	d2.str_ac, OD_ATTR_READ | OD_ATTR_WRITE),
	OD_ENTRY_DEF(MLX2_U8_RO, 	d2.obj_ro_u8, OD_ATTR_READ),
	OD_ENTRY_TERMINATOR,
};

static object_dictionary_tst	od2_st = {
	.mem_drv_pst = 		NULL,
	.mlx_mask_u32 = 	0xffffffff,
	.objects_ast = 		od_entries2_ast
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

TEST_SETUP(od)
{
	d1.obj_u8 = 0;
	d1.obj_u16 = 0;
	d1.obj_u32 = 0;
	d1.obj_ro_u8 = 0x3c;
	strcpy(d1.str_ac, "D1 String object");

	d2.obj_u8 = 0;
	d2.obj_u16 = 0;
	d2.obj_u32 = 0;
	d2.obj_ro_u8 = 0x4d;
	strcpy(d2.str_ac, "D2 String object");

    memset(&node_events_apst, 0, sizeof node_events_apst);
    memset(&node_st, 0, sizeof node_st);
	
	chsm_ctor(&node_st.super, co_node_top, node_events_apst, 8, 4);
	
	node_st.config_st = (co_node_cfg_tst){
		.node_id_u8 = 0x11,
		.od_pst = &od2_st,
		.guard_time_ms_u16 = 10,
		.life_time_factor_u16 = 2,
		.sdo_timeout_ms_u32 = 10,
		};

	node_st.super.send = co_send;
}

TEST_TEAR_DOWN(od)
{
}

/* init
 * Init the CRF. It is only done once, but that should not be a problem,
 * as long as all resources are properly garbage collected. So in a way,
 * this is also a test for the framework.
 */
TEST(od, init)
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



/* sdo_dl_exp_1b
 * Initiate an expedited 1 byte download with an mlx that is in an extension
 * object dictionaryand check that the value was correctly written.
 */
TEST(od, sdo_dl_exp_1b)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;

	node_init();

	send_sdo_request(CO_SDO_DL_REQ_EXP_1B, MLX_U8_RW, 0xa5);

	tick_ms(1);

	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_U8_RW, 0);

	TEST_ASSERT_EQUAL_HEX32(0xa5, d1.obj_u8);
}

/* sdo_dl_seg_str
 * Perform a segmented download and check the responses from the slave.
 */
TEST(od, sdo_dl_seg_str)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;
	uint8_t seg1_au8[8] = "SDO seg";
	uint8_t seg2_au8[8] = "mented ";
	uint8_t seg3_au8[8] = "downloa";
	uint8_t seg4_au8[8] = "d\n    ";

	node_init();

	send_sdo_request(CO_SDO_DL_REQ_SEG_INIT_SI, MLX_STR_RW, 29);
	tick_ms(1);
	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_STR_RW, 0);

	send_sdo_segment(0, 7, 0, seg1_au8);
	tick_ms(1);
	test_sdo_seg_response(0);
	TEST_ASSERT_EQUAL_STRING_LEN(seg1_au8, d1.str_ac, 7);

	send_sdo_segment(1, 7, 0, seg2_au8);
	tick_ms(1);
	test_sdo_seg_response(1);
	TEST_ASSERT_EQUAL_STRING_LEN(seg2_au8, d1.str_ac+7, 7);

	send_sdo_segment(0, 7, 0, seg3_au8);
	tick_ms(1);
	test_sdo_seg_response(0);
	TEST_ASSERT_EQUAL_STRING_LEN(seg3_au8, d1.str_ac+14, 7);

	send_sdo_segment(1, 3, 1, seg4_au8);
	tick_ms(1);
	test_sdo_seg_response(1);
	TEST_ASSERT_EQUAL_STRING_LEN(seg4_au8, d1.str_ac+21, 3);

	/*
	* Initiate an expedited 1 byte download.
	* This test is inserted after a segmented download to make sure, that
	* the state machine returned to a state, where expedited transfers are
	* possible
	*/
	send_sdo_request(CO_SDO_DL_REQ_EXP_1B, MLX_U8_RW, 0xa5);
	tick_ms(1);
	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_U8_RW, 0);
	TEST_ASSERT_EQUAL_HEX32(0xa5, d1.obj_u8);
}

/* sdo_dl_seg_timeout
 * Start a segmented download and check that the node sends an abort if the
 * master waits too long between segments.
 */
TEST(od, sdo_dl_seg_timeout)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;
	uint8_t seg1_au8[8] = "SDO seg";
	uint8_t seg2_au8[8] = "mented ";
	uint8_t seg3_au8[8] = "downloa";
	uint8_t seg4_au8[8] = "d\n    ";

	node_init();

	send_sdo_request(CO_SDO_DL_REQ_SEG_INIT_SI, MLX_STR_RW, 29);
	tick_ms(1);
	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_STR_RW, 0);

	send_sdo_segment(0, 7, 0, seg1_au8);
	tick_ms(1);
	test_sdo_seg_response(0);
	TEST_ASSERT_EQUAL_STRING_LEN(seg1_au8, d1.str_ac, 7);

	send_sdo_segment(1, 7, 0, seg2_au8);
	tick_ms(1);
	test_sdo_seg_response(1);
	TEST_ASSERT_EQUAL_STRING_LEN(seg2_au8, d1.str_ac+7, 7);

	tick_ms(node_st.config_st.sdo_timeout_ms_u32);
	test_sdo_response(CO_SDO_ABORT, MLX_STR_RW, CO_SDO_ABORT_TIMEOUT);

	/*
	* Initiate an expedited 1 byte download.
	* This test is inserted after a segmented download to make sure, that
	* the state machine returned to a state, where expedited transfers are
	* possible
	*/
	send_sdo_request(CO_SDO_DL_REQ_EXP_1B, MLX_U8_RW, 0xa5);
	tick_ms(1);
	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_U8_RW, 0);
	TEST_ASSERT_EQUAL_HEX32(0xa5, d1.obj_u8);
}

/* sdo_dl_seg_toggle_bit_error
 * Start a segmented download and check that the node ignores any segments
 * with the same toggle bit as the previous
 * request.
 */
TEST(od, sdo_dl_seg_toggle_bit_error)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;
	uint8_t seg1_au8[8] = "SDO seg";
	uint8_t seg2_au8[8] = "mented ";
	uint8_t seg3_au8[8] = "downloa";
	uint8_t seg4_au8[8] = "d\n    ";

	node_init();

	send_sdo_request(CO_SDO_DL_REQ_SEG_INIT_SI, MLX_STR_RW, 29);
	tick_ms(1);
	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_STR_RW, 0);

	send_sdo_segment(0, 7, 0, seg1_au8);
	tick_ms(1);
	test_sdo_seg_response(0);
	TEST_ASSERT_EQUAL_STRING_LEN(seg1_au8, d1.str_ac, 7);

	send_sdo_segment(1, 7, 0, seg2_au8);
	tick_ms(1);
	test_sdo_seg_response(1);
	TEST_ASSERT_EQUAL_STRING_LEN(seg2_au8, d1.str_ac+7, 7);

	/* Send the previous segment again */
	send_sdo_segment(1, 7, 0, seg2_au8);
	tick_ms(1);
	test_assert_no_response();

	send_sdo_segment(0, 7, 0, seg3_au8);
	tick_ms(1);
	test_sdo_seg_response(0);
	TEST_ASSERT_EQUAL_STRING_LEN(seg3_au8, d1.str_ac+14, 7);

	send_sdo_segment(1, 3, 1, seg4_au8);
	tick_ms(1);
	test_sdo_seg_response(1);
	TEST_ASSERT_EQUAL_STRING_LEN(seg4_au8, d1.str_ac+21, 3);

	/*
	* Initiate an expedited 1 byte download.
	* This test is inserted after a segmented download to make sure, that
	* the state machine returned to a state, where expedited transfers are
	* possible
	*/
	send_sdo_request(CO_SDO_DL_REQ_EXP_1B, MLX_U8_RW, 0xa5);
	tick_ms(1);
	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_U8_RW, 0);
	TEST_ASSERT_EQUAL_HEX32(0xa5, d1.obj_u8);
}

/* sdo_dl_seg_str_too_many_frames
 * Perform a segmented download with more data that fits into the selected object
 * and check that the slave abort the transfer.
 */
TEST(od, sdo_dl_seg_str_too_many_frames)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;
	uint8_t seg1_au8[8] = "SDO seg";
	uint8_t seg2_au8[8] = "mented ";
	uint8_t seg3_au8[8] = "downloa";
	uint8_t seg4_au8[8] = "d\n    ";

	node_init();

	send_sdo_request(CO_SDO_DL_REQ_SEG_INIT_SI, MLX_STR_RW, 29);
	tick_ms(1);
	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_STR_RW, 0);

	send_sdo_segment(0, 7, 0, seg1_au8);
	tick_ms(1);
	test_sdo_seg_response(0);
	TEST_ASSERT_EQUAL_STRING_LEN(seg1_au8, d1.str_ac, 7);

	send_sdo_segment(1, 7, 0, seg2_au8);
	tick_ms(1);
	test_sdo_seg_response(1);
	TEST_ASSERT_EQUAL_STRING_LEN(seg2_au8, d1.str_ac+7, 7);

	send_sdo_segment(0, 7, 0, seg3_au8);
	tick_ms(1);
	test_sdo_seg_response(0);
	TEST_ASSERT_EQUAL_STRING_LEN(seg3_au8, d1.str_ac+14, 7);

	send_sdo_segment(1, 7, 0, seg4_au8);
	tick_ms(1);
	test_sdo_seg_response(1);
	TEST_ASSERT_EQUAL_STRING_LEN(seg4_au8, d1.str_ac+21, 7);

	send_sdo_segment(0, 7, 0, seg1_au8);
	tick_ms(1);
	test_sdo_response(CO_SDO_ABORT, MLX_STR_RW, CO_SDO_ABORT_LENGTH_TOO_HIGH);

	/*
	* Initiate an expedited 1 byte download.
	* This test is inserted after a segmented download to make sure, that
	* the state machine returned to a state, where expedited transfers are
	* possible
	*/
	send_sdo_request(CO_SDO_DL_REQ_EXP_1B, MLX_U8_RW, 0xa5);
	tick_ms(1);
	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_U8_RW, 0);
	TEST_ASSERT_EQUAL_HEX32(0xa5, d1.obj_u8);
}

/* sdo_dl_seg_str_new_transfer
 * Start a segmented download and in the middle of the transfer start an
 * expedited upload that aborts the segmented transfer.
 */
TEST(od, sdo_dl_seg_str_new_transfer)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;
	uint8_t seg1_au8[8] = "SDO seg";
	uint8_t seg2_au8[8] = "mented ";
	uint8_t seg3_au8[8] = "downloa";
	uint8_t seg4_au8[8] = "d\n    ";

	node_init();

	send_sdo_request(CO_SDO_DL_REQ_SEG_INIT_SI, MLX_STR_RW, 29);
	tick_ms(1);
	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_STR_RW, 0);

	send_sdo_segment(0, 7, 0, seg1_au8);
	tick_ms(1);
	test_sdo_seg_response(0);
	TEST_ASSERT_EQUAL_STRING_LEN(seg1_au8, d1.str_ac, 7);

	send_sdo_segment(1, 7, 0, seg2_au8);
	tick_ms(1);
	test_sdo_seg_response(1);
	TEST_ASSERT_EQUAL_STRING_LEN(seg2_au8, d1.str_ac+7, 7);

	send_sdo_segment(0, 7, 0, seg3_au8);
	tick_ms(1);
	test_sdo_seg_response(0);
	TEST_ASSERT_EQUAL_STRING_LEN(seg3_au8, d1.str_ac+14, 7);

	d1.obj_u8 = 0xa5;
	send_sdo_request(CO_SDO_UL_REQ, MLX_U8_RW, 0);
	tick_ms(1);
	test_sdo_response(CO_SDO_UL_RESP_EXP_1B, MLX_U8_RW, 0xa5);
}

/* sdo_dl_seg_str_abort
 * Start a segmented download and in the middle of the transfer send an abort.
 * Check that the slave is responding properly for expedited requests.
 */
TEST(od, sdo_dl_seg_str_abort)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;
	uint8_t seg1_au8[8] = "SDO seg";
	uint8_t seg2_au8[8] = "mented ";
	uint8_t seg3_au8[8] = "downloa";
	uint8_t seg4_au8[8] = "d\n    ";

	node_init();

	send_sdo_request(CO_SDO_DL_REQ_SEG_INIT_SI, MLX_STR_RW, 29);
	tick_ms(1);
	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_STR_RW, 0);

	send_sdo_segment(0, 7, 0, seg1_au8);
	tick_ms(1);
	test_sdo_seg_response(0);
	TEST_ASSERT_EQUAL_STRING_LEN(seg1_au8, d1.str_ac, 7);

	send_sdo_segment(1, 7, 0, seg2_au8);
	tick_ms(1);
	test_sdo_seg_response(1);
	TEST_ASSERT_EQUAL_STRING_LEN(seg2_au8, d1.str_ac+7, 7);

	send_sdo_segment(0, 7, 0, seg3_au8);
	tick_ms(1);
	test_sdo_seg_response(0);
	TEST_ASSERT_EQUAL_STRING_LEN(seg3_au8, d1.str_ac+14, 7);

	send_sdo_request(CO_SDO_ABORT, MLX_STR_RW, CO_SDO_ABORT_GENERAL_ERROR);
	tick_ms(1);

	d1.obj_u8 = 0xa5;
	send_sdo_request(CO_SDO_UL_REQ, MLX_U8_RW, 0);
	tick_ms(1);
	test_sdo_response(CO_SDO_UL_RESP_EXP_1B, MLX_U8_RW, 0xa5);
}

/* sdo_ul_seg_str
 * Perform a segmented upload and check the responses from the slave.
 */
TEST(od, sdo_ul_seg_str)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;
	uint8_t seg1_au8[8] = "";
	uint8_t seg2_au8[8] = "";
	uint8_t seg3_au8[8] = "";
	uint8_t seg4_au8[8] = "";

	node_init();

	send_sdo_request(CO_SDO_UL_REQ_SEG_INIT, MLX_STR_RW, 0);
	tick_ms(1);
	test_sdo_response(CO_SDO_UL_RESP_SEG_INIT_SI, MLX_STR_RW, 32);

	send_sdo_request(CO_SDO_UL_REQ_SEG(0), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(0, 7, 0, d1.str_ac);

	send_sdo_request(CO_SDO_UL_REQ_SEG(1), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(1, 7, 0, d1.str_ac+7);

	send_sdo_request(CO_SDO_UL_REQ_SEG(0), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(0, 7, 0, d1.str_ac+14);

	send_sdo_request(CO_SDO_UL_REQ_SEG(1), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(1, 7, 0, d1.str_ac+21);

	send_sdo_request(CO_SDO_UL_REQ_SEG(0), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(0, 4, 1, d1.str_ac+28);

	/*
	* Initiate an expedited 1 byte download.
	* This test is inserted after a segmented download to make sure, that
	* the state machine returned to a state, where expedited transfers are
	* possible
	*/

	send_sdo_request(CO_SDO_DL_REQ_EXP_1B, MLX_U8_RW, 0xa5);
	tick_ms(1);
	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_U8_RW, 0);
	TEST_ASSERT_EQUAL_HEX32(0xa5, d1.obj_u8);
}

/* sdo_ul_seg_str_new_transfer
 * Perform a segmented upload and check that the master can abort the transfer
 * by starting a new one.
 */
TEST(od, sdo_ul_seg_str_new_transfer)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;
	uint8_t seg1_au8[8] = "";
	uint8_t seg2_au8[8] = "";
	uint8_t seg3_au8[8] = "";
	uint8_t seg4_au8[8] = "";

	node_init();

	send_sdo_request(CO_SDO_UL_REQ_SEG_INIT, MLX_STR_RW, 0);
	tick_ms(1);
	test_sdo_response(CO_SDO_UL_RESP_SEG_INIT_SI, MLX_STR_RW, 32);

	send_sdo_request(CO_SDO_UL_REQ_SEG(0), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(0, 7, 0, d1.str_ac);

	send_sdo_request(CO_SDO_UL_REQ_SEG(1), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(1, 7, 0, d1.str_ac+7);

	send_sdo_request(CO_SDO_UL_REQ_SEG(0), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(0, 7, 0, d1.str_ac+14);

	d1.obj_u8 = 0xa5;
	send_sdo_request(CO_SDO_UL_REQ, MLX_U8_RW, 0);
	tick_ms(1);
	test_sdo_response(CO_SDO_UL_RESP_EXP_1B, MLX_U8_RW, 0xa5);
}

/* sdo_ul_seg_timeout
 * Start a segmented upload and check the slave will abort it if not all
 * segments are requested.
 */
TEST(od, sdo_ul_seg_timeout)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;
	uint8_t seg1_au8[8] = "";
	uint8_t seg2_au8[8] = "";
	uint8_t seg3_au8[8] = "";
	uint8_t seg4_au8[8] = "";

	node_init();

	send_sdo_request(CO_SDO_UL_REQ_SEG_INIT, MLX_STR_RW, 0);
	tick_ms(1);
	test_sdo_response(CO_SDO_UL_RESP_SEG_INIT_SI, MLX_STR_RW, 32);

	send_sdo_request(CO_SDO_UL_REQ_SEG(0), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(0, 7, 0, d1.str_ac);

	send_sdo_request(CO_SDO_UL_REQ_SEG(1), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(1, 7, 0, d1.str_ac+7);

	send_sdo_request(CO_SDO_UL_REQ_SEG(0), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(0, 7, 0, d1.str_ac+14);

	tick_ms(node_st.config_st.sdo_timeout_ms_u32);
	test_sdo_response(CO_SDO_ABORT, MLX_STR_RW, CO_SDO_ABORT_TIMEOUT);

	/*
	* Initiate an expedited 1 byte download.
	* This test is inserted after a segmented download to make sure, that
	* the state machine returned to a state, where expedited transfers are
	* possible
	*/

	send_sdo_request(CO_SDO_DL_REQ_EXP_1B, MLX_U8_RW, 0xa5);
	tick_ms(1);
	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_U8_RW, 0);
	TEST_ASSERT_EQUAL_HEX32(0xa5, d1.obj_u8);
}

/* sdo_ul_seg_toggle_bit_error
 * Perform a segmented upload and check that the slave repeats previously
 * sent data for frames with repeated toggle bit.
 */
TEST(od, sdo_ul_seg_toggle_bit_error)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;
	uint8_t seg1_au8[8] = "";
	uint8_t seg2_au8[8] = "";
	uint8_t seg3_au8[8] = "";
	uint8_t seg4_au8[8] = "";

	node_init();

	send_sdo_request(CO_SDO_UL_REQ_SEG_INIT, MLX_STR_RW, 0);
	tick_ms(1);
	test_sdo_response(CO_SDO_UL_RESP_SEG_INIT_SI, MLX_STR_RW, 32);

	send_sdo_request(CO_SDO_UL_REQ_SEG(0), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(0, 7, 0, d1.str_ac);

	send_sdo_request(CO_SDO_UL_REQ_SEG(1), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(1, 7, 0, d1.str_ac+7);

	send_sdo_request(CO_SDO_UL_REQ_SEG(0), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(0, 7, 0, d1.str_ac+14);

	/* Repeat the last, simulating an event, where the
	 * previous frame was not received by the master
	 */
	send_sdo_request(CO_SDO_UL_REQ_SEG(0), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(0, 7, 0, d1.str_ac+14);

	send_sdo_request(CO_SDO_UL_REQ_SEG(1), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(1, 7, 0, d1.str_ac+21);

	send_sdo_request(CO_SDO_UL_REQ_SEG(0), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(0, 4, 1, d1.str_ac+28);

	/*
	* Initiate an expedited 1 byte download.
	* This test is inserted after a segmented download to make sure, that
	* the state machine returned to a state, where expedited transfers are
	* possible
	*/

	send_sdo_request(CO_SDO_DL_REQ_EXP_1B, MLX_U8_RW, 0xa5);
	tick_ms(1);
	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_U8_RW, 0);
	TEST_ASSERT_EQUAL_HEX32(0xa5, d1.obj_u8);
}

/* sdo_ul_seg_str_abort
 * Perform a segmented upload and abort it in the middle. Check that the
 * slave responds to expedited requests.
 */
TEST(od, sdo_ul_seg_str_abort)
{
	const can_frame_tst *e_pst;
	can_frame_tst *f_pst;
	uint8_t seg1_au8[8] = "";
	uint8_t seg2_au8[8] = "";
	uint8_t seg3_au8[8] = "";
	uint8_t seg4_au8[8] = "";

	node_init();

	send_sdo_request(CO_SDO_UL_REQ_SEG_INIT, MLX_STR_RW, 0);
	tick_ms(1);
	test_sdo_response(CO_SDO_UL_RESP_SEG_INIT_SI, MLX_STR_RW, 32);

	send_sdo_request(CO_SDO_UL_REQ_SEG(0), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(0, 7, 0, d1.str_ac);

	send_sdo_request(CO_SDO_UL_REQ_SEG(1), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(1, 7, 0, d1.str_ac+7);

	send_sdo_request(CO_SDO_UL_REQ_SEG(0), 0, 0);
	tick_ms(1);
	assert_sdo_ul_seg_response(0, 7, 0, d1.str_ac+14);

	send_sdo_request(CO_SDO_ABORT, MLX_STR_RW, CO_SDO_ABORT_GENERAL_ERROR);
	tick_ms(1);

	/*
	* Initiate an expedited 1 byte download.
	* This test is inserted after a segmented download to make sure, that
	* the state machine returned to a state, where expedited transfers are
	* possible
	*/

	send_sdo_request(CO_SDO_DL_REQ_EXP_1B, MLX_U8_RW, 0xa5);
	tick_ms(1);
	test_sdo_response(CO_SDO_DL_RESP_EXP, MLX_U8_RW, 0);
	TEST_ASSERT_EQUAL_HEX32(0xa5, d1.obj_u8);
}

TEST_GROUP_RUNNER(od)
{
	RUN_TEST_CASE(od, init);
	RUN_TEST_CASE(od, sdo_dl_exp_1b);
	RUN_TEST_CASE(od, sdo_dl_seg_str);
	RUN_TEST_CASE(od, sdo_dl_seg_timeout);
	RUN_TEST_CASE(od, sdo_dl_seg_toggle_bit_error);
	RUN_TEST_CASE(od, sdo_dl_seg_str_too_many_frames);
	RUN_TEST_CASE(od, sdo_dl_seg_str_new_transfer);
	RUN_TEST_CASE(od, sdo_dl_seg_str_abort);
	//RUN_TEST_CASE(od, init);
	//RUN_TEST_CASE(od, init);
	//RUN_TEST_CASE(od, init);
	RUN_TEST_CASE(od, sdo_ul_seg_str);
	RUN_TEST_CASE(od, sdo_ul_seg_timeout);
	RUN_TEST_CASE(od, sdo_ul_seg_toggle_bit_error);
	RUN_TEST_CASE(od, sdo_ul_seg_str_new_transfer);
	RUN_TEST_CASE(od, sdo_ul_seg_str_abort);
	//RUN_TEST_CASE(od, init);
	//RUN_TEST_CASE(od, init);
	//RUN_TEST_CASE(od, init);
	//RUN_TEST_CASE(od, init);
	//RUN_TEST_CASE(od, init);
	//RUN_TEST_CASE(od, init);
	//RUN_TEST_CASE(od, init);
	//RUN_TEST_CASE(od, init);
	//RUN_TEST_CASE(od, init);
	//RUN_TEST_CASE(od, init);
	//RUN_TEST_CASE(od, init);

}
