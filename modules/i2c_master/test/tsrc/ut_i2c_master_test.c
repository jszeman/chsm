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

/* send_1b:
 * Send one byte to the correct slave address and assert the following:
 * 	- The data was written to the device (via the mock driver)
 *  - The device is not busy after the operation was finsihed
 *  - The master sent back a success message (put it in the queue designated by the transaction)
 */
TEST(i2c_master, send_1b)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 1
    };

    drv_mock_st.slave_pst = &dev_st;

	uint8_t data_au8[4] = {0x5a, 0x6b, 0, 0};

	i2c_transaction_tst*	t_pst = CRF_NEW(SIG_I2C_W_TRANSACTION);

	TEST_ASSERT(t_pst);

	*t_pst = (i2c_transaction_tst){
		.super.sig = SIG_I2C_W_TRANSACTION,
		.target_q_pst = &q_st,
		.read_cnt_u16 = 0,
		.write_cnt_u16 = 1,
		.slave_addr_u16 = 0x12,
		.write_data_pu8 = data_au8
	};

	TEST_ASSERT_EQUAL(SIG_I2C_W_TRANSACTION, t_pst->super.sig);

	CRF_POST(t_pst, &i2c_master_st);

	drv_tick(5);

	TEST_ASSERT_FALSE(dev_st.busy_b);

	uint8_t expected_au8[4] = {0x5a, 0, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_au8, dev_st.rx_data_au8, 2);

	const cevent_tst* e_pst = q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_I2C_RESULT_SUCCESS, e_pst->sig);
}

/* send_2b:
 * Send two bytes to the correct slave address and assert the following:
 * 	- The data was written to the device (via the mock driver)
 *  - The device is not busy after the operation was finsihed
 *  - The master sent back a success message (put it in the queue designated by the transaction)
 */
TEST(i2c_master, send_2b)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 2
    };

    drv_mock_st.slave_pst = &dev_st;

	uint8_t data_au8[4] = {0x5a, 0x6b, 0x7c, 0};

	i2c_transaction_tst*	t_pst = CRF_NEW(SIG_I2C_W_TRANSACTION);

	TEST_ASSERT(t_pst);

	*t_pst = (i2c_transaction_tst){
		.super.sig = SIG_I2C_W_TRANSACTION,
		.target_q_pst = &q_st,
		.read_cnt_u16 = 0,
		.write_cnt_u16 = 2,
		.slave_addr_u16 = 0x12,
		.write_data_pu8 = data_au8
	};

	CRF_POST(t_pst, &i2c_master_st);

	drv_tick(5);

	TEST_ASSERT_FALSE(dev_st.busy_b);

	uint8_t expected_au8[4] = {0x5a, 0x6b, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_au8, dev_st.rx_data_au8, 4);

	const cevent_tst* e_pst = q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_I2C_RESULT_SUCCESS, e_pst->sig);
}

/* send_1b_addr_nack:
 * Send one byte to the wrong slave address and assert the following:
 * 	- The data was not written to the slave
 *  - The device is not busy
 *	- The master sent back an address NACK message (put it in the queue designated by the transaction)
 */

TEST(i2c_master, send_1b_addr_nack)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x13,
        .nack_idx_u16 = 1
    };

    drv_mock_st.slave_pst = &dev_st;

	uint8_t data_au8[4] = {0x5a, 0x6b, 0, 0};

	i2c_transaction_tst*	t_pst = CRF_NEW(SIG_I2C_W_TRANSACTION);

	TEST_ASSERT(t_pst);

	*t_pst = (i2c_transaction_tst){
		.super.sig = SIG_I2C_W_TRANSACTION,
		.target_q_pst = &q_st,
		.read_cnt_u16 = 0,
		.write_cnt_u16 = 1,
		.slave_addr_u16 = 0x12,
		.write_data_pu8 = data_au8
	};

	CRF_POST(t_pst, &i2c_master_st);

	while(CRF_STEP())
	{
		printf("|");
	}

	TEST_ASSERT_FALSE(dev_st.busy_b);

	uint8_t expected_au8[4] = {0, 0, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_au8, dev_st.rx_data_au8, 2);

	const cevent_tst* e_pst = q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_I2C_RESULT_ADDR_NACK, e_pst->sig);
}

/* send_1b_data_nack:
 * Send one byte to a slave that NACKs the data and assert the following:
 * 	- The data was not written to the slave
 *  - The device is not busy
 *	- The master sent back a data NACK message (put it in the queue designated by the transaction)
 */

TEST(i2c_master, send_1b_data_nack)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 0
    };

    drv_mock_st.slave_pst = &dev_st;

	uint8_t data_au8[4] = {0x5a, 0x6b, 0, 0};

	i2c_transaction_tst*	t_pst = CRF_NEW(SIG_I2C_W_TRANSACTION);

	TEST_ASSERT(t_pst);

	*t_pst = (i2c_transaction_tst){
		.super.sig = SIG_I2C_W_TRANSACTION,
		.target_q_pst = &q_st,
		.read_cnt_u16 = 0,
		.write_cnt_u16 = 1,
		.slave_addr_u16 = 0x12,
		.write_data_pu8 = data_au8
	};

	CRF_POST(t_pst, &i2c_master_st);

	drv_tick(5);

	TEST_ASSERT_FALSE(dev_st.busy_b);

	uint8_t expected_au8[4] = {0x5a, 0, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_au8, dev_st.rx_data_au8, 2);

	const cevent_tst* e_pst = q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_I2C_RESULT_DATA_NACK, e_pst->sig);
}

/* send_2x_1b:
 * Send two different one byte write transactions to the master with the same
 * slave address and assert the following:
 * 	- The device is not busy
 * 	- The correct data was written into the rx buffer of the slave device
 * 	- The master sent two success responses
 */
TEST(i2c_master, send_2x_1b)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 4
    };

    drv_mock_st.slave_pst = &dev_st;

	uint8_t data1_au8[4] = {0x5a, 0x6b, 0, 0};
	uint8_t data2_au8[4] = {0x7c, 0x8d, 0, 0};

	i2c_transaction_tst*	t1_pst = CRF_NEW(SIG_I2C_W_TRANSACTION);

	TEST_ASSERT(t1_pst);

	*t1_pst = (i2c_transaction_tst){
		.super.sig = SIG_I2C_W_TRANSACTION,
		.target_q_pst = &q_st,
		.read_cnt_u16 = 0,
		.write_cnt_u16 = 1,
		.slave_addr_u16 = 0x12,
		.write_data_pu8 = data1_au8,
	};

	i2c_transaction_tst*	t2_pst = CRF_NEW(SIG_I2C_W_TRANSACTION);

	TEST_ASSERT(t2_pst);

	*t2_pst = (i2c_transaction_tst){
		.super.sig = SIG_I2C_W_TRANSACTION,
		.target_q_pst = &q_st,
		.read_cnt_u16 = 0,
		.write_cnt_u16 = 2,
		.slave_addr_u16 = 0x12,
		.write_data_pu8 = data2_au8,
	};

	CRF_POST(t1_pst, &i2c_master_st);
	CRF_POST(t2_pst, &i2c_master_st);

	drv_tick(10);

	TEST_ASSERT_FALSE(dev_st.busy_b);

	uint8_t expected_au8[4] = {0x5a, 0x7c, 0x8d, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_au8, dev_st.rx_data_au8, 4);

	const cevent_tst* e_pst = q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_I2C_RESULT_SUCCESS, e_pst->sig);

	e_pst = q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_I2C_RESULT_SUCCESS, e_pst->sig);
}

/* read_1b:
 * Read one byte from the slave and assert the following:
 * 	- The correct data was read into the read buffer
 *  - The slave device is not busy
 * 	- The master sent a success response
 */
TEST(i2c_master, read_1b)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 4,
		.tx_data_au8 = {0x5a, 0x6b}
    };

    drv_mock_st.slave_pst = &dev_st;

	uint8_t data1_au8[4] = {0};

	i2c_transaction_tst*	t1_pst = CRF_NEW(SIG_I2C_R_TRANSACTION);

	TEST_ASSERT(t1_pst);

	*t1_pst = (i2c_transaction_tst){
		.super.sig = SIG_I2C_R_TRANSACTION,
		.target_q_pst = &q_st,
		.read_cnt_u16 = 1,
		.write_cnt_u16 = 0,
		.slave_addr_u16 = 0x12,
		.write_data_pu8 = NULL,
		.read_data_pu8 = data1_au8,
	};

	CRF_POST(t1_pst, &i2c_master_st);

	drv_tick(5);

	TEST_ASSERT_FALSE(dev_st.busy_b);

	uint8_t expected_au8[4] = {0x5a, 0, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_au8, data1_au8, 4);

	const cevent_tst* e_pst = q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_I2C_RESULT_SUCCESS, e_pst->sig);
}

/* read_2b:
 * Read two bytes from the slave and assert the following:
 * 	- The correct data was read into the read buffer
 *  - The slave device is not busy
 * 	- The master sent a success response
 */
TEST(i2c_master, read_2b)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 4,
		.tx_data_au8 = {0x5a, 0x6b, 0x7c}
    };

    drv_mock_st.slave_pst = &dev_st;

	uint8_t data1_au8[4] = {0};

	i2c_transaction_tst*	t1_pst = CRF_NEW(SIG_I2C_R_TRANSACTION);

	TEST_ASSERT(t1_pst);

	*t1_pst = (i2c_transaction_tst){
		.super.sig = SIG_I2C_R_TRANSACTION,
		.target_q_pst = &q_st,
		.read_cnt_u16 = 2,
		.write_cnt_u16 = 0,
		.slave_addr_u16 = 0x12,
		.write_data_pu8 = NULL,
		.read_data_pu8 = data1_au8,
	};

	CRF_POST(t1_pst, &i2c_master_st);

	drv_tick(5);

	TEST_ASSERT_FALSE(dev_st.busy_b);

	uint8_t expected_au8[4] = {0x5a, 0x6b, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_au8, data1_au8, 4);

	const cevent_tst* e_pst = q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_I2C_RESULT_SUCCESS, e_pst->sig);
}

/* read_1b_addr_nack:
 * Try to read one byte from the wrong slave address and assert the following:
 * 	- No data was read into the read buffer
 *  - The slave device is not busy
 * 	- The master sent an address NACK response
 */
TEST(i2c_master, read_1b_addr_nack)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x13,
        .nack_idx_u16 = 4,
		.tx_data_au8 = {0x5a, 0x6b}
    };

    drv_mock_st.slave_pst = &dev_st;

	uint8_t data1_au8[4] = {0};

	i2c_transaction_tst*	t1_pst = CRF_NEW(SIG_I2C_R_TRANSACTION);

	TEST_ASSERT(t1_pst);

	*t1_pst = (i2c_transaction_tst){
		.super.sig = SIG_I2C_R_TRANSACTION,
		.target_q_pst = &q_st,
		.read_cnt_u16 = 1,
		.write_cnt_u16 = 0,
		.slave_addr_u16 = 0x12,
		.write_data_pu8 = NULL,
		.read_data_pu8 = data1_au8,
	};

	CRF_POST(t1_pst, &i2c_master_st);

	drv_tick(5);

	TEST_ASSERT_FALSE(dev_st.busy_b);

	uint8_t expected_au8[4] = {0, 0, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_au8, data1_au8, 4);

	const cevent_tst* e_pst = q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_I2C_RESULT_ADDR_NACK, e_pst->sig);
}

/* write_2b_read_2b:
 * Send two bytes to the slave, restart and read two bytes then assert the following:
 * 	- The data was written to the slave
 * 	- The data was read from the slave
 * 	- The slave is not busy
 * 	- The master put a success event in the queue designated by the transaction
 */
TEST(i2c_master, write_2b_read_2b)
{
    i2c_mock_slave_device_tst dev_st = {
        .address_u8 = 0x12,
        .nack_idx_u16 = 2,
		.tx_data_au8 = {0x5a, 0x6b, 0x7c},
		.rx_data_au8 = {0}
    };

    drv_mock_st.slave_pst = &dev_st;

	uint8_t wr_data1_au8[4] = {0x8d, 0x9e, 0xaf, 0};
	uint8_t rd_data1_au8[4] = {0};

	i2c_transaction_tst*	t1_pst = CRF_NEW(SIG_I2C_WR_TRANSACTION);

	TEST_ASSERT(t1_pst);

	*t1_pst = (i2c_transaction_tst){
		.super.sig = SIG_I2C_WR_TRANSACTION,
		.target_q_pst = &q_st,
		.read_cnt_u16 = 2,
		.write_cnt_u16 = 2,
		.slave_addr_u16 = 0x12,
		.write_data_pu8 = wr_data1_au8,
		.read_data_pu8 = rd_data1_au8,
	};

	CRF_POST(t1_pst, &i2c_master_st);

	drv_tick(10);

	TEST_ASSERT_FALSE(dev_st.busy_b);

	uint8_t expected_read_au8[4] = {0x5a, 0x6b, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_read_au8, rd_data1_au8, 4);

	uint8_t expected_write_au8[4] = {0x8d, 0x9e, 0, 0};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_write_au8, dev_st.rx_data_au8, 4);

	const cevent_tst* e_pst = q_st.get(&q_st);
	TEST_ASSERT(e_pst);
	TEST_ASSERT_EQUAL(SIG_I2C_RESULT_SUCCESS, e_pst->sig);
}

TEST_GROUP_RUNNER(i2c_master)
{
	RUN_TEST_CASE(i2c_master, init);
	RUN_TEST_CASE(i2c_master, send_1b);
	RUN_TEST_CASE(i2c_master, send_2b);
	RUN_TEST_CASE(i2c_master, send_1b_addr_nack);
	RUN_TEST_CASE(i2c_master, send_1b_data_nack);
	RUN_TEST_CASE(i2c_master, send_2x_1b);
	RUN_TEST_CASE(i2c_master, read_1b);
	RUN_TEST_CASE(i2c_master, read_2b);
	RUN_TEST_CASE(i2c_master, read_1b_addr_nack);
	RUN_TEST_CASE(i2c_master, write_2b_read_2b);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);
	//RUN_TEST_CASE(i2c_master, init);

}
