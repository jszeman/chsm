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

const cevent_tst*		fram_events_apst[12];
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

static void crf_run()
{
	drv_tick(1000);
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
	chsm_ctor(&fram_st.super, fram_top, fram_events_apst, 8, 4);
	
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

/* read_2b_from_page_0:
 * Trigger a 2 byte read from page 0 and check the result.
 */
TEST(fram, read_2b_from_page_0)
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
		.len_u32 = 2,
		.q_pst = &q_st,
	};

    drv_mock_st.slave_pst = &dev_st;

	CRF_POST(&mem_op_st, &fram_st);

	crf_run();

	uint8_t expected_buff_au8[4] = {0xa5, 0x90, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buff_au8, buff_au8, 4);

	uint8_t expected_rx_data_au8[4] = {0xf0, 0x02, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 4);

	TEST_ASSERT_EQUAL(2, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(2, dev_st.tx_idx_u16);

	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_READ_SUCCESS, ms_pst->super.sig);
}

/* read_16b_from_page_0:
 * Trigger a 16 byte read from page 0 and check the result.
 */
TEST(fram, read_16b_from_page_0)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {
			0, 1, 2, 3, 4, 5, 6, 7,
			8, 9, 10, 11, 12, 13, 14, 15},
		.rx_data_au8 = {0},
    };

	uint8_t buff_au8[32] = {0};

	mem_op_tst mem_op_st = {
		.super.sig = SIG_MEM_READ,
		.address_u32 = 0xf002,
		.buff_pu8 = buff_au8,
		.len_u32 = 16,
		.q_pst = &q_st,
	};

    drv_mock_st.slave_pst = &dev_st;

	CRF_POST(&mem_op_st, &fram_st);

	crf_run();

    TEST_ASSERT_EQUAL_HEX8_ARRAY(dev_st.tx_data_au8, buff_au8, 32);

	uint8_t expected_rx_data_au8[4] = {0xf0, 0x02, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 4);

	TEST_ASSERT_EQUAL(2, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(16, dev_st.tx_idx_u16);

	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_READ_SUCCESS, ms_pst->super.sig);
}

/* read_1b_from_page_1:
 * Trigger a 1 byte read from page 1 and check the result. The page select bit is
 * the LSB of the address.
 */
TEST(fram, read_1b_from_page_1)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x13,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0xa5, 0x90, 0xf3, 0x80, 0x0c, 0x80},
		.rx_data_au8 = {0},
    };

	uint8_t buff_au8[4] = {0};

	mem_op_tst mem_op_st = {
		.super.sig = SIG_MEM_READ,
		.address_u32 = 0x1f002,
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

/* write_1b_to_page_0:
 * Trigger a 1 byte write to page 0 and check the result.
 */
TEST(fram, write_1b_to_page_0)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0},
		.rx_data_au8 = {0},
    };

	uint8_t buff_au8[4] = {0x5a, 0x6b, 0, 0};

	mem_op_tst mem_op_st = {
		.super.sig = SIG_MEM_WRITE,
		.address_u32 = 0xf002,
		.buff_pu8 = buff_au8,
		.len_u32 = 1,
		.q_pst = &q_st,
	};

    drv_mock_st.slave_pst = &dev_st;

	CRF_POST(&mem_op_st, &fram_st);

	crf_run();

	uint8_t expected_rx_data_au8[4] = {0xf0, 0x02, 0x5a, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 4);

	TEST_ASSERT_EQUAL(3, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(0, dev_st.tx_idx_u16);

	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_WRITE_SUCCESS, ms_pst->super.sig);
}

/* write_2b_to_page_0:
 * Trigger a 2 byte write to page 0 and check the result.
 */
TEST(fram, write_2b_to_page_0)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0},
		.rx_data_au8 = {0},
    };

	uint8_t buff_au8[4] = {0x5a, 0x6b, 0x7c, 0};

	mem_op_tst mem_op_st = {
		.super.sig = SIG_MEM_WRITE,
		.address_u32 = 0xf002,
		.buff_pu8 = buff_au8,
		.len_u32 = 2,
		.q_pst = &q_st,
	};

    drv_mock_st.slave_pst = &dev_st;

	CRF_POST(&mem_op_st, &fram_st);

	crf_run();

	uint8_t expected_rx_data_au8[8] = {0xf0, 0x02, 0x5a, 0x6b, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 8);

	TEST_ASSERT_EQUAL(4, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(0, dev_st.tx_idx_u16);

	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_WRITE_SUCCESS, ms_pst->super.sig);
}

/* write_16b_to_page_0:
 * Trigger a 16 byte write to page 0 and check the result.
 */
TEST(fram, write_16b_to_page_0)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0},
		.rx_data_au8 = {0},
    };

	uint8_t buff_au8[32] = {0, 1, 2, 3, 4, 5, 6, 7,
							8, 9, 10, 11, 12, 13, 14, 15};

	mem_op_tst mem_op_st = {
		.super.sig = SIG_MEM_WRITE,
		.address_u32 = 0xf002,
		.buff_pu8 = buff_au8,
		.len_u32 = 16,
		.q_pst = &q_st,
	};

    drv_mock_st.slave_pst = &dev_st;

	CRF_POST(&mem_op_st, &fram_st);

	crf_run();

	uint8_t expected_rx_data_au8[32] = {0xf0, 0x02, 0, 1, 2, 3, 4, 5, 6, 7,
							8, 9, 10, 11, 12, 13, 14, 15};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 32);

	TEST_ASSERT_EQUAL(18, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(0, dev_st.tx_idx_u16);

	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_WRITE_SUCCESS, ms_pst->super.sig);
}

/* write_1b_to_page_1:
 * Trigger a 1 byte write to page 1 and check the result.
 */
TEST(fram, write_1b_to_page_1)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x13,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0},
		.rx_data_au8 = {0},
    };

	uint8_t buff_au8[4] = {0x5a, 0x6b, 0, 0};

	mem_op_tst mem_op_st = {
		.super.sig = SIG_MEM_WRITE,
		.address_u32 = 0x1f002,
		.buff_pu8 = buff_au8,
		.len_u32 = 1,
		.q_pst = &q_st,
	};

    drv_mock_st.slave_pst = &dev_st;

	CRF_POST(&mem_op_st, &fram_st);

	crf_run();

	uint8_t expected_rx_data_au8[4] = {0xf0, 0x02, 0x5a, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 4);

	TEST_ASSERT_EQUAL(3, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(0, dev_st.tx_idx_u16);

	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_WRITE_SUCCESS, ms_pst->super.sig);
}

/* read_1b_addr_nack:
 * Trigger a 1 byte read with the wrong slave address. Check for the fault message.
 */
TEST(fram, read_1b_addr_nack)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x13,
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

	uint8_t expected_buff_au8[4] = {0, 0, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buff_au8, buff_au8, 4);

	uint8_t expected_rx_data_au8[4] = {0, 0, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 4);

	TEST_ASSERT_EQUAL(0, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(0, dev_st.tx_idx_u16);

	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_READ_FAIL, ms_pst->super.sig);
	TEST_ASSERT_EQUAL(MEM_EC_HW_ACCESS_ERROR, ms_pst->error_en);
	TEST_ASSERT_EQUAL(0xf002, ms_pst->address_u32);
}

/* read_1b_data_nack:
 * Trigger a 1 byte read with with invalid address that will be NACKd by the slave. Check for the fault message.
 */
TEST(fram, read_1b_data_nack)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x13,
        .nack_idx_u16 = 1,
		.tx_data_au8 = {0xa5, 0x90, 0xf3, 0x80, 0x0c, 0x80},
		.rx_data_au8 = {0},
    };

	uint8_t buff_au8[4] = {0};

	mem_op_tst mem_op_st = {
		.super.sig = SIG_MEM_READ,
		.address_u32 = 0x2f002,
		.buff_pu8 = buff_au8,
		.len_u32 = 1,
		.q_pst = &q_st,
	};

    drv_mock_st.slave_pst = &dev_st;

	CRF_POST(&mem_op_st, &fram_st);

	crf_run();

	uint8_t expected_buff_au8[4] = {0, 0, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_buff_au8, buff_au8, 4);

	uint8_t expected_rx_data_au8[4] = {0xf0, 0x02, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 4);

	TEST_ASSERT_EQUAL(2, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(0, dev_st.tx_idx_u16);

	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_READ_FAIL, ms_pst->super.sig);
	TEST_ASSERT_EQUAL(MEM_EC_INVALID_ADDRESS, ms_pst->error_en);
	TEST_ASSERT_EQUAL(0x2f002, ms_pst->address_u32);
}

/* write_1b_addr_nack:
 * Trigger a 1 byte write with the wrong slave address.
 */
TEST(fram, write_1b_addr_nack)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x13,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0},
		.rx_data_au8 = {0},
    };

	uint8_t buff_au8[4] = {0x5a, 0x6b, 0, 0};

	mem_op_tst mem_op_st = {
		.super.sig = SIG_MEM_WRITE,
		.address_u32 = 0xf002,
		.buff_pu8 = buff_au8,
		.len_u32 = 1,
		.q_pst = &q_st,
	};

    drv_mock_st.slave_pst = &dev_st;

	CRF_POST(&mem_op_st, &fram_st);

	crf_run();

	uint8_t expected_rx_data_au8[4] = {0, 0, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 4);

	TEST_ASSERT_EQUAL(0, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(0, dev_st.tx_idx_u16);

	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_WRITE_FAIL, ms_pst->super.sig);
	TEST_ASSERT_EQUAL(MEM_EC_HW_ACCESS_ERROR, ms_pst->error_en);
	TEST_ASSERT_EQUAL(0xf002, ms_pst->address_u32);
}

/* write_1b_data_nack:
 * Trigger a 1 byte write with invalid memory address
 */
TEST(fram, write_1b_data_nack)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x13,
        .nack_idx_u16 = 1,
		.tx_data_au8 = {0},
		.rx_data_au8 = {0},
    };

	uint8_t buff_au8[4] = {0x5a, 0x6b, 0, 0};

	mem_op_tst mem_op_st = {
		.super.sig = SIG_MEM_WRITE,
		.address_u32 = 0x2f002,
		.buff_pu8 = buff_au8,
		.len_u32 = 1,
		.q_pst = &q_st,
	};

    drv_mock_st.slave_pst = &dev_st;

	CRF_POST(&mem_op_st, &fram_st);

	crf_run();

	uint8_t expected_rx_data_au8[4] = {0xf0, 0x02, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 4);

	TEST_ASSERT_EQUAL(2, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(0, dev_st.tx_idx_u16);

	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_WRITE_FAIL, ms_pst->super.sig);
	TEST_ASSERT_EQUAL(MEM_EC_INVALID_ADDRESS, ms_pst->error_en);
	TEST_ASSERT_EQUAL(0x2f002, ms_pst->address_u32);
}

/* write_17b_to_page_0:
 * Trigger a 17 byte write to page 0 and check that the write is
 * really chopped up into two pieces.
 */
TEST(fram, write_17b_to_page_0)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 50,
		.tx_data_au8 = {0},
		.rx_data_au8 = {0},
    };

	uint8_t buff_au8[32] = {0, 1, 2, 3, 4, 5, 6, 7,
							8, 9, 10, 11, 12, 13, 14, 15, 16};

	mem_op_tst mem_op_st = {
		.super.sig = SIG_MEM_WRITE,
		.address_u32 = 0xf002,
		.buff_pu8 = buff_au8,
		.len_u32 = 17,
		.q_pst = &q_st,
	};

    drv_mock_st.slave_pst = &dev_st;

	CRF_POST(&mem_op_st, &fram_st);

	crf_run();

	TEST_ASSERT_EQUAL(21, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(0, dev_st.tx_idx_u16);

	uint8_t expected_rx_data_au8[32] = {0xf0, 0x02, 0, 1, 2, 3, 4, 5, 6, 7,
							8, 9, 10, 11, 12, 13, 14, 15, 0xf0, 0x12, 16};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 32);


	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_WRITE_SUCCESS, ms_pst->super.sig);
}

/* write_33b_to_page_0:
 * Trigger a 33 byte write to page 0 and check that the write is
 * really chopped up into 3 pieces.
 */
TEST(fram, write_33b_to_page_0)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 50,
		.tx_data_au8 = {0},
		.rx_data_au8 = {0},
    };

	uint8_t buff_au8[64] = {0,  1,  2,  3,  4,  5,  6,  7,
							8,  9,  10, 11, 12, 13, 14, 15,
							16, 17, 18, 19, 20, 21, 22, 23,
							24, 25, 26, 27, 28, 29, 30, 31,
							32};

	mem_op_tst mem_op_st = {
		.super.sig = SIG_MEM_WRITE,
		.address_u32 = 0xf002,
		.buff_pu8 = buff_au8,
		.len_u32 = 33,
		.q_pst = &q_st,
	};

    drv_mock_st.slave_pst = &dev_st;

	CRF_POST(&mem_op_st, &fram_st);

	crf_run();

	TEST_ASSERT_EQUAL(39, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(0, dev_st.tx_idx_u16);

	uint8_t expected_rx_data_au8[64] = {
		0xf0, 0x02, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
		0xf0, 0x12, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
		0xf0, 0x22, 32};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 64);

	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_WRITE_SUCCESS, ms_pst->super.sig);
}

/* read_17b_from_page_0:
 * Trigger a 17 byte read from page 0 and check that the read is really
 * chopped up into 2 transactions.
 */
TEST(fram, read_17b)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0,  1,  2,  3,  4,  5,  6,  7,
						8,  9,  10, 11, 12, 13, 14, 15,
						16},
		.rx_data_au8 = {0},
    };

	uint8_t buff_au8[32] = {0};

	mem_op_tst mem_op_st = {
		.super.sig = SIG_MEM_READ,
		.address_u32 = 0xf002,
		.buff_pu8 = buff_au8,
		.len_u32 = 17,
		.q_pst = &q_st,
	};

    drv_mock_st.slave_pst = &dev_st;

	CRF_POST(&mem_op_st, &fram_st);

	crf_run();

    TEST_ASSERT_EQUAL_UINT8_ARRAY(dev_st.tx_data_au8, buff_au8, 32);

	uint8_t expected_rx_data_au8[8] = {0xf0, 0x02, 0xf0, 0x12};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 8);

	TEST_ASSERT_EQUAL(4, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(17, dev_st.tx_idx_u16);

	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_READ_SUCCESS, ms_pst->super.sig);
}

/* read_33b_from_page_0:
 * Trigger a 33 byte read from page 0 and check that the read is really
 * chopped up into 3 transactions.
 */
TEST(fram, read_33b)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 20,
		.tx_data_au8 = {0,  1,  2,  3,  4,  5,  6,  7,
						8,  9,  10, 11, 12, 13, 14, 15,
						16, 17, 18, 19, 20, 21, 22, 23,
						24, 25, 26, 27, 28, 29, 30, 31,
						32},
		.rx_data_au8 = {0},
    };

	uint8_t buff_au8[64] = {0};

	mem_op_tst mem_op_st = {
		.super.sig = SIG_MEM_READ,
		.address_u32 = 0xf002,
		.buff_pu8 = buff_au8,
		.len_u32 = 33,
		.q_pst = &q_st,
	};

    drv_mock_st.slave_pst = &dev_st;

	CRF_POST(&mem_op_st, &fram_st);

	crf_run();

    TEST_ASSERT_EQUAL_UINT8_ARRAY(dev_st.tx_data_au8, buff_au8, 64);

	uint8_t expected_rx_data_au8[8] = {0xf0, 0x02, 0xf0, 0x12, 0xf0, 0x22};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 8);

	TEST_ASSERT_EQUAL(6, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(33, dev_st.tx_idx_u16);

	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_READ_SUCCESS, ms_pst->super.sig);
}

/* parallel_read_33b:
 * Trigger a two 33 byte reads from page 0 and check that both operations finish successfu.
 */
TEST(fram, parallel_read_33b)
{
	i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 100,
		.tx_data_au8 = {0},
		.rx_data_au8 = {0},
    };

	for (int i=0; i<66; i++)
	{
		dev_st.tx_data_au8[i] = i;
	}

	uint8_t expected1_au8[64] = {0};
	uint8_t expected2_au8[64] = {0};

	uint8_t buff1_au8[64] = {0};
	uint8_t buff2_au8[64] = {0};

	for (int i=0; i<33; i++)
	{
		expected1_au8[i] = i;
		expected2_au8[i] = i + 33;
	}

	mem_op_tst mem_op1_st = {
		.super.sig = SIG_MEM_READ,
		.address_u32 = 0xf002,
		.buff_pu8 = buff1_au8,
		.len_u32 = 33,
		.q_pst = &q_st,
	};

	mem_op_tst mem_op2_st = {
		.super.sig = SIG_MEM_READ,
		.address_u32 = 0xc1ca,
		.buff_pu8 = buff2_au8,
		.len_u32 = 33,
		.q_pst = &q_st,
	};

    drv_mock_st.slave_pst = &dev_st;

	CRF_POST(&mem_op1_st, &fram_st);
	CRF_POST(&mem_op2_st, &fram_st);

	crf_run();

	uint8_t expected_rx_data_au8[32] = {0xf0, 0x02, 0xf0, 0x12, 0xf0, 0x22, 0xc1, 0xca, 0xc1, 0xda, 0xc1, 0xea};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data_au8, dev_st.rx_data_au8, 32);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected1_au8, buff1_au8, 64);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected2_au8, buff2_au8, 64);

	TEST_ASSERT_EQUAL(12, dev_st.rx_idx_u16);
	TEST_ASSERT_EQUAL(66, dev_st.tx_idx_u16);

	const mem_status_tst* ms_pst;

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_READ_SUCCESS, ms_pst->super.sig);

	ms_pst = (const mem_status_tst*)q_st.get(&q_st);
	TEST_ASSERT_NOT_NULL(ms_pst);
	TEST_ASSERT_EQUAL(SIG_MEM_READ_SUCCESS, ms_pst->super.sig);
}

TEST_GROUP_RUNNER(fram)
{
	RUN_TEST_CASE(fram, init);
	RUN_TEST_CASE(fram, read_1b_from_page_0);
	RUN_TEST_CASE(fram, read_2b_from_page_0);
	RUN_TEST_CASE(fram, read_16b_from_page_0);
	RUN_TEST_CASE(fram, read_1b_from_page_1);
	RUN_TEST_CASE(fram, write_1b_to_page_0);
	RUN_TEST_CASE(fram, write_2b_to_page_0);
	RUN_TEST_CASE(fram, write_16b_to_page_0);
	RUN_TEST_CASE(fram, write_1b_to_page_1);
	RUN_TEST_CASE(fram, read_1b_addr_nack);
	RUN_TEST_CASE(fram, read_1b_data_nack);
	RUN_TEST_CASE(fram, write_1b_addr_nack);
	RUN_TEST_CASE(fram, write_1b_data_nack);
	RUN_TEST_CASE(fram, write_17b_to_page_0);
	RUN_TEST_CASE(fram, write_33b_to_page_0);
	RUN_TEST_CASE(fram, read_17b);
	RUN_TEST_CASE(fram, read_33b);
	RUN_TEST_CASE(fram, parallel_read_33b);
	//RUN_TEST_CASE(fram, init);
	//RUN_TEST_CASE(fram, init);
	//RUN_TEST_CASE(fram, init);
	//RUN_TEST_CASE(fram, init);
	//RUN_TEST_CASE(fram, init);

}
